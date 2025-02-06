const os = require('os');
const fs = require('fs/promises');
const { existsSync, createWriteStream } = require('fs');
const path = require('path');
const https = require('https');
const { pipeline } = require('stream/promises');

const platform = os.platform();
const arch = os.arch();

// Mapeia a arquitetura do Node para os nomes de diretório do PDFium
const archMap = {
    'x64': 'x64',
    'ia32': 'x86',
    'arm64': 'arm64',
    'arm': 'arm'
};

// Mapeia o SO para o diretório correto
const osMap = {
    'win32': 'win',
    'linux': 'linux'
};

const pdfiumDir = path.join(__dirname, 'pdfium');
const currentOS = osMap[platform];
const currentArch = archMap[arch];

if (!currentOS || !currentArch) {
    console.error(`Unsupported platform (${platform}) or architecture (${arch})`);
    process.exit(1);
}

// Cria apenas os diretórios necessários para o SO atual
async function createRequiredDirs() {
    const libPath = path.join(pdfiumDir, 'lib', currentOS, currentArch);
    await fs.mkdir(libPath, { recursive: true });

    if (platform === 'win32') {
        await fs.mkdir(path.join(libPath, 'bin'), { recursive: true });
        await fs.mkdir(path.join(libPath, 'lib'), { recursive: true });
    } else if (platform === 'linux') {
        await fs.mkdir(path.join(libPath, 'lib'), { recursive: true });
    }
    console.log('PDFium directory created successfully');
}

async function downloadFile(url, destPath) {
    const tempPath = `${destPath}.tmp`;
    let fileStream = null;

    try {
        fileStream = createWriteStream(tempPath);

        await new Promise((resolve, reject) => {
            const request = https.get(url, {
                headers: {
                    'User-Agent': 'Node.js',
                    'Accept': '*/*'
                },
                followRedirect: true
            }, async (response) => {
                if (response.statusCode === 302 || response.statusCode === 301) {
                    const redirectUrl = response.headers.location;
                    const redirectReq = https.get(redirectUrl, {
                        headers: {
                            'User-Agent': 'Node.js',
                            'Accept': '*/*'
                        }
                    }, async (redirectResponse) => {
                        if (redirectResponse.statusCode !== 200) {
                            reject(new Error(`Failed to download after redirect: ${redirectResponse.statusCode}`));
                            return;
                        }

                        try {
                            await pipeline(redirectResponse, fileStream);
                            resolve();
                        } catch (err) {
                            reject(err);
                        }
                    });

                    redirectReq.on('error', reject);
                    return;
                }

                if (response.statusCode !== 200) {
                    reject(new Error(`Failed to download: ${response.statusCode}`));
                    return;
                }

                try {
                    await pipeline(response, fileStream);
                    resolve();
                } catch (err) {
                    reject(err);
                }
            });

            request.on('error', reject);
        });

        await new Promise((resolve) => {
            fileStream.end(() => resolve());
        });

        await fs.rename(tempPath, destPath);

    } catch (error) {
        if (fileStream) {
            fileStream.destroy();
        }
        if (existsSync(tempPath)) {
            await fs.unlink(tempPath);
        }
        throw error;
    }
}

async function verifyFiles(files) {
    for (const file of files) {
        if (!existsSync(file.dest)) {
            return false;
        }
    }
    return true;
}

async function copyPdfiumFiles() {
    const baseUrl = `https://github.com/Alexssmusica/pdfium/releases/download/v1.0.0`;
    try {
        const files = [];
        const libPath = path.join(pdfiumDir, 'lib', currentOS, currentArch);

        if (platform === 'win32') {
            files.push({
                url: `${baseUrl}/pdfium-${currentArch}.dll`,
                dest: path.join(libPath, 'bin', 'pdfium.dll')
            });
            files.push({
                url: `${baseUrl}/pdfium-${currentArch}.dll.lib`,
                dest: path.join(libPath, 'lib', 'pdfium.dll.lib')
            });
        } else if (platform === 'linux') {
            files.push({
                url: `${baseUrl}/libpdfium-${currentArch}.so`,
                dest: path.join(libPath, 'lib', 'libpdfium.so')
            });
        }

        // Verifica se todos os arquivos já existem
        const filesExist = await verifyFiles(files);
        if (filesExist) {
            console.log('PDFium files already exist, skipping download');
            return;
        }

        console.log(`Downloading PDFium files for ${currentOS}-${currentArch}...`);
        for (const file of files) {
            try {
                await downloadFile(file.url, file.dest);
                console.log(`Successfully downloaded: ${path.basename(file.dest)}`);
            } catch (error) {
                console.error(`Failed to download ${file.url}: ${error.message}`);
                throw error;
            }
        }

        console.log('PDFium files downloaded successfully');
    } catch (error) {
        console.error('Failed to copy PDFium files:', error);
        process.exit(1);
    }
}

// Executa a instalação
async function install() {
    try {
        await createRequiredDirs();
        await copyPdfiumFiles();
        console.log('Installation completed successfully');
        // Força o encerramento do processo após a conclusão
        process.exit(0);
    } catch (error) {
        console.error('Installation failed:', error);
        process.exit(1);
    }
}

install(); 
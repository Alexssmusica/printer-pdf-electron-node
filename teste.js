const { printPDF } = require('./');
const path = require('path');
// Usando Buffer para garantir a codificação correta do caminho
const filePath = Buffer.from(path.join(__dirname, 'PréVenda.pdf')).toString();

async function imprimirDocumento() {
    try {
        await printPDF({
            filePath: filePath,
            printerName: String.raw`\\desktop-lj9a226\Hewlett-Packard HP LaserJet Pro MFP M125a`
        });
        console.log('Documento enviado para impressão');
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}

imprimirDocumento();
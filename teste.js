const { printPDF } = require('./');
const path = require('path');
// Usando Buffer para garantir a codificação correta do caminho
const filePath = Buffer.from(path.join(__dirname, 'pré-venda.pdf')).toString();

async function imprimirDocumento() {
    try {
        await printPDF({
            filePath: filePath,
            printerName: 'HP508140D7C039(HP Laser MFP 131 133 135-138)'
        });
        console.log('Documento enviado para impressão');
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}

imprimirDocumento();
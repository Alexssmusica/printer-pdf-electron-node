const { printPDF } = require('./');
const path = require('path');
const filePath = path.join(__dirname, 'file.pdf');

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
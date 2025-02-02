const { printPDF } = require('./');
const path = require('path');
const filePath = path.join(__dirname, 'file.pdf');

async function imprimirDocumento() {
    try {
        await printPDF({
            filePath: filePath,
            printerName: 'HP508140D7C039'
        });
        console.log('Documento enviado para impressão');
    } catch (error) {
        console.error('Erro ao imprimir:', error);
    }
}

imprimirDocumento();
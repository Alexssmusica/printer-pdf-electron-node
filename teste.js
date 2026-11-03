const { printPDF } = require('./lib/');
const path = require('path');

const filePath = path.join(__dirname, 'pré-venda.pdf');

async function imprimirDocumento() {
  try {
    await printPDF({
      filePath: filePath,
      printerName: 'PDF',
      orientation: 'landscape'
    });
    console.log('Documento enviado para impressão');
  } catch (error) {
    console.error('Erro ao imprimir:', error);
  }
}

imprimirDocumento();

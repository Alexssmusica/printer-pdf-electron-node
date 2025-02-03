const printer = require('bindings')('printer_pdf_node_electron');


/**
 * Print a PDF file
 * @param {Object} options - Print options
 * @param {Array<Array<number>>} options.pageList - List of page ranges to print, e.g. [[1,3], [5,7]] prints pages 1-3 and 5-7
 * @param {string} options.paperSize - Paper size ('A4', 'LETTER', 'LEGAL', 'A3', 'CUSTOM')
 * @param {Object} options.margins - Page margins in inches
 * @param {number} options.margins.top - Top margin in inches (default: 0)
 * @param {number} options.margins.right - Right margin in inches (default: 0)
 * @param {number} options.margins.bottom - Bottom margin in inches (default: 0)
 * @param {number} options.margins.left - Left margin in inches (default: 0)
 * @param {boolean} options.fitToPage - Whether to scale content to fit the page (default: true)
 * @param {number} options.width - Custom width in points (only used when paperSize is 'CUSTOM')
 * @param {number} options.height - Custom height in points (only used when paperSize is 'CUSTOM')
 * @param {number} options.dpi - Print resolution in DPI (default: 300)
 * @param {number} options.copies - Number of copies to print (default: 1)
 * @param {string} options.filePath - Path to the PDF file
 * @param {string} options.printerName - Name of the printer to use
 * @returns {Promise<string>} - Resolves when print job is created
 */
function printPDF({
    pageList = [],
    paperSize = 'A4',  
    margins = {
        top: 0,
        right: 0,
        bottom: 0,
        left: 0
    },
    fitToPage = true, 
    width = 0,
    height = 0,
    dpi = 300,
    copies = 1,
    filePath,
    printerName,
}) {
    // Validate required parameters
    if (!filePath) {
        throw new Error('filePath is required');
    }
    if (!printerName) {
        throw new Error('printerName is required');
    }

    const pointMargins = {
        top: margins.top * 72,
        right: margins.right * 72,
        bottom: margins.bottom * 72,
        left: margins.left * 72
    };

    return new Promise((resolve, reject) => {
        try {
            printer.printPDF(printerName, filePath, {
                pageList,
                paperSize,
                fitToPage,
                margins: pointMargins,
                width,
                height,
                dpi,
                copies,
            });
            resolve('Print job created successfully');
        } catch (e) {
            console.error(e);
            reject(e);
        }
    });
}

module.exports = {
    printPDF
};

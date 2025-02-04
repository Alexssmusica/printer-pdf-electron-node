/// <reference types="node" />

/**
 * Paper size options for printing
 */
export type PaperSize = 'A4' | 'LETTER' | 'LEGAL' | 'A3' | 'CUSTOM';

/**
* Margins configuration in inches
*/
export interface Margins {
    /** Top margin in inches */
    top?: number;
    /** Right margin in inches */
    right?: number;
    /** Bottom margin in inches */
    bottom?: number;
    /** Left margin in inches */
    left?: number;
}

/**
  * Options for printing a PDF file
  */
export interface PrintPDFOptions {
    /** List of page ranges to print, e.g. [[1,3], [5,7]] prints pages 1-3 and 5-7 */
    pageList?: Array<[number, number]>;
    /** Paper size for printing */
    paperSize?: PaperSize;
    /** Page margins in inches */
    margins?: Margins;
    /** Whether to scale content to fit the page */
    fitToPage?: boolean;
    /** Custom width in points (only used when paperSize is 'CUSTOM') */
    width?: number;
    /** Custom height in points (only used when paperSize is 'CUSTOM') */
    height?: number;
    /** Print resolution in DPI */
    dpi?: number;
    /** Number of copies to print */
    copies?: number;
    /** Path to the PDF file */
    filePath: string;
    /** Name of the printer to use */
    printerName: string;
}
declare module 'printer-pdf-electron-node' {

    /**
     * Prints a PDF file with the specified options
     * @param options - Print configuration options
     * @returns A promise that resolves when the print job is created
     */
    function printPDF(options: PrintPDFOptions): Promise<void>;
}

export default 'printer-pdf-electron-node';
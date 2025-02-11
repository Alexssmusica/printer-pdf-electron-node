import bindings from 'bindings';
import fs from 'fs';
const printer = bindings('printer_pdf_electron_node');

export type PaperSize = 'A4' | 'LETTER' | 'LEGAL' | 'A3' | 'CUSTOM';

export interface Margins {
    top?: number;
    right?: number;
    bottom?: number;
    left?: number;
}

export interface PrintPDFOptions {
    pageList?: Array<[number, number]>;
    paperSize?: PaperSize;
    margins?: Margins;
    fitToPage?: boolean;
    width?: number;
    height?: number;
    dpi?: number;
    copies?: number;
    filePath: string;
    printerName: string;
}

export async function printPDF({
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
}: PrintPDFOptions): Promise<void> {
    if (!filePath) {
        throw new Error('filePath is required');
    }
    if (!printerName) {
        throw new Error('printerName is required');
    }

    if (!fs.existsSync(filePath)) {
        throw new Error(`PDF file not found: ${filePath}`);
    }

    const pointMargins = {
        top: margins.top ?? 0 * 72,
        right: margins.right ?? 0 * 72,
        bottom: margins.bottom ?? 0 * 72,
        left: margins.left ?? 0 * 72
    };

    try {
        await printer.printPDF(normalizeString(printerName), normalizeString(filePath), {
            pageList,
            paperSize,
            fitToPage,
            margins: pointMargins,
            width,
            height,
            dpi,
            copies,
        });
    } catch (e) {
        throw e;
    }
}

function normalizeString(str: string): string {
    return String.raw`${str}`
}

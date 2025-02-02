#include "printer_interface.h"
#include "pdfium_imp.h"

namespace printer_pdf_node_electron
{

bool WindowsPrinter::Initialize(const Napi::Value& printerName) {
    const Napi::String printerNameV8Str = printerName.ToString();
    printer_dc = ::CreateDCW(L"WINSPOOL", 
                           reinterpret_cast<LPCWSTR>(printerNameV8Str.Utf16Value().c_str()), 
                           NULL, 
                           NULL);
    return printer_dc != nullptr;
}

bool WindowsPrinter::Print(const std::string& filePath, const PdfiumOption& options) {
    if (!printer_dc) {
        return false;
    }

    try {
        auto filePathW = std::wstring(filePath.begin(), filePath.end());
        auto doc = std::make_unique<PDFDocument>(std::move(filePathW));

        if (!doc->LoadDocument()) {
            return false;
        }

        doc->PrintDocument(printer_dc, options);
        return true;
    }
    catch (...) {
        return false;
    }
}

// Factory function implementation for Windows
std::unique_ptr<PrinterInterface> CreatePrinter() {
    return std::unique_ptr<PrinterInterface>(new WindowsPrinter());
}

} 
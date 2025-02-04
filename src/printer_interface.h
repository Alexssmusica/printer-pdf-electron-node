#ifndef PRINTER_INTERFACE_H
#define PRINTER_INTERFACE_H

#include "inc.h"
#include "pdfium_option.h"

namespace printer_pdf_electron_node {

class PrinterInterface {
public:
    virtual ~PrinterInterface() = default;
    virtual std::string Initialize(const Napi::Value& printerName) = 0;
    virtual bool Print(const std::string& filePath, const PdfiumOption& options) = 0;
};

#ifdef _WIN32
class WindowsPrinter : public PrinterInterface {
public:
    WindowsPrinter() = default;
    ~WindowsPrinter() {
        if (printer_dc) {
            DeleteDC(printer_dc);
            printer_dc = nullptr;
        }
    }
    std::string Initialize(const Napi::Value& printerName) override;
    bool Print(const std::string& filePath, const PdfiumOption& options) override;
private:
    HDC printer_dc = nullptr;
};
#else
// Forward declaration for Linux
class LinuxPrinter;
#endif

// Factory function declaration
std::unique_ptr<PrinterInterface> CreatePrinter();

}  // namespace printer_pdf_electron_node

#endif 
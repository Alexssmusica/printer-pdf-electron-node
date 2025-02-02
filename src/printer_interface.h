#ifndef PRINTER_INTERFACE_H
#define PRINTER_INTERFACE_H

#include "inc.h"
#include "pdfium_option.h"

namespace printer_pdf_node_electron {

class PrinterInterface {
public:
    virtual ~PrinterInterface() = default;
    virtual bool Initialize(const Napi::Value& printerName) = 0;
    virtual bool Print(const std::string& filePath, const PdfiumOption& options) = 0;
};

#ifdef _WIN32
class WindowsPrinter : public PrinterInterface {
public:
    WindowsPrinter() = default;
    ~WindowsPrinter() = default;
    bool Initialize(const Napi::Value& printerName) override;
    bool Print(const std::string& filePath, const PdfiumOption& options) override;
private:
    HDC printer_dc = nullptr;
};
#else
// Forward declaration for Linux
class LinuxPrinter;
#endif

#ifdef _WIN32
inline std::unique_ptr<PrinterInterface> CreatePrinter() {
    return std::make_unique<WindowsPrinter>();
}
#else
inline std::unique_ptr<PrinterInterface> CreatePrinter() {
    return std::make_unique<LinuxPrinter>();
}
#endif

}  // namespace printer_pdf_node_electron

#endif 
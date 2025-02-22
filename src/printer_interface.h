#ifndef PRINTER_INTERFACE_H
#define PRINTER_INTERFACE_H
#include "inc.h"
#include "pdfium_option.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace printer_pdf_electron_node
{

    class PrinterInterface
    {
    public:
        virtual ~PrinterInterface() = default;
        virtual std::string Initialize(const Napi::Value &printerName) = 0;
        virtual bool Print(const std::string &filePath, const PdfiumOption &options) = 0;
    };

#ifdef _WIN32
    class WindowsPrinter : public PrinterInterface
    {
    public:
        WindowsPrinter() : printer_dc(NULL) {}
        virtual ~WindowsPrinter();
        std::string Initialize(const Napi::Value &printerName) override;
        bool Print(const std::string &filePath, const PdfiumOption &options) override;

    private:
        HDC printer_dc;
    };
#else
    class LinuxPrinter;
#endif

    std::unique_ptr<PrinterInterface> CreatePrinter();

}

#endif
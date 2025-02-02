#ifndef PRINTER_LINUX
#define PRINTER_LINUX

#include "inc.h"
#include <cups/cups.h>

namespace printer_pdf_node_electron {
    struct CupsDeleter {
        inline void operator()(cups_dest_t* dest) { cupsFreeDests(1, dest); }
    };

    struct PrinterContextDeleter {
        inline void operator()(void* ctx) { cupsFreeOptions((int)reinterpret_cast<intptr_t>(ctx), nullptr); }
    };

    using Unique_CupsDest = std::unique_ptr<cups_dest_t, CupsDeleter>;
    using Unique_PrinterContext = std::unique_ptr<void, PrinterContextDeleter>;

    Unique_PrinterContext GetPrinterContext(const Napi::Value& printerName);
    bool PrintPDFToCups(const std::string& printerName, const std::string& filePath, void* ctx);
};

#endif 
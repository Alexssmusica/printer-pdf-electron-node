#include "printer_linux.h"

namespace printer_pdf_node_electron {

Unique_PrinterContext GetPrinterContext(const Napi::Value& printerName) {
    const Napi::String printerNameV8Str = printerName.ToString();
    std::string printerNameStr = printerNameV8Str.Utf8Value();

    cups_dest_t* dest = nullptr;
    int num_dests = cupsGetDest(printerNameStr.c_str(), nullptr, 1, &dest);
    
    if (num_dests == 0 || !dest) {
        return nullptr;
    }

    // Create a context with printer options
    int num_options = 0;
    cups_option_t* options = nullptr;

    // Add default options
    num_options = cupsAddOption("media", "A4", num_options, &options);
    num_options = cupsAddOption("fit-to-page", "true", num_options, &options);

    // Free the destination since we don't need it anymore
    cupsFreeDests(1, dest);

    // Return the options context
    return Unique_PrinterContext(reinterpret_cast<void*>(static_cast<intptr_t>(num_options)));
}

bool PrintPDFToCups(const std::string& printerName, const std::string& filePath, void* ctx) {
    int num_options = static_cast<int>(reinterpret_cast<intptr_t>(ctx));
    cups_option_t* options = nullptr;

    // Create the print job
    int job_id = cupsPrintFile(printerName.c_str(), 
                             filePath.c_str(),
                             "PDF Document",
                             num_options, 
                             options);

    return job_id != 0;
}

} 
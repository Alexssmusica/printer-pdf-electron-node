#include "printer_linux.h"

namespace printer_pdf_node_electron {

LinuxPrinter::~LinuxPrinter() {
    if (printer_dest) {
        cupsFreeDests(1, printer_dest);
    }
    CleanupOptions();
}

void LinuxPrinter::CleanupOptions() {
    if (options) {
        cupsFreeOptions(num_options, options);
        options = nullptr;
        num_options = 0;
    }
}

bool LinuxPrinter::Initialize(const Napi::Value& printerName) {
    const Napi::String printerNameV8Str = printerName.ToString();
    printer_name = printerNameV8Str.Utf8Value();

    // Get printer destination
    int num_dests = cupsGetDest(printer_name.c_str(), nullptr, 1, &printer_dest);
    if (num_dests == 0 || !printer_dest) {
        return false;
    }

    return true;
}

bool LinuxPrinter::Print(const std::string& filePath, const PdfiumOption& options) {
    // Cleanup any existing options
    CleanupOptions();

    // Set up print options
    num_options = 0;
    
    // Add media size based on paperSize
    switch (options.paperSize) {
        case PaperSize::A4:
            num_options = cupsAddOption("media", "A4", num_options, &this->options);
            break;
        case PaperSize::LETTER:
            num_options = cupsAddOption("media", "Letter", num_options, &this->options);
            break;
        case PaperSize::LEGAL:
            num_options = cupsAddOption("media", "Legal", num_options, &this->options);
            break;
        case PaperSize::A3:
            num_options = cupsAddOption("media", "A3", num_options, &this->options);
            break;
        default:
            // For custom size, we'll use A4 as default
            num_options = cupsAddOption("media", "A4", num_options, &this->options);
            break;
    }

    // Set fit-to-page option
    if (options.fitToPage) {
        num_options = cupsAddOption("fit-to-page", "true", num_options, &this->options);
    }

    // Set number of copies
    if (options.copies > 1) {
        num_options = cupsAddOption("copies", std::to_string(options.copies).c_str(), 
                                  num_options, &this->options);
    }

    // Create the print job
    int job_id = cupsPrintFile(printer_name.c_str(), 
                             filePath.c_str(),
                             "PDF Document",
                             num_options, 
                             this->options);

    return job_id != 0;
}

} 
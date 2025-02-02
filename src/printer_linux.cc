#include "printer_linux.h"

namespace printer_pdf_node_electron {

LinuxPrinter::~LinuxPrinter() {
    if (printer_dest) {
        cupsFreeDests(1, printer_dest);
        printer_dest = nullptr;
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
    cups_dest_t* dests = nullptr;
    int num_dests = cupsGetDests(&dests);
    
    // Find the specific printer
    printer_dest = cupsGetDest(printer_name.c_str(), nullptr, num_dests, dests);
    
    if (!printer_dest) {
        cupsFreeDests(num_dests, dests);
        return false;
    }

    // Create a copy of the destination
    cups_dest_t* dest_copy = new cups_dest_t;
    memcpy(dest_copy, printer_dest, sizeof(cups_dest_t));
    printer_dest = dest_copy;

    // Free the original destinations list
    cupsFreeDests(num_dests, dests);

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
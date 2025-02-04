#include "printer_linux.h"
#include <napi.h>
#include <cups/cups.h>
#include <cstring>
namespace printer_pdf_electron_node {

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

std::string LinuxPrinter::Initialize(const Napi::Value& printerName) {
    // Converte o valor recebido para string do Napi::Value
    Napi::String printerNameV8Str = printerName.ToString();
    printer_name = printerNameV8Str.Utf8Value();

    // Obter destinos de impressoras
    cups_dest_t* dests = nullptr;
    int num_dests = cupsGetDests(&dests);

    if (num_dests == 0) {
        return "Nenhuma impressora encontrada.";
    }

    // Encontrar a impressora específica
    printer_dest = cupsGetDest(printer_name.c_str(), nullptr, num_dests, dests);
    if (!printer_dest) {
        cupsFreeDests(num_dests, dests);
        return "Impressora não encontrada: " + printer_name;
    }

    // Criar uma cópia do destino
    cups_dest_t* dest_copy = new cups_dest_t;
    memcpy(dest_copy, printer_dest, sizeof(cups_dest_t));
    printer_dest = dest_copy;

    // Liberar a lista original de destinos
    cupsFreeDests(num_dests, dests);

    // Verificação adicional: tentar obter informações sobre a impressora
    http_t* http = httpConnect2(cupsServer(), ippPort(), 
                              nullptr,      // addrlist
                              AF_UNSPEC,    // family
                              HTTP_ENCRYPTION_IF_REQUESTED,
                              1,            // blocking
                              30000,        // timeout (ms)
                              nullptr);     // cancel
    if (!http) {
        return "Falha ao conectar ao servidor CUPS.";
    }

    ipp_t* request = ippNewRequest(CUPS_GET_PRINTERS);
    ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", nullptr, printer_name.c_str());

    ipp_t* response = cupsDoRequest(http, request, "/");
    if (!response) {
        httpClose(http);
        return "Falha ao obter informações da impressora: " + printer_name;
    }

    ippDelete(response);
    httpClose(http);

    return ""; // Retorna string vazia em caso de sucesso
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

// Factory function implementation for Linux
std::unique_ptr<PrinterInterface> CreatePrinter() {
    return std::unique_ptr<PrinterInterface>(new LinuxPrinter());
}

} 
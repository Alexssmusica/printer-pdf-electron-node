#ifndef PDFIUM_OPTION
#define PDFIUM_OPTION

#include "inc.h"

namespace printer_pdf_electron_node {
    enum class PaperSize {
        A4,      // 210 x 297 mm
        LETTER,  // 216 x 279 mm
        LEGAL,   // 216 x 356 mm
        A3,      // 297 x 420 mm
        CUSTOM
    };

    struct Margins {
        float top = 0;
        float right = 0;
        float bottom = 0;
        float left = 0;
    };

    struct PdfiumOption {
        PdfiumOption() = default;

        float dpi = 324;
        int32_t copies = 1;
        int32_t width = 0;
        int32_t height = 0;
        PaperSize paperSize = PaperSize::A4;
        Margins margins;
        bool fitToPage = true;
        std::vector<std::pair<int32_t, int32_t>> page_list;
    };

    std::unique_ptr<PdfiumOption> V8OptionToStruct(const Napi::Value& options);
};
#endif
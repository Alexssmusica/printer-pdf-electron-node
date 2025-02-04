#include "pdfium_imp.h"
#include <clocale>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <winspool.h>
using namespace std;
namespace printer_pdf_electron_node
{

#ifdef _WIN32

    PrinterDocumentJob::PrinterDocumentJob(DeviceContext dc, const std::wstring &filename)
        : dc_(dc), filename_(filename), jobId_(0), cancelled_(false)
    {
        // Inicializar DOCINFOW corretamente
        DOCINFOW di = {0};
        di.cbSize = sizeof(DOCINFOW);
        di.lpszDocName = filename_.c_str();
        di.lpszOutput = nullptr;
        di.lpszDatatype = nullptr; // Alterado de L"RAW" para nullptr
        di.fwType = 0;
        // Iniciar o documento
        jobId_ = ::StartDocW(dc_, &di);
        if (jobId_ <= 0)
        {
            cancelled_ = true;
            DWORD error = GetLastError();
            // Log do erro se necessário
        }
    }

    PrinterDocumentJob::~PrinterDocumentJob()
    {
        if (jobId_ > 0)
        {
            ::EndDoc(dc_);
        }
    }

    bool PrinterDocumentJob::Start()
    {
        return jobId_ > 0;
    }

    bool PrinterDocumentJob::IsCancelled() const
    {
        return cancelled_;
    }

    PDFDocument::PDFDocument(std::wstring &&f) : filename(f) {}

    bool PDFDocument::LoadDocument()
    {
        std::ifstream pdfStream(std::string(filename.begin(), filename.end()),
                                std::ifstream::binary | std::ifstream::in);
        file_content.insert(file_content.end(),
                            std::istreambuf_iterator<char>(pdfStream),
                            std::istreambuf_iterator<char>());
        auto pdf_pointer = FPDF_LoadMemDocument(file_content.data(), (int)file_content.size(), nullptr);
        if (!pdf_pointer)
            return false;
        doc.reset(pdf_pointer);
        return true;
    }

    void PDFDocument::PrintDocument(DeviceContext dc, const PdfiumOption &options)
    {
        PrinterDocumentJob djob(dc, filename);
        if (!djob.Start())
        {
            std::cerr << "Falha ao iniciar o trabalho de impressão. Erro: " << GetLastError() << std::endl;
            return;
        }

        auto pageCount = FPDF_GetPageCount(doc.get());
        auto width = options.width;
        auto height = options.height;

        for (int16_t i = 0; i < options.copies; ++i)
        {
            if (std::size(options.page_list) > 0)
            {
                for (auto pair : options.page_list)
                {
                    for (auto j = pair.first; j < pair.second + 1; ++j)
                    {
                        if (djob.IsCancelled())
                        {
                            return;
                        }
                        printPage(dc, j, width, height, options.dpi, options);
                    }
                }
            }
            else
            {
                for (auto j = 0; j < pageCount; j++)
                {
                    if (djob.IsCancelled())
                    {
                        return;
                    }
                    printPage(dc, j, width, height, options.dpi, options);
                }
            }
        }
    }

    void PDFDocument::printPage(DeviceContext dc,
                                int32_t index, int32_t width, int32_t height, float dpiRatio,
                                const PdfiumOption &options)
    {
        PrinterPageJob pJob(dc);
        auto page = getPage(doc.get(), index);
        if (!page)
        {
            return;
        }

        // Garantir que estamos no modo correto
        SetGraphicsMode(dc, GM_ADVANCED);
        SetMapMode(dc, MM_TEXT);
        SetBkMode(dc, TRANSPARENT);

        // Get physical page dimensions
        int physicalWidth = GetDeviceCaps(dc, PHYSICALWIDTH);
        int physicalHeight = GetDeviceCaps(dc, PHYSICALHEIGHT);
        int physicalOffsetX = GetDeviceCaps(dc, PHYSICALOFFSETX);
        int physicalOffsetY = GetDeviceCaps(dc, PHYSICALOFFSETY);

        // Calculate printable area
        int printableWidth = GetDeviceCaps(dc, HORZRES);
        int printableHeight = GetDeviceCaps(dc, VERTRES);

        // Apply margins to printable area (converting from points to device units)
        int marginLeft = static_cast<int>((options.margins.left / 72.0f) * dpiRatio);
        int marginTop = static_cast<int>((options.margins.top / 72.0f) * dpiRatio);
        int marginRight = static_cast<int>((options.margins.right / 72.0f) * dpiRatio);
        int marginBottom = static_cast<int>((options.margins.bottom / 72.0f) * dpiRatio);

        // Adjust printable area considering margins
        int effectivePrintableWidth = printableWidth - (marginLeft + marginRight);
        int effectivePrintableHeight = printableHeight - (marginTop + marginBottom);

        // If no width/height specified, get from PDF page
        if (!width)
        {
            auto pageWidth = FPDF_GetPageWidth(page);
            width = static_cast<int32_t>(pageWidth * dpiRatio);
        }
        if (!height)
        {
            auto pageHeight = FPDF_GetPageHeight(page);
            height = static_cast<int32_t>(pageHeight * dpiRatio);
        }

        // Create clipping region for the printable area (including margins)
        HRGN rgn = CreateRectRgn(0, 0, printableWidth, printableHeight);
        SelectClipRgn(dc, rgn);
        DeleteObject(rgn);

        // Clear the background
        Rectangle(dc, -physicalOffsetX, -physicalOffsetY, physicalWidth, physicalHeight);

        float scale;
        int32_t x, y;
        if (options.fitToPage)
        {
            // Calculate scaling to fit page while maintaining aspect ratio
            float scaleX = static_cast<float>(effectivePrintableWidth) / width;
            float scaleY = static_cast<float>(effectivePrintableHeight) / height;
            scale = scaleX < scaleY ? scaleX : scaleY;

            // Calculate centered position within the effective printable area
            x = marginLeft + (effectivePrintableWidth - static_cast<int32_t>(width * scale)) / 2;
            y = marginTop + (effectivePrintableHeight - static_cast<int32_t>(height * scale)) / 2;
        }
        else
        {
            // Use actual size (1:1 scale)
            scale = 1.0f;

            // Center the content in the available space
            x = marginLeft + (effectivePrintableWidth - width) / 2;
            y = marginTop + (effectivePrintableHeight - height) / 2;

            // If content is larger than printable area, align to top-left corner
            if (width > effectivePrintableWidth || height > effectivePrintableHeight)
            {
                x = marginLeft;
                y = marginTop;
            }
        }

        // Render the PDF page
        ::FPDF_RenderPage(dc,                                   // DC handle
                          page,                                 // page handle
                          x,                                    // start x
                          y,                                    // start y
                          static_cast<int32_t>(width * scale),  // size x
                          static_cast<int32_t>(height * scale), // size y
                          0,                                    // rotate
                          FPDF_ANNOT | FPDF_PRINTING);          // flags

        // Após renderizar a página
        GdiFlush();
    }

    FPDF_PAGE PDFDocument::getPage(const FPDF_DOCUMENT &doc, int32_t index)
    {
        auto iter = loaded_pages.find(index);
        if (iter != loaded_pages.end())
            return iter->second.get();

        ScopedFPDFPage page(FPDF_LoadPage(doc, index));
        if (!page)
            return nullptr;

        // Mark the page as loaded first to prevent infinite recursion.
        FPDF_PAGE page_ptr = page.get();
        loaded_pages[index] = std::move(page);
        return page_ptr;
    }

#endif

}

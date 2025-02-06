#ifndef PDFIUM_IMP
#define PDFIUM_IMP
#include <iostream>
#include "inc.h"
#include "pdfium_option.h"
#include "platform_defs.h"

namespace printer_pdf_electron_node
{

#ifdef _WIN32
  class PDFDocument
  {
  public:
    PDFDocument(std::wstring &&filename);
    bool LoadDocument();
    void PrintDocument(DeviceContext dc, const PdfiumOption &options);

  private:
    void printPage(DeviceContext dc, int32_t index, int32_t width, int32_t height, float dpiRatio,
                   const PdfiumOption &options);
    std::wstring filename;
    ScopedFPDFDocument doc;
    std::map<int, ScopedFPDFPage> loaded_pages;
    std::vector<char> file_content;
    FPDF_PAGE getPage(const FPDF_DOCUMENT &doc, int32_t index);
  };

  class PrinterDocumentJob
  {
  public:
    PrinterDocumentJob(DeviceContext dc, const std::wstring &filename);
    ~PrinterDocumentJob();
    bool Start();
    bool IsCancelled() const;

  private:
    DeviceContext dc_;
    std::wstring filename_;
    DWORD jobId_;
    bool cancelled_;
  };

  class PrinterPageJob
  {
  public:
    PrinterPageJob(DeviceContext d) : dc(d) {
        if (::StartPage(dc) <= 0) {
            DWORD error = GetLastError();
            throw std::runtime_error("StartPage failed with error: " + std::to_string(error));
        }
    }
    
    ~PrinterPageJob() {
        try {
            if (::EndPage(dc) <= 0) {
                // Log do erro apenas, não lançar exceção no destrutor
                DWORD error = GetLastError();
                std::cerr << "EndPage failed with error: " << error << std::endl;
            }
        } catch (...) {
            // Nunca deixar exceções escaparem do destrutor
            std::cerr << "Unexpected error in PrinterPageJob destructor" << std::endl;
        }
    }

  private:
    DeviceContext dc;
  };
#endif

};

#endif

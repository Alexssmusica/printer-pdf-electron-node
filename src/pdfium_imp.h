#ifndef PDFIUM_IMP
#define PDFIUM_IMP

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
  PrinterDocumentJob(DeviceContext dc, const std::wstring& filename);
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
  PrinterPageJob(DeviceContext d) : dc(d)
  {
    auto res = ::StartPage(dc);
    if (!res)
      throw;
  };
  ~PrinterPageJob()
  {
    ::EndPage(dc);
  };

private:
  DeviceContext dc;
};
#endif

};

#endif

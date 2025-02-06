#ifndef PRINTER_WIN
#define PRINTER_WIN
#ifndef PDFIUM_IMP_H
#define PDFIUM_IMP_H
#define UNICODE
#define _UNICODE
#include "inc.h"
#include "printer_interface.h"

namespace printer_pdf_electron_node
{
    struct DCDeleter
    {
        inline void operator()(HDC dc) { ::DeleteDC(dc); }
    };
    struct HPDeleter
    {
        inline void operator()(HANDLE h) { ::ClosePrinter(h); }
    };
    using Unique_HDC =
        std::unique_ptr<std::remove_pointer<HDC>::type, DCDeleter>;
    using Unique_HPrinter =
        std::unique_ptr<std::remove_pointer<HANDLE>::type, HPDeleter>;


    class PrinterDocumentJob
    {
    public:
        PrinterDocumentJob(DeviceContext dc, const wchar_t *filename)
            : dc(dc), filename(filename), jobId(0)
        {
            // Inicializa o trabalho de impressão
            DOC_INFO_1 docInfo;
            docInfo.pDocName = filename;
            docInfo.pOutputFile = NULL;
            docInfo.pDatatype = L"RAW";

            jobId = StartDocPrinter(dc, 1, (LPBYTE)&docInfo);
        }

        bool Start()
        {
            if (jobId <= 0)
            {
                return false;
            }
            return true;
        }

        bool IsCancelled() const
        {
            HANDLE hPrinter;
            PRINTER_DEFAULTS pd = {NULL, NULL, PRINTER_ACCESS_USE};
            if (!OpenPrinter(const_cast<LPWSTR>(filename.c_str()), &hPrinter, &pd))
            {
                return false;
            }

            JOB_INFO_1 *pJobInfo = NULL;
            DWORD dwNeeded = 0, dwReturned = 0;

            // Primeira chamada para GetJob para obter o tamanho do buffer
            if (!GetJob(hPrinter, jobId, 1, NULL, 0, &dwNeeded))
            {
                if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                {
                    ClosePrinter(hPrinter);
                    return false;
                }

                pJobInfo = (JOB_INFO_1 *)malloc(dwNeeded);
                if (!pJobInfo)
                {
                    ClosePrinter(hPrinter);
                    return false;
                }
            }

            // Segunda chamada para GetJob para obter as informações do trabalho
            if (!GetJob(hPrinter, jobId, 1, (LPBYTE)pJobInfo, dwNeeded, &dwReturned))
            {
                free(pJobInfo);
                ClosePrinter(hPrinter);
                return false;
            }

            bool cancelled = (pJobInfo->Status & JOB_STATUS_DELETING) || (pJobInfo->Status & JOB_STATUS_DELETED);

            free(pJobInfo);
            ClosePrinter(hPrinter);

            return cancelled;
        }

    private:
        DeviceContext dc;
        std::wstring filename;
        DWORD jobId;
    };

    class PDFDocument
    {
    public:
        PDFDocument(std::wstring &&f);
        bool LoadDocument();
        void PrintDocument(DeviceContext dc, const PdfiumOption &options);
        void printPage(DeviceContext dc, int32_t index, int32_t width, int32_t height, float dpiRatio, const PdfiumOption &options);
        FPDF_PAGE getPage(const FPDF_DOCUMENT &doc, int32_t index);

    private:
        std::wstring filename;
        std::vector<char> file_content;
        std::unique_ptr<FPDF_DOCUMENT, void (*)(FPDF_DOCUMENT)> doc;
        std::map<int32_t, ScopedFPDFPage> loaded_pages;
    };
};

#endif
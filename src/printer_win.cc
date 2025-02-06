#include "printer_interface.h"
#include "pdfium_imp.h"
#include "utils.h"
#include <iostream>
#include <napi.h> // Supondo que você esteja usando o N-API no C++
#include <string>
#include <windows.h>
namespace printer_pdf_electron_node
{

    WindowsPrinter::~WindowsPrinter()
    {
        if (printer_dc)
        {
            DeleteDC(printer_dc);
            printer_dc = NULL;
        }
    }

    std::string WindowsPrinter::Initialize(const Napi::Value &printerName)
    {
        try
        {
            LogError("Initializing printer...");

            Napi::String printerNameV8Str = printerName.ToString();
            std::u16string u16Str = printerNameV8Str.Utf16Value();

            std::wstring wPrinterName;
            wPrinterName.resize(u16Str.length());
            for (size_t i = 0; i < u16Str.length(); ++i)
            {
                wPrinterName[i] = static_cast<wchar_t>(u16Str[i]);
            }

            LogError("Opening printer: " + std::string(wPrinterName.begin(), wPrinterName.end()));

            HANDLE hPrinter = nullptr;
            if (!OpenPrinterW(const_cast<LPWSTR>(wPrinterName.c_str()), &hPrinter, NULL))
            {
                DWORD errorCode = GetLastError();
                std::string error = "Failed to open printer. Error code: " + std::to_string(errorCode);
                LogError(error);
                return error;
            }

            // Usar RAII com escopo local
            struct PrinterHandleCloser
            {
                void operator()(HANDLE h)
                {
                    if (h)
                        ClosePrinter(h);
                }
            };
            std::unique_ptr<void, PrinterHandleCloser> printerHandle(hPrinter);

            DWORD needed = 0;
            GetPrinter(hPrinter, 2, NULL, 0, &needed);
            if (needed == 0)
            {
                std::string error = "Failed to get printer info size";
                LogError(error);
                return error;
            }

            std::vector<BYTE> buffer(needed);
            PRINTER_INFO_2 *pi2 = reinterpret_cast<PRINTER_INFO_2 *>(buffer.data());

            if (!GetPrinter(hPrinter, 2, buffer.data(), needed, &needed))
            {
                DWORD errorCode = GetLastError();
                std::string error = "Failed to get printer info. Error code: " + std::to_string(errorCode);
                LogError(error);
                return error;
            }

            // Verificar status da impressora
            if (pi2->Status != 0)
            {
                std::string status = "Printer status issues:";
                if (pi2->Status & PRINTER_STATUS_ERROR)
                    status += " ERROR";
                if (pi2->Status & PRINTER_STATUS_OFFLINE)
                    status += " OFFLINE";
                if (pi2->Status & PRINTER_STATUS_PAPER_JAM)
                    status += " PAPER_JAM";
                if (pi2->Status & PRINTER_STATUS_PAPER_OUT)
                    status += " PAPER_OUT";
                if (pi2->Status & PRINTER_STATUS_OUTPUT_BIN_FULL)
                    status += " BIN_FULL";

                LogError(status);
                return status;
            }

            // Criar DC com tratamento de erro melhorado
            printer_dc = CreateDCW(L"WINSPOOL", wPrinterName.c_str(), NULL, NULL);
            if (!printer_dc)
            {
                DWORD errorCode = GetLastError();
                LPWSTR errorMsg = nullptr;
                FormatMessageW(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPWSTR)&errorMsg, 0, NULL);

                std::string error = "Failed to create DC. Error code: " + std::to_string(errorCode);
                if (errorMsg)
                {
                    error += " - " + std::string(errorMsg, errorMsg + wcslen(errorMsg));
                    LocalFree(errorMsg);
                }

                LogError(error);
                return error;
            }

            // Configurar o DC com verificação de erros
            if (!SetMapMode(printer_dc, MM_TEXT))
            {
                std::string error = "Failed to set map mode. Error: " + std::to_string(GetLastError());
                LogError(error);
                DeleteDC(printer_dc);
                printer_dc = NULL;
                return error;
            }

            // Verificar capacidades do dispositivo
            int caps = GetDeviceCaps(printer_dc, TECHNOLOGY);
            if (caps == 0)
            {
                std::string error = "Failed to get device capabilities. Error: " + std::to_string(GetLastError());
                LogError(error);
                DeleteDC(printer_dc);
                printer_dc = NULL;
                return error;
            }

            LogError("Printer initialized successfully");
            return "";
        }
        catch (const std::exception &e)
        {
            std::string error = "Exception in Initialize: " + std::string(e.what());
            LogError(error);
            if (printer_dc)
            {
                DeleteDC(printer_dc);
                printer_dc = NULL;
            }
            return error;
        }
        catch (...)
        {
            std::string error = "Unknown exception in Initialize";
            LogError(error);
            if (printer_dc)
            {
                DeleteDC(printer_dc);
                printer_dc = NULL;
            }
            return error;
        }
    }

    bool WindowsPrinter::Print(const std::string &filePath, const PdfiumOption &options)
    {
        if (!printer_dc)
        {
            LogError("No valid printer DC");
            return false;
        }

        try
        {
            LogError("Starting print job for file: " + filePath);

            auto filePathW = std::wstring(filePath.begin(), filePath.end());
            auto doc = std::make_unique<PDFDocument>(std::move(filePathW));

            if (!doc->LoadDocument())
            {
                LogError("Failed to load document");
                return false;
            }

            if (!SetGraphicsMode(printer_dc, GM_ADVANCED))
            {
                LogError("Failed to set graphics mode");
                return false;
            }

            if (!SetMapMode(printer_dc, MM_TEXT))
            {
                LogError("Failed to set map mode");
                return false;
            }

            doc->PrintDocument(printer_dc, options);

            if (!GdiFlush())
            {
                LogError("Failed to flush GDI operations");
                return false;
            }

            LogError("Print job completed successfully");
            return true;
        }
        catch (const std::exception &e)
        {
            LogError("Exception in Print: " + std::string(e.what()));
            return false;
        }
        catch (...)
        {
            LogError("Unknown exception in Print");
            return false;
        }
    }

    // Factory function implementation for Windows
    std::unique_ptr<PrinterInterface> CreatePrinter()
    {
        return std::make_unique<WindowsPrinter>();
    }

}
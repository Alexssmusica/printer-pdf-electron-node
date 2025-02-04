#include "printer_interface.h"
#include "pdfium_imp.h"
#include <iostream>
#include <napi.h> // Supondo que você esteja usando o N-API no C++
#include <string>
#include <windows.h>
namespace printer_pdf_electron_node
{

    std::string WindowsPrinter::Initialize(const Napi::Value &printerName)
    {
        Napi::String printerNameV8Str = printerName.ToString();
        std::u16string u16Str = printerNameV8Str.Utf16Value();

        // Converter corretamente para WCHAR*
        std::wstring wPrinterName;
        wPrinterName.resize(u16Str.length());
        for (size_t i = 0; i < u16Str.length(); ++i)
        {
            wPrinterName[i] = static_cast<wchar_t>(u16Str[i]);
        }

        // Verificar o status da impressora
        HANDLE hPrinter;
        if (!OpenPrinterW(const_cast<LPWSTR>(wPrinterName.c_str()), &hPrinter, NULL))
        {
            DWORD errorCode = GetLastError();
            return "Erro ao abrir a impressora.";
        }

        // Obter o tamanho necessário para o buffer de PRINTER_INFO_2
        DWORD needed = 0;
        GetPrinter(hPrinter, 2, NULL, 0, &needed);
        if (needed == 0)
        {
            ClosePrinter(hPrinter);
            return "Erro ao obter informações da impressora.";
        }

        // Alocar buffer e obter informações da impressora
        PRINTER_INFO_2 *pi2 = (PRINTER_INFO_2 *)malloc(needed);
        if (!GetPrinter(hPrinter, 2, (LPBYTE)pi2, needed, &needed))
        {
            DWORD errorCode = GetLastError();
            free(pi2);
            ClosePrinter(hPrinter);
            return "Erro ao obter informações da impressora.";
        }

        // Verificar o status da impressora
        if (pi2->Status & PRINTER_STATUS_ERROR)
        {
            free(pi2);
            ClosePrinter(hPrinter);
            return "A impressora está em estado de erro.";
        }
        if (pi2->Status & PRINTER_STATUS_OFFLINE)
        {
            free(pi2);
            ClosePrinter(hPrinter);
            return "A impressora está offline.";
        }
        if (pi2->Status & PRINTER_STATUS_PAPER_JAM)
        {
            free(pi2);
            ClosePrinter(hPrinter);
            return "A impressora está com atolamento de papel.";
        }

        // Liberar recursos
        free(pi2);
        ClosePrinter(hPrinter);

        // Criar o contexto de impressão usando o nome convertido corretamente
        printer_dc = CreateDCW(L"WINSPOOL", wPrinterName.c_str(), NULL, NULL);
        if (printer_dc == NULL)
        {
            DWORD errorCode = GetLastError();
            LPWSTR errorMsg = nullptr;
            FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorMsg,
                0,
                NULL);

            std::wstring errorMessage;
            if (errorMsg)
            {
                errorMessage = errorMsg;
                LocalFree(errorMsg);
            }
            else
            {
                errorMessage = L"Erro desconhecido ao criar o Device Context.";
            }

            return std::string(errorMessage.begin(), errorMessage.end());
        }

        // Configurar o DC para impressão
        if (printer_dc != NULL)
        {
            // Configurar o modo de mapeamento
            SetMapMode(printer_dc, MM_TEXT);

            // Configurar a orientação padrão e qualidade
            PRINTDLG pd = {0};
            pd.lStructSize = sizeof(PRINTDLG);
            pd.hDC = printer_dc;

            // Configurar qualidade de impressão usando DeviceMode
            DEVMODE *pDevMode = nullptr;
            if (printer_dc)
            {
                pDevMode = (DEVMODE *)GlobalLock(GlobalAlloc(GHND, sizeof(DEVMODE)));
                if (pDevMode)
                {
                    pDevMode->dmSize = sizeof(DEVMODE);
                    pDevMode->dmFields = DM_PRINTQUALITY;
                    pDevMode->dmPrintQuality = DMRES_HIGH; // Alta qualidade

                    // Aplicar configurações
                    ResetDC(printer_dc, pDevMode);

                    GlobalUnlock(pDevMode);
                    GlobalFree(pDevMode);
                }
            }

            // Verificar se as configurações foram aplicadas
            if (GetDeviceCaps(printer_dc, RASTERCAPS) == 0)
            {
                DeleteDC(printer_dc);
                printer_dc = NULL;
                return "Falha ao configurar o dispositivo de impressão";
            }
        }

        int caps = GetDeviceCaps(printer_dc, TECHNOLOGY);
        if (caps == 0)
        {
            DeleteDC(printer_dc);
            printer_dc = NULL;
            return "Falha ao obter capacidades do Device Context.";
        }
        return "";
    }

    bool WindowsPrinter::Print(const std::string &filePath, const PdfiumOption &options)
    {
        if (!printer_dc)
        {
            return false;
        }

        try
        {
            auto filePathW = std::wstring(filePath.begin(), filePath.end());
            auto doc = std::make_unique<PDFDocument>(std::move(filePathW));

            if (!doc->LoadDocument())
            {
                return false;
            }

            // Configurar o DC antes da impressão
            SetGraphicsMode(printer_dc, GM_ADVANCED);
            SetMapMode(printer_dc, MM_TEXT);

            // Imprimir o documento
            doc->PrintDocument(printer_dc, options);

            // Garantir que tudo foi enviado para a impressora
            GdiFlush();

            return true;
        }
        catch (const std::exception &e)
        {
            // Log do erro
            std::cerr << "Erro durante a impressão: " << e.what() << std::endl;
            return false;
        }
    }

    // Factory function implementation for Windows
    std::unique_ptr<PrinterInterface> CreatePrinter()
    {
        return std::unique_ptr<PrinterInterface>(new WindowsPrinter());
    }

}
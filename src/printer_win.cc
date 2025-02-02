#include "printer_win.h"
#include <iostream>

namespace printer_pdf_node_electron
{
Unique_HDC GetPrinterDC(const Napi::Value &printerName)
{
    const Napi::String printerNameV8Str = printerName.ToString();

    Unique_HDC printerDC(::CreateDCW(L"WINSPOOL", reinterpret_cast<LPCWSTR>(printerNameV8Str.Utf16Value().c_str()), NULL, NULL));

    return printerDC;
}
Unique_HPrinter GetPrinterHanlde(const Napi::Value &printerName)
{
    const Napi::String printerNameV8Str = printerName.ToString();

    HANDLE handle = NULL;

    ::OpenPrinterA(const_cast<LPSTR>(printerNameV8Str.Utf8Value().c_str()), &handle, NULL);

    Unique_HPrinter printerH(handle);

    return printerH;
}
} 
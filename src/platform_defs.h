#ifndef PLATFORM_DEFS_H
#define PLATFORM_DEFS_H

#ifdef _WIN32
#include <windows.h>
typedef HDC DeviceContext;
#else
typedef void *DeviceContext;
#endif

namespace printer_pdf_electron_node
{

#ifdef _WIN32
    using WideString = LPCWSTR;
#else
    using WideString = const wchar_t *;
#endif

}

#endif
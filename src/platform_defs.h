#ifndef PLATFORM_DEFS_H
#define PLATFORM_DEFS_H
#include "inc.h"

namespace printer_pdf_electron_node
{

#ifdef _WIN32
    using DeviceContext = HDC;
    using WideString = LPCWSTR;
#else
    using DeviceContext = void *;
    using WideString = const wchar_t *;
#endif

}

#endif
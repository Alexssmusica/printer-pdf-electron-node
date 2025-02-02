#ifndef PLATFORM_DEFS_H
#define PLATFORM_DEFS_H

#include "inc.h"

namespace printer_pdf_node_electron {

#ifdef _WIN32
    using DeviceContext = HDC;
    using WideString = LPCWSTR;
#else
    using DeviceContext = void*;  // Não usado no Linux
    using WideString = const wchar_t*;
#endif

} // namespace printer_pdf_node_electron

#endif 
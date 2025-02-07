#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <locale>
#include <codecvt>
#include <sys/stat.h>
#endif

namespace printer_pdf_electron_node
{
    inline std::string GetFormattedTimestamp() {
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
        return std::string(buffer);
    }

    inline void LogError(const std::string &message)
    {
        #ifdef _WIN32
        CreateDirectoryA("logs", NULL);
        #else
        mkdir("logs", 0777);
        #endif

        static std::ofstream logFile("logs/printer-errors.log", std::ios::app);
        if (logFile.is_open())
        {
            #ifdef _WIN32
            int wlen = MultiByteToWideChar(CP_ACP, 0, message.c_str(), -1, NULL, 0);
            wchar_t* wstr = new wchar_t[wlen];
            MultiByteToWideChar(CP_ACP, 0, message.c_str(), -1, wstr, wlen);
            
            int utf8len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
            char* utf8str = new char[utf8len];
            WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8str, utf8len, NULL, NULL);
            
            logFile << "[" << GetFormattedTimestamp() << "] " << utf8str << std::endl;
            
            delete[] wstr;
            delete[] utf8str;
            #else
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            logFile << "[" << GetFormattedTimestamp() << "] " << message << std::endl;
            #endif
            
            logFile.flush();
        }
    }
}

#endif
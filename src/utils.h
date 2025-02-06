#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <ctime>

namespace printer_pdf_electron_node
{
    inline void LogError(const std::string &message)
    {
        static std::ofstream logFile("printer-errors.log", std::ios::app);
        if (logFile.is_open())
        {
            logFile << "[" << std::time(nullptr) << "] " << message << std::endl;
            logFile.flush();
        }
    }
}

#endif
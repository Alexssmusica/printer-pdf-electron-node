#include "inc.h"
#include "printer_interface.h"
#include "pdfium_option.h"

#define CHECK_STRING(name)                     \
    auto err##name = checkString(name, #name); \
    if (!err##name)                            \
    {                                          \
        return;                                \
    }

namespace printer_pdf_node_electron
{
    bool checkString(const Napi::Value &arg, const std::string &name)
    {
        if (!arg.IsString())
        {
            std::stringstream ss;
            ss << "Wrong type of" << name;
            std::string s = ss.str();
            Napi::Error::New(arg.Env(), s).ThrowAsJavaScriptException();
            return false;
        }
        return true;
    }

    void checkError(Napi::Env env, const std::string &failure, const std::string &arg)
    {
        unsigned long err = FPDF_GetLastError();

        std::stringstream ss;
        ss << "Fail to " << failure << " , for the " << arg << std::endl;
        ss << "Rason: ";
        switch (err)
        {
        case FPDF_ERR_SUCCESS:
            ss << "Success";
            break;
        case FPDF_ERR_UNKNOWN:
            ss << "Unknown error";
            break;
        case FPDF_ERR_FILE:
            ss << "File not found or could not be opened";
            break;
        case FPDF_ERR_FORMAT:
            ss << "File not in PDF format or corrupted";
            break;
        case FPDF_ERR_PASSWORD:
            ss << "Password required or incorrect password";
            break;
        case FPDF_ERR_SECURITY:
            ss << "Unsupported security scheme";
            break;
        case FPDF_ERR_PAGE:
            ss << "Page not found or content error";
            break;
        default:
            ss << "Unknown error " << err;
        }
        std::string s = ss.str();
        Napi::Error::New(env, s).ThrowAsJavaScriptException();
    }

    void PrintPDF(const Napi::CallbackInfo &args)
    {
        Napi::Env env = args.Env();
        const Napi::Value &printerName = args[0];
        const Napi::Value &filePath = args[1];
        const Napi::Value &v8_options = args[2];

        CHECK_STRING(printerName)
        CHECK_STRING(filePath)

        auto printer = CreatePrinter();
        if (!printer->Initialize(printerName)) {
            Napi::Error::New(env, "Failed to initialize printer").ThrowAsJavaScriptException();
            return;
        }

        std::unique_ptr<PdfiumOption> options(V8OptionToStruct(v8_options));
        auto filePathStr = filePath.As<Napi::String>();

        try {
            if (!printer->Print(filePathStr.Utf8Value(), *options)) {
                Napi::Error::New(env, "Failed to print document").ThrowAsJavaScriptException();
                return;
            }
        } catch (const std::exception &e) {
            std::string basicErrInfo("Failed to print document: ");
            Napi::Error::New(env, basicErrInfo + e.what()).ThrowAsJavaScriptException();
            return;
        }
    }

    Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        FPDF_InitLibrary();
        exports.Set(Napi::String::New(env, "printPDF"),
                    Napi::Function::New(env, PrintPDF));
        return exports;
    }

    NODE_API_MODULE(printer_pdf_node_electron, Init)
} 
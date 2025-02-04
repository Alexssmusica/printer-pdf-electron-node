#include "pdfium_option.h"

#define INIT_PDFIUM_OPTION(name)                              \
    if (obj.Has(#name))                                       \
    {                                                         \
        ops->name = (obj).Get(#name).ToNumber().Int32Value(); \
    }

#define INIT_PDFIUM_FLOAT_OPTION(name)                        \
    if (obj.Has(#name))                                       \
    {                                                         \
        ops->name = (obj).Get(#name).ToNumber().FloatValue(); \
    }

namespace printer_pdf_electron_node
{
    std::unique_ptr<PdfiumOption> V8OptionToStruct(const Napi::Value &options)
    {
        auto ops = std::make_unique<PdfiumOption>();
        if (options.IsObject())
        {
            auto obj = options.ToObject();

            INIT_PDFIUM_OPTION(dpi);
            INIT_PDFIUM_OPTION(copies);
            INIT_PDFIUM_OPTION(width);
            INIT_PDFIUM_OPTION(height);

            ops->dpi = ops->dpi / 72;

            // Process paper size
            if (obj.Has("paperSize"))
            {
                std::string paperSize = obj.Get("paperSize").ToString().Utf8Value();
                if (paperSize == "A4")
                    ops->paperSize = PaperSize::A4;
                else if (paperSize == "LETTER")
                    ops->paperSize = PaperSize::LETTER;
                else if (paperSize == "LEGAL")
                    ops->paperSize = PaperSize::LEGAL;
                else if (paperSize == "A3")
                    ops->paperSize = PaperSize::A3;
                else
                    ops->paperSize = PaperSize::CUSTOM;
            }

            // Process margins
            if (obj.Has("margins"))
            {
                auto margins = obj.Get("margins").ToObject();
                if (margins.Has("top"))
                    ops->margins.top = margins.Get("top").ToNumber().FloatValue();
                if (margins.Has("right"))
                    ops->margins.right = margins.Get("right").ToNumber().FloatValue();
                if (margins.Has("bottom"))
                    ops->margins.bottom = margins.Get("bottom").ToNumber().FloatValue();
                if (margins.Has("left"))
                    ops->margins.left = margins.Get("left").ToNumber().FloatValue();
            }

            // Process fit to page option
            if (obj.Has("fitToPage"))
            {
                ops->fitToPage = obj.Get("fitToPage").ToBoolean();
            }

            if (obj.Has("pageList"))
            {
                auto pageList = obj.Get("pageList");
                Napi::Array arr = pageList.As<Napi::Array>();
                for (unsigned int i = 0; i < arr.Length(); ++i)
                {
                    const Napi::Array &item = (arr.Get(i)).As<Napi::Array>();
                    auto pair = std::make_pair(
                        item.Get(static_cast<unsigned int>(0)).As<Napi::Number>().Int32Value(),
                        item.Get(static_cast<unsigned int>(1)).As<Napi::Number>().Int32Value());
                    ops->page_list.push_back(std::move(pair));
                }
            }
        }

        return ops;
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/PNG.h>

#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace PNG
        {
            Plugin::Plugin()
            {}

            std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Plugin>(new Plugin);
                out->_init(
                    "PNG",
                    DJV_TEXT("plugin_png_i_o"),
                    { ".png" },
                    context);
                return out;
            }

            std::shared_ptr<IO::IRead> Plugin::read(const System::File::Info& fileInfo, const IO::ReadOptions& options) const
            {
                return Read::create(fileInfo, options, _textSystem, _resourceSystem, _logSystem);
            }

            std::shared_ptr<IO::IWrite> Plugin::write(const System::File::Info& fileInfo, const IO::Info& info, const IO::WriteOptions& options) const
            {
                return Write::create(fileInfo, info, options, _textSystem, _resourceSystem, _logSystem);
            }

        } // namespace PNG
    } // namespace AV
} // namespace djv

extern "C"
{
    void djvPngError(png_structp in, png_const_charp msg)
    {
        auto error = reinterpret_cast<djv::AV::PNG::ErrorStruct *>(png_get_error_ptr(in));
        error->messages.push_back(msg);
        longjmp(png_jmpbuf(in), 1);
    }

    void djvPngWarning(png_structp in, png_const_charp msg)
    {
        auto error = reinterpret_cast<djv::AV::PNG::ErrorStruct *>(png_get_error_ptr(in));
        error->messages.push_back(msg);
    }

} // extern "C"

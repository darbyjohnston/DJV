//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/PNG.h>

#include <djvCore/String.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PNG
            {
                Plugin::Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(
                    const std::shared_ptr<ResourceSystem>& resourceSystem,
                    const std::shared_ptr<LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        "PNG",
                        DJV_TEXT("This plugin provides Portable Network Graphics (PNG) image I/O."),
                        { ".png" },
                        resourceSystem,
                        logSystem);
                    return out;
                }

                std::shared_ptr<IRead> Plugin::read(const std::string & fileName) const
                {
                    return Read::create(fileName, _resourceSystem, _logSystem);
                }

                std::shared_ptr<IWrite> Plugin::write(const std::string & fileName, const Info & info) const
                {
                    return Write::create(fileName, info, _resourceSystem, _logSystem);
                }

                extern "C"
                {
                    void djvPngError(png_structp in, png_const_charp msg)
                    {
                        auto error = reinterpret_cast<ErrorStruct *>(png_get_error_ptr(in));
                        DJV_STRNCPY(error->msg, msg, String::cStringLength);
                        longjmp(png_jmpbuf(in), 1);
                    }

                    void djvPngWarning(png_structp in, png_const_charp msg)
                    {
                        auto error = reinterpret_cast<ErrorStruct *>(png_get_error_ptr(in));
                        DJV_STRNCPY(error->msg, msg, String::cStringLength);
                        longjmp(png_jmpbuf(in), 1);
                    }

                } // extern "C"

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv


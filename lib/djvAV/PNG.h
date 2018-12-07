//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#pragma once

#include <djvAV/SequenceIO.h>

#include <djvCore/String.h>

#include <png.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This plugin provides Portable Network Graphics (PNG) image I/O.
            //!
            //! References:
            //! - http://www.libpng.org
            namespace PNG
            {
                static const std::string pluginName = "PNG";
                static const std::set<std::string> fileExtensions = { ".png" };

                struct ErrorStruct
                {
                    char msg[Core::String::cStringLength];
                };

                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read();

                    static std::shared_ptr<Read> create(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);

                private:
                    Info _open(const std::string &, const Speed &, Duration) override;
                    std::shared_ptr<Image> _read() override;
                    void _close() override;

                    DJV_PRIVATE();
                };
                
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write();

                    static std::shared_ptr<Write> create(
                        const std::string & fileName,
                        const Info &,
                        const std::shared_ptr<Queue> &,
                        const std::shared_ptr<Core::Context> &);

                protected:
                    void _open(const std::string & fileName, const Info &) override;
                    void _write(const std::shared_ptr<Image> &) override;
                    void _close() override;
                    void _exit() override;

                private:
                    DJV_PRIVATE();
                };

                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    std::shared_ptr<IRead> read(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &) const override;
                    std::shared_ptr<IWrite> write(
                        const std::string & fileName,
                        const Info &,
                        const std::shared_ptr<Queue> &) const override;
                };

                extern "C"
                {
                    void djvPngError(png_structp, png_const_charp);
                    void djvPngWarning(png_structp, png_const_charp);

                } // extern "C"

            } // namespace PNG
        } // namespace IO
    } // namespace AV
} // namespace djv

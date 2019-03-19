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

#pragma once

#include <djvAV/SequenceIO.h>

#include <tiffio.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides Tagged Image File Format (TIFF) image I/O.
            //!
            //! References:
            //! - http://www.libtiff.org
            namespace TIFF
            {
                static const std::string pluginName = "TIFF";
                static const std::set<std::string> fileExtensions = { ".tif", ".tiff" };

                //! This enumeration provides the TIFF compression types.
                enum class Compression
                {
                    None,
                    RLE,
                    LZW,

                    Count,
                    First
                };
                DJV_ENUM_HELPERS(Compression);

                //! This struct provides the TIFF settings.
                struct Settings
                {
                    Compression compression = Compression::LZW;
                };

                //! Load a TIFF palette.
                void paletteLoad(
                    uint8_t *  out,
                    int        size,
                    int        bytes,
                    uint16_t * red,
                    uint16_t * green,
                    uint16_t * blue);

                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    static std::shared_ptr<Read> create(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &,
                        Core::Context *);

                protected:
                    Info _readInfo(const std::string & fileName) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string & fileName) override;

                private:
                    struct File;
                    Info _open(const std::string &, File &);
                };
                
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write(const Settings &);

                public:
                    ~Write();

                    static std::shared_ptr<Write> create(
                        const std::string & fileName,
                        const Settings &,
                        const Info &,
                        const std::shared_ptr<Queue> &,
                        Core::Context *);

                protected:
                    void _write(const std::string & fileName, const std::shared_ptr<Image::Image> &) override;

                private:
                    DJV_PRIVATE();
                };

                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    static std::shared_ptr<Plugin> create(Core::Context *);

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value &) override;

                    std::shared_ptr<IRead> read(
                        const std::string & fileName,
                        const std::shared_ptr<Queue> &) const override;
                    std::shared_ptr<IWrite> write(
                        const std::string & fileName,
                        const Info &,
                        const std::shared_ptr<Queue> &) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace TIFF
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::TIFF::Compression);

    picojson::value toJSON(const AV::IO::TIFF::Settings &);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, AV::IO::TIFF::Settings &);

} // namespace djv

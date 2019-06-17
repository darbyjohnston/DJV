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

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides Cineon image I/O.
            //!
            //! References:
            //! - Kodak, "4.5 DRAFT - Image File Format Proposal for Digital Pictures"
            namespace Cineon
            {
                static const std::string pluginName = "Cineon";
                static const std::set<std::string> fileExtensions = { ".cin" };

                //! This enumeration provides the color profiles.
                enum class ColorProfile
                {
                    Auto,
                    Raw,
                    FilmPrint,

                    Count,
                    First = Auto
                };
                DJV_ENUM_HELPERS(ColorProfile);

                //! This struct provides options to convert from linear color space to
                //! the Cineon film print color space.
                //!
                //! - Black point range = 0 - 1023, default value = 95
                //! - White point range = 0 - 1023, default value = 685
                //! - Gamma range = 0.01 - 4.0, default value = 1.7
                struct LinearToFilmPrint
                {
                    uint16_t black = 95;
                    uint16_t white = 685;
                    float    gamma = 1.7f;
                };

                //! Create a linear color space to Cineon film print color space LUT.
                static Image::Data linearToFilmPrintLut(const LinearToFilmPrint&);

                //! This struct provides options to convert from the Cineon film print
                //! color space to a linear color space.
                //!
                //! - Black point range = 0 - 1023, default value = 95
                //! - White point range = 0 - 1023, default value = 685
                //! - Gamma range = 0.01 - 4.0, default value = 1.7
                //! - Soft clip range = 0 - 50, default value = 0
                struct FilmPrintToLinear
                {
                    uint16_t black    = 95;
                    uint16_t white    = 685;
                    uint16_t gamma    = 1.7f;
                    uint8_t  softClip = 0;
                };

                //! Create a Cineon film print color space to linear space LUT.
                static Image::Data filmPrintToLinearLut(const FilmPrintToLinear&);

                //! This enumeration provides additional image tags for Cineon files.
                enum class Tag
                {
                    SourceOffset,
                    SourceFile,
                    SourceTime,
                    SourceInputDevice,
                    SourceInputModel,
                    SourceInputSerial,
                    SourceInputPitch,
                    SourceGamma,
                    FilmFormat,
                    FilmFrame,
                    FilmFrameRate,
                    FilmFrameID,
                    FilmSlate,

                    Count,
                    First = SourceOffset
                };
                DJV_ENUM_HELPERS(Tag);

                //! This struct provides the settings.
                struct Settings
                {
                    ColorProfile      inputProfile    = ColorProfile::Auto;
                    FilmPrintToLinear inputFilmPrint;
                    ColorProfile      outputProfile   = ColorProfile::FilmPrint;
                    FilmPrintToLinear outputFilmPrint;
                };

                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const std::string & fileName,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Info _readInfo(const std::string &) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string &) override;

                private:
                    Info _open(const std::string &, Core::FileSystem::FileIO &);
                };
                
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write(const Settings &);

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const std::string & fileName,
                        const Settings &,
                        const Info &,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

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
                    static std::shared_ptr<Plugin> create(
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                    picojson::value getOptions() const override;
                    void setOptions(const picojson::value &) override;

                    std::shared_ptr<IRead> read(const std::string & fileName) const override;
                    std::shared_ptr<IWrite> write(const std::string & fileName, const Info &) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace Cineon
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::Cineon::ColorProfile);

    picojson::value toJSON(const AV::IO::Cineon::FilmPrintToLinear&);
    picojson::value toJSON(const AV::IO::Cineon::LinearToFilmPrint&);
    picojson::value toJSON(const AV::IO::Cineon::Settings&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::Cineon::LinearToFilmPrint&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::Cineon::FilmPrintToLinear&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::Cineon::Settings&);

} // namespace djv

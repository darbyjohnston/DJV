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

                //! This enumeration provides the Cineon file color profiles.
                enum class ColorProfile
                {
                    Raw,
                    FilmPrint,

                    Count,
                    First = Raw
                };

                //! This constant provides the Cineon file header magic numbers.
                const uint32_t magic[] =
                {
                    0x802a5fd7,
                    0xd75f2a80
                };

                //! This enumeration provides the Cineon file image orientations.
                enum class Orient
                {
                    LeftRightTopBottom,
                    LeftRightBottomTop,
                    RightLeftTopBottom,
                    RightLeftBottomTop,
                    TopBottomLeftRight,
                    TopBottomRightLeft,
                    BottomTopLeftRight,
                    BottomTopRightLeft
                };

                //! This enumeration provides the Cineon file descriptors.
                enum class Descriptor
                {
                    Luminance,
                    RedFilmPrint,
                    GreenFilmPrint,
                    BlueFilmPrint,
                    RedCCIRXA11,
                    GreenCCIRXA11,
                    BlueCCIRXA11
                };

                //! This stuct provides the Cineon file header.
                struct Header
                {
                    struct File
                    {
                        uint32_t magic;
                        uint32_t imageOffset;
                        uint32_t headerSize;
                        uint32_t industryHeaderSize;
                        uint32_t userHeaderSize;
                        uint32_t size;
                        char     version[8];
                        char     name[100];
                        char     time[24];
                        uint8_t  pad[36];
                    }
                        file;

                    struct Image
                    {
                        uint8_t orient;
                        uint8_t channels;
                        uint8_t pad[2];

                        struct Channel
                        {
                            uint8_t  descriptor[2];
                            uint8_t  bitDepth;
                            uint8_t  pad;
                            uint32_t size[2];
                            float    lowData;
                            float    lowQuantity;
                            float    highData;
                            float    highQuantity;
                        }
                            channel[8];

                        float    white[2];
                        float    red[2];
                        float    green[2];
                        float    blue[2];
                        char     label[200];
                        uint8_t  pad2[28];
                        uint8_t  interleave;
                        uint8_t  packing;
                        uint8_t  dataSign;
                        uint8_t  dataSense;
                        uint32_t linePadding;
                        uint32_t channelPadding;
                        uint8_t  pad3[20];
                    }
                        image;

                    struct Source
                    {
                        int32_t offset[2];
                        char    file[100];
                        char    time[24];
                        char    inputDevice[64];
                        char    inputModel[32];
                        char    inputSerial[32];
                        float   inputPitch[2];
                        float   gamma;
                        char    pad[40];
                    }
                        source;

                    struct Film
                    {
                        uint8_t  id;
                        uint8_t  type;
                        uint8_t  offset;
                        uint8_t  pad;
                        uint8_t  prefix;
                        uint32_t count;
                        char     format[32];
                        uint32_t frame;
                        float    frameRate;
                        char     frameId[32];
                        char     slate[200];
                        char     pad2[740];
                    }
                        film;
                };

                //! Zero out the data in a Cineon file header.
                void zero(Header&);

                //! Read a Cineon file header.
                //!
                //! Throws:
                //! - std::exception
                Header read(Core::FileSystem::FileIO&, Info&, ColorProfile&);
                
                //! Write a Cineon file header.
                //!
                //! Throws:
                //! - std::exception
                void write(Core::FileSystem::FileIO&, const Info& info, ColorProfile);

                //! Finish writing the Cineon file header after image data is written.
                void writeFinish(Core::FileSystem::FileIO&);

                //! This struct provides the Cineon file I/O options.
                struct Options
                {
                    std::string colorSpace;
                };

                //! This class provides the Cineon file reader.
                class Read : public ISequenceRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const Core::FileSystem::FileInfo&,
                        const ReadOptions&,
                        const Options&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Info _readInfo(const std::string &) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string &) override;

                private:
                    Info _open(const std::string &, Core::FileSystem::FileIO &);

                    DJV_PRIVATE();
                };

                //! This class provides the Cineon file writer.
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const Core::FileSystem::FileInfo&,
                        const Info &,
                        const WriteOptions&,
                        const Options&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    Image::Layout _getImageLayout() const override;
                    void _write(const std::string & fileName, const std::shared_ptr<Image::Image> &) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the Cineon file I/O plugin.
                class Plugin : public ISequencePlugin
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

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info &, const WriteOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace Cineon
        } // namespace IO
    } // namespace AV

    picojson::value toJSON(const AV::IO::Cineon::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::Cineon::Options&);

} // namespace djv

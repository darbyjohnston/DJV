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
            //! This namespace provides DPX image I/O.
            //!
            //! References:
            //! - SMPTE, "SMPTE 268M-2003"
            //!   http://www.smpte.org
            //! - Cinesite, "Conversion of 10-bit Log Film Data To 8-bit Linear or Video Data"
            namespace DPX
            {
                static const std::string pluginName = "DPX";
                static const std::set<std::string> fileExtensions = { ".dpx" };

                //! This enumeration provides the DPX file format versions.
                enum class Version
                {
                    _1_0,
                    _2_0,

                    Count,
                    First = _1_0
                };
                DJV_ENUM_HELPERS(Version);

                //! This enumeration provides DPX file endian options.
                enum class Endian
                {
                    Auto, //!< Use the endian of the machine 
                    MSB,  //!< Most-significant byte first
                    LSB,  //!< Least-significant byte first

                    Count,
                    First = Auto
                };
                DJV_ENUM_HELPERS(Endian);

                //! This enumeration provides the DPX file color profiles.
                enum class ColorProfile
                {
                    Raw,
                    FilmPrint,

                    Count,
                    First = Raw
                };

                //! This constant provides the DPX file magic numbers.
                static const char magic[][5] =
                {
                    "SDPX",
                    "XPDS"
                };

                //! This enumeration provides the DPX file image orientations.
                enum class Orient
                {
                    LeftRightTopBottom,
                    RightLeftTopBottom,
                    LeftRightBottomTop,
                    RightLeftBottomTop,
                    TopBottomLeftRight,
                    TopBottomRightLeft,
                    BottomTopLeftRight,
                    BottomTopRightLeft
                };

                //! This enumeration provides the DPX file channel descriptors.
                enum class Descriptor
                {
                    User            = 0,
                    R               = 1,
                    G               = 2,
                    B               = 3,
                    A               = 4,
                    L               = 6,
                    Chroma          = 7,
                    Z               = 8,
                    CompositeVideo  = 9,
                    RGB             = 50,
                    RGBA            = 51,
                    ABGR            = 52,
                    CBYCRY          = 100,
                    CBYACRYA        = 101,
                    CBYCR           = 102,
                    CBYCRA          = 103,
                    User2           = 150,
                    User3           = 151,
                    User4           = 152,
                    User5           = 153,
                    User6           = 154,
                    User7           = 155,
                    User8           = 156
                };

                //! This enumeration provides the DPX file color transfer information.
                enum class Transfer
                {
                    User,
                    FilmPrint,
                    Linear,
                    Log,
                    Video,
                    SMPTE_274M,
                    ITU_R_709_4,
                    ITU_R_601_5_B_OR_G,
                    ITU_R_601_5_M,
                    NTSC,
                    PAL,
                    Z,
                    ZHomogeneous
                };

                //! This enumeration provides the colorimetric information for 1.0 version
                //! DPX files.
                enum class Colorimetric_1_0
                {
                    User = 0,
                    FilmPrint = 1,
                    Video = 4,
                    SMPTE_240M = 5,
                    ITU_R_709_1 = 6,
                    ITU_R_601_2_B_OR_G = 7,
                    ITU_R_601_2_M = 8,
                    NTSC = 9,
                    PAL = 10
                };

                //! This enumeration provides the colorimetric information for 2.0 version
                //! DPX files.
                enum Colorimetric_2_0
                {
                    User = 0,
                    FilmPrint = 1,
                    Video = 4,
                    SMPTE_274M = 5,
                    ITU_R_709_4 = 6,
                    ITU_R_601_5_B_OR_G = 7,
                    ITU_R_601_5_M = 8,
                    NTSC = 9,
                    PAL = 10
                };

                //! This enumeration provides how the DPX file data components are ordered.
                enum class Components
                {
                    Pack,
                    TypeA,
                    TypeB
                };

                //! This stuct provides the DPX file header.
                struct Header
                {
                    struct File
                    {
                        uint32_t magic;
                        uint32_t imageOffset;
                        char     version[8];
                        uint32_t size;
                        uint32_t dittoKey;
                        uint32_t headerSize;
                        uint32_t industryHeaderSize;
                        uint32_t userHeaderSize;
                        char     name[100];
                        char     time[24];
                        char     creator[100];
                        char     project[200];
                        char     copyright[200];
                        uint32_t encryptionKey;
                        uint8_t  pad[104];
                    }
                        file;

                    struct Image
                    {
                        uint16_t orient;
                        uint16_t elemSize;
                        uint32_t size[2];

                        struct Elem
                        {
                            uint32_t dataSign;
                            uint32_t lowData;
                            float    lowQuantity;
                            uint32_t highData;
                            float    highQuantity;
                            uint8_t  descriptor;
                            uint8_t  transfer;
                            uint8_t  colorimetric;
                            uint8_t  bitDepth;
                            uint16_t packing;
                            uint16_t encoding;
                            uint32_t dataOffset;
                            uint32_t linePadding;
                            uint32_t elemPadding;
                            char     description[32];
                        };

                        Elem    elem[8];
                        uint8_t pad[52];
                    }
                        image;

                    struct Source
                    {
                        uint32_t offset[2];
                        float    center[2];
                        uint32_t size[2];
                        char     file[100];
                        char     time[24];
                        char     inputDevice[32];
                        char     inputSerial[32];
                        uint16_t border[4];
                        uint32_t pixelAspect[2];
                        float    scanSize[2]; // V2.0
                        uint8_t  pad[20];
                    }
                        source;

                    struct Film
                    {
                        char     id[2];
                        char     type[2];
                        char     offset[2];
                        char     prefix[6];
                        char     count[4];
                        char     format[32];
                        uint32_t frame;
                        uint32_t sequence;
                        uint32_t hold;
                        float    frameRate;
                        float    shutter;
                        char     frameId[32];
                        char     slate[100];
                        uint8_t  pad[56];
                    }
                        film;

                    struct TV
                    {
                        uint32_t timecode;
                        uint32_t userBits;
                        uint8_t  interlace;
                        uint8_t  field;
                        uint8_t  videoSignal;
                        uint8_t  pad;
                        float    sampleRate[2];
                        float    frameRate;
                        float    timeOffset;
                        float    gamma;
                        float    blackLevel;
                        float    blackGain;
                        float    breakpoint;
                        float    whiteLevel;
                        float    integrationTimes;
                        uint8_t  pad2[76];
                    }
                        tv;
                };

                //! Zero out the data in a DPX file header.
                void zero(Header&);

                //! Read a DPX file header.
                //!
                //! Throws:
                //! - std::exception
                Header read(Core::FileSystem::FileIO&, Info&, ColorProfile&);
                
                //! Write a DPX file header.
                //!
                //! Throws:
                //! - std::exception
                void write(Core::FileSystem::FileIO&, const Info& info, Version, Endian, ColorProfile);

                //! Finish writing the DPX file header after image data is written.
                void writeFinish(Core::FileSystem::FileIO&);

                //! This struct provides the DPX file I/O options.
                struct Options
                {
                    Version         version         = Version::_2_0;
                    Endian          endian          = Endian::Auto;
                    std::string     colorSpace;
                };

                //! This class provides the DPX file reader.
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

                //! This class provides the DPX file writer.
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

                //! This class provides the DPX file I/O plugin.
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

            } // namespace DPX
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::DPX::Version);
    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::DPX::Endian);

    picojson::value toJSON(const AV::IO::DPX::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, AV::IO::DPX::Options&);

} // namespace djv

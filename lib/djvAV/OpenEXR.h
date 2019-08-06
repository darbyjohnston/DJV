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

#include <djvCore/BBox.h>

#include <ImathBox.h>
#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfInputFile.h>
#include <ImfPixelType.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            //! This namespace provides OpenEXR file I/O.
            //!
            //! References:
            //! - http://www.openexr.com
            //!
            //! \todo Add support for writing luminance/chroma images.
            //! \todo Add better support for tiled images.
            namespace OpenEXR
            {
                static const std::string pluginName = "OpenEXR";
                static const std::set<std::string> fileExtensions = { ".exr" };

                //! This enumeration provides how the OpenEXR channels are grouped together.
                enum class Channels
                {
                    None,
                    Known,
                    All,

                    Count,
                    First = None
                };
                DJV_ENUM_HELPERS(Channels);

                //! This struct provides an OpenEXR image channel.
                struct Channel
                {
                    Channel();
                    Channel(
                        const std::string& name,
                        Image::DataType    type,
                        const glm::ivec2&  sampling = glm::ivec2(1, 1));

                    std::string     name;
                    Image::DataType type     = Image::DataType::None;
                    glm::ivec2      sampling = glm::ivec2(1, 1);
                };

                //! This struct provides an OpenEXR image layer.
                struct Layer
                {
                    Layer(
                        const std::vector<Channel>& channels        = std::vector<Channel>(),
                        bool                        luminanceChroma = false);

                    std::string          name;
                    std::vector<Channel> channels;
                    bool                 luminanceChroma = false;
                };

                //! This enumeration provides the OpenEXR file compression types.
                enum class Compression
                {
                    None,
                    RLE,
                    ZIPS,
                    ZIP,
                    PIZ,
                    PXR24,
                    B44,
                    B44A,
                    DWAA,
                    DWAB,

                    Count,
                    First = None
                };
                DJV_ENUM_HELPERS(Compression);

                //! Get a layer name from a list of channel names.
                std::string getLayerName(const std::vector<std::string>&);

                //! Get the channels that aren't in any layer.
                Imf::ChannelList getDefaultLayer(const Imf::ChannelList&);

                //! Find a channel by name.
                const Imf::Channel* find(const Imf::ChannelList&, std::string&);

                //! Get a list of layers from Imf channels.
                std::vector<Layer> getLayers(const Imf::ChannelList&, Channels);

                //! Read the tags from an Imf header.
                void readTags(const Imf::Header&, Tags&, Core::Time::Speed&);

                //! Write tags to an Imf header.
                void writeTags(const Tags&, const Core::Time::Speed&, Imf::Header&);

                //! Convert an Imath box type.
                Core::BBox2i fromImath(const Imath::Box2i&);

                //! Convert to an Imf pixel type.
                Imf::PixelType toImf(Image::DataType);

                //! Convert from an Imf pixel type.
                Image::DataType fromImf(Imf::PixelType);

                //! Convert from an Imf channel.
                Channel fromImf(const std::string& name, const Imf::Channel&);

                //! This struct provides the OpenEXR file I/O optioms.
                struct Options
                {
                    size_t      threadCount         = 4;
                    Channels    channels            = Channels::Known;
                    Compression compression         = Compression::None;
                    float       dwaCompressionLevel = 45.f;
                    std::string colorSpace;
                };

                //! This class provides a memory-mapped input stream.
                class MemoryMappedIStream : public Imf::IStream
                {
                    DJV_NON_COPYABLE(MemoryMappedIStream);

                public:
                    MemoryMappedIStream(const char fileName[]);
                    ~MemoryMappedIStream() override;

                    bool isMemoryMapped() const override;
                    char* readMemoryMapped(int n) override;
                    bool read(char c[], int n) override;
                    Imf::Int64 tellg() override;
                    void seekg(Imf::Int64 pos) override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the OpenEXR file reader.
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
                    Info _readInfo(const std::string & fileName) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string & fileName) override;

                private:
                    struct File;
                    Info _open(const std::string &, File &);

                    DJV_PRIVATE();
                };
                
                //! This class provides the OpenEXR file writer.
                class Write : public ISequenceWrite
                {
                    DJV_NON_COPYABLE(Write);

                protected:
                    Write();

                public:
                    ~Write() override;

                    static std::shared_ptr<Write> create(
                        const Core::FileSystem::FileInfo&,
                        const Info&,
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

                //! This class provides the OpenEXR file I/O plugin.
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

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::OpenEXR::Compression);
    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::OpenEXR::Channels);

    picojson::value toJSON(const AV::IO::OpenEXR::Options&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value &, AV::IO::OpenEXR::Options&);

} // namespace djv

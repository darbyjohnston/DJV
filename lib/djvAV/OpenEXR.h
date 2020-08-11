// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
                void readTags(const Imf::Header&, Tags&, Core::Math::Rational&);

                //! Write tags to an Imf header.
                //!
                //! \todo Write all the tags that are handled by readTags().
                void writeTags(const Tags&, const Core::Math::Rational& speed, Imf::Header&);

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
                    float       dwaCompressionLevel = 45.F;
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
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Info _readInfo(const std::string& fileName) override;
                    std::shared_ptr<Image::Image> _readImage(const std::string& fileName) override;

                private:
                    struct File;
                    Info _open(const std::string&, File&);

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
                        const std::shared_ptr<Core::TextSystem>&,
                        const std::shared_ptr<Core::ResourceSystem>&,
                        const std::shared_ptr<Core::LogSystem>&);

                protected:
                    Image::Type _getImageType(Image::Type) const override;
                    Image::Layout _getImageLayout() const override;
                    void _write(const std::string& fileName, const std::shared_ptr<Image::Image>&) override;

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
                    static std::shared_ptr<Plugin> create(const std::shared_ptr<Core::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const Core::FileSystem::FileInfo&, const ReadOptions&) const override;
                    std::shared_ptr<IWrite> write(const Core::FileSystem::FileInfo&, const Info&, const WriteOptions&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace OpenEXR
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::OpenEXR::Compression);
    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::OpenEXR::Channels);

    rapidjson::Value toJSON(const AV::IO::OpenEXR::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::OpenEXR::Options&);

} // namespace djv

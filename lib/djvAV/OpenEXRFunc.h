// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OpenEXR.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace OpenEXR
            {
                //! Get a layer name from a list of channel names.
                std::string getLayerName(const std::vector<std::string>&);

                //! Get the channels that aren't in any layer.
                Imf::ChannelList getDefaultLayer(const Imf::ChannelList&);

                //! Find a channel by name.
                const Imf::Channel* find(const Imf::ChannelList&, std::string&);

                //! Get a list of layers from Imf channels.
                std::vector<Layer> getLayers(const Imf::ChannelList&, Channels);

                //! Read the tags from an Imf header.
                void readTags(const Imf::Header&, Image::Tags&, Math::IntRational&);

                //! Write tags to an Imf header.
                //!
                //! \todo Write all the tags that are handled by readTags().
                void writeTags(const Image::Tags&, const Math::IntRational& speed, Imf::Header&);

                //! Convert an Imath box type.
                Math::BBox2i fromImath(const Imath::Box2i&);

                //! Convert to an Imf pixel type.
                Imf::PixelType toImf(Image::DataType);

                //! Convert from an Imf pixel type.
                Image::DataType fromImf(Imf::PixelType);

                //! Convert from an Imf channel.
                Channel fromImf(const std::string& name, const Imf::Channel&);

                DJV_ENUM_HELPERS(Channels);
                DJV_ENUM_HELPERS(Compression);

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

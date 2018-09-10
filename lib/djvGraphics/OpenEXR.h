//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

#include <djvGraphics/ColorProfile.h>
#include <djvGraphics/ImageIO.h>

#include <ImathBox.h>
#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfPixelType.h>

//! \todo Where is this define coming from?
#undef COMPRESSION_NONE

//! \addtogroup plugins
//@{

//! \defgroup djvOpenEXRPlugin djvOpenEXRPlugin
//!
//! This plugin provides support for the Industrial Light and Magic OpenEXR
//! image file format.
//!
//! Requires:
//!
//! - OpenEXR - http://www.openexr.com
//!
//! File extensions: .exr
//!
//! Supported features:
//!
//! - 16-bit float, 32-bit float, Luminance, Luminance Alpha, RGB, RGBA
//! - Image layers
//! - Display and data windows
//! - File compression
//!
//! \todo Add support for writing luminance/chroma images.
//! \todo Add support for OpenEXR aspect ratios.
//! \todo Add better support for tiled images.

//@} // plugins

//! \addtogroup djvOpenEXRPlugin
//@{

//------------------------------------------------------------------------------
//! \struct djvOpenEXR
//!
//! This struct provides OpenEXR utilities.
//------------------------------------------------------------------------------

struct djvOpenEXR
{
    //! The plugin name.
    static const QString staticName;

    //! This struct provides an image channel.
    struct Channel
    {
        Channel() :
            sampling(1)
        {}
        Channel(
            const QString &    name,
            djvPixel::TYPE     type,
            const glm::ivec2 & sampling = glm::ivec2(1, 1));

        QString        name;
        djvPixel::TYPE type;
        glm::ivec2     sampling;
    };

    //! This struct provides an image layer.
    struct Layer
    {
        Layer(
            const QVector<Channel> & channels        = QVector<Channel>(),
            bool                     luminanceChroma = false);

        QString          name;
        QVector<Channel> channels;
        bool             luminanceChroma;
    };

    //! This enumeration provides the color profiles.
    enum COLOR_PROFILE
    {
        COLOR_PROFILE_NONE,
        COLOR_PROFILE_GAMMA,
        COLOR_PROFILE_EXPOSURE,

        COLOR_PROFILE_COUNT
    };

    //! Get the color profile labels.
    static const QStringList & colorProfileLabels();

    //! This enumeration provides the file compression.
    enum COMPRESSION
    {
        COMPRESSION_NONE,
        COMPRESSION_RLE,
        COMPRESSION_ZIPS,
        COMPRESSION_ZIP,
        COMPRESSION_PIZ,
        COMPRESSION_PXR24,
        COMPRESSION_B44,
        COMPRESSION_B44A,
#if OPENEXR_VERSION_HEX >= 0x02020000
        COMPRESSION_DWAA,
        COMPRESSION_DWAB,
#endif // OPENEXR_VERSION_HEX

        COMPRESSION_COUNT
    };

    //! Get the compression labels.
    static const QStringList & compressionLabels();

    //! This enumeration provides the channels.
    enum CHANNELS
    {
        CHANNELS_GROUP_NONE,
        CHANNELS_GROUP_KNOWN,
        CHANNELS_GROUP_ALL,

        CHANNELS_COUNT
    };

    //! Get the channel labels.
    static const QStringList & channelsLabels();

    //! This enumeration provides the image tags.
    enum TAG
    {
        TAG_LONGITUDE,
        TAG_LATITUDE,
        TAG_ALTITUDE,
        TAG_FOCUS,
        TAG_EXPOSURE,
        TAG_APERTURE,
        TAG_ISO_SPEED,
        TAG_CHROMATICITIES,
        TAG_WHITE_LUMINANCE,
        TAG_X_DENSITY,

        TAG_COUNT
    };

    //! Get the image tag labels.
    static const QStringList & tagLabels();

    //! Create a layer name from a list of channel names.
    //!
    //! Example:
    //!
    //! - R, G, B - R,G,B
    //! - normal.X, normal.Y, normal.Z - normal.X,Y,Z
    static QString layerName(const QStringList &);

    //! Find channels that aren't in any layer.
    static Imf::ChannelList defaultLayer(const Imf::ChannelList &);

    //! Find a channel by name.
    static const Imf::Channel * find(const Imf::ChannelList &, QString &);

    //! Build a list of layers from Imf channels.
    static QVector<Layer> layer(const Imf::ChannelList &, CHANNELS);

    //! Load image tags.
    static void loadTags(const Imf::Header &, djvImageIOInfo &);

    //! Save image tags.
    static void saveTags(const djvImageIOInfo &, Imf::Header &);

    //! Convert from an OpenEXR box type.
    static djvBox2i imfToBox(const Imath::Box2i &);

    //! Convert to a OpenEXR pixel type.
    static Imf::PixelType pixelTypeToImf(djvPixel::TYPE);

    //! Convert from an OpenEXR pixel type.
    static djvPixel::TYPE imfToPixelType(Imf::PixelType);

    //! Convert from an OpenEXR channel.
    static Channel imfToChannel(const QString & name, const Imf::Channel &);

    //! This enumeration provides the options.
    enum OPTIONS
    {
        THREADS_ENABLE_OPTION,
        THREAD_COUNT_OPTION,
        INPUT_COLOR_PROFILE_OPTION,
        INPUT_GAMMA_OPTION,
        INPUT_EXPOSURE_OPTION,
        CHANNELS_OPTION,
        COMPRESSION_OPTION,
#if OPENEXR_VERSION_HEX >= 0x02020000
        DWA_COMPRESSION_LEVEL_OPTION,
#endif // OPENEXR_VERSION_HEX

        OPTIONS_COUNT
    };

    //! Get the option labels.
    static const QStringList & optionsLabels();

    //! This struct provides options.
    struct Options
    {
        Options();

        bool                      threadsEnable;
        int                       threadCount;
        djvOpenEXR::COLOR_PROFILE inputColorProfile;
        float                     inputGamma;
        djvColorProfile::Exposure inputExposure;
        djvOpenEXR::CHANNELS      channels;
        djvOpenEXR::COMPRESSION   compression;
#if OPENEXR_VERSION_HEX >= 0x02020000
        float                     dwaCompressionLevel;
#endif // OPENEXR_VERSION_HEX
    };
};

DJV_STRING_OPERATOR(djvOpenEXR::COLOR_PROFILE);
DJV_STRING_OPERATOR(djvOpenEXR::COMPRESSION);
DJV_STRING_OPERATOR(djvOpenEXR::CHANNELS);

bool compare(const QVector<Imf::Channel> &);

//@} // djvOpenEXRPlugin


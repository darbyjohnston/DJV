//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvGraphics/OpenEXR.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>
#include <djvCore/ListUtil.h>
#include <djvCore/Time.h>

#include <ImfFramesPerSecond.h>
#include <ImfStandardAttributes.h>
#include <ImfThreading.h>

#include <QCoreApplication>
#include <QSet>

namespace djv
{
    namespace Graphics
    {
        OpenEXR::Channel::Channel()
        {}

        OpenEXR::Channel::Channel(
            const QString &    name,
            Pixel::TYPE        type,
            const glm::ivec2 & sampling) :
            name(name),
            type(type),
            sampling(sampling)
        {}

        OpenEXR::Layer::Layer(
            const QVector<Channel> & channels,
            bool                     luminanceChroma) :
            channels(channels),
            luminanceChroma(luminanceChroma)
        {
            QStringList names;
            for (int i = 0; i < channels.count(); ++i)
            {
                names += channels[i].name;
            }
            name = layerName(names);
        }

        const QString OpenEXR::staticName = "OpenEXR";

        const QStringList & OpenEXR::colorProfileLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenEXR", "None") <<
                qApp->translate("djv::Graphics::OpenEXR", "Gamma") <<
                qApp->translate("djv::Graphics::OpenEXR", "Exposure");
            DJV_ASSERT(data.count() == COLOR_PROFILE_COUNT);
            return data;
        }

        const QStringList & OpenEXR::compressionLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenEXR", "None") <<
                qApp->translate("djv::Graphics::OpenEXR", "RLE") <<
                qApp->translate("djv::Graphics::OpenEXR", "ZIPS") <<
                qApp->translate("djv::Graphics::OpenEXR", "ZIP") <<
                qApp->translate("djv::Graphics::OpenEXR", "PIZ") <<
                qApp->translate("djv::Graphics::OpenEXR", "PXR24") <<
                qApp->translate("djv::Graphics::OpenEXR", "B44") <<
                qApp->translate("djv::Graphics::OpenEXR", "B44A")
#if OPENEXR_VERSION_HEX >= 0x02020000
                <<
                qApp->translate("djv::Graphics::OpenEXR", "DWAA") <<
                qApp->translate("djv::Graphics::OpenEXR", "DWAB")
#endif // OPENEXR_VERSION_HEX
                ;
            DJV_ASSERT(data.count() == COMPRESSION_COUNT);
            return data;
        }

        const QStringList & OpenEXR::channelsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenEXR", "None") <<
                qApp->translate("djv::Graphics::OpenEXR", "Known") <<
                qApp->translate("djv::Graphics::OpenEXR", "All");
            DJV_ASSERT(data.count() == CHANNELS_COUNT);
            return data;
        }

        const QStringList & OpenEXR::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenEXR", "Longitude") <<
                qApp->translate("djv::Graphics::OpenEXR", "Latitude") <<
                qApp->translate("djv::Graphics::OpenEXR", "Altitude") <<
                qApp->translate("djv::Graphics::OpenEXR", "Focus") <<
                qApp->translate("djv::Graphics::OpenEXR", "Exposure") <<
                qApp->translate("djv::Graphics::OpenEXR", "Aperture") <<
                qApp->translate("djv::Graphics::OpenEXR", "ISO Speed") <<
                qApp->translate("djv::Graphics::OpenEXR", "Chromaticities") <<
                qApp->translate("djv::Graphics::OpenEXR", "White Luminance") <<
                qApp->translate("djv::Graphics::OpenEXR", "XDensity");
            DJV_ASSERT(data.count() == TAG_COUNT);
            return data;
        }

        QString OpenEXR::layerName(const QStringList & in)
        {
            QString out;

            // Split into a prefix and suffix.
            QSet<QString> prefixSet;
            QStringList   suffixes;
            for (int i = 0; i < in.count(); ++i)
            {
                const QString & name = in[i];
                const int index = name.lastIndexOf('.');
                if (index != -1 && index != 0 && index != name.length() - 1)
                {
                    prefixSet += name.mid(0, index);
                    suffixes += name.mid(index + 1, name.length() - index - 1);
                }
                else
                {
                    prefixSet += name;
                }
            }

            // Join pieces.
            QList<QString> list = prefixSet.toList();
            out = QStringList(list).join(",");
            if (suffixes.count())
            {
                out += "." + suffixes.join(",");
            }

            return out;
        }

        Imf::ChannelList OpenEXR::defaultLayer(const Imf::ChannelList & in)
        {
            Imf::ChannelList out;
            for (
                Imf::ChannelList::ConstIterator i = in.begin();
                i != in.end();
                ++i)
            {
                const QString tmp = i.name();
                const int index = tmp.indexOf('.');
                if (index != -1)
                {
                    if (index != 0 || index != tmp.length() - 1)
                    {
                        continue;
                    }
                }
                out.insert(i.name(), i.channel());
            }
            return out;
        }

        const Imf::Channel * OpenEXR::find(
            const Imf::ChannelList & in,
            QString &                channel)
        {
            //DJV_DEBUG("OpenEXR::find");
            //DJV_DEBUG_PRINT("channel = " << channel);
            const QString channelLower = channel.toLower();
            for (
                Imf::ChannelList::ConstIterator i = in.begin();
                i != in.end();
                ++i)
            {
                const QString & inName = i.name();
                const int index = inName.lastIndexOf('.');
                const QString tmp =
                    (index != -1) ?
                    inName.mid(index + 1, inName.length() - index - 1) :
                    inName;
                if (channelLower == tmp.toLower())
                {
                    //DJV_DEBUG_PRINT("found = " << inName);
                    channel = inName;
                    return &i.channel();
                }
            }
            return 0;
        }

        namespace
        {
            QVector<OpenEXR::Layer> _layer(
                const Imf::ChannelList & in,
                OpenEXR::CHANNELS        channels)
            {
                //DJV_DEBUG("_layer");
                QVector<OpenEXR::Layer> out;
                QVector<const Imf::Channel *> reserved;
                if (channels != OpenEXR::CHANNELS_GROUP_NONE)
                {
                    // Look for known channel configurations then convert the remainder.

                    // RGB / RGBA.
                    QString rName = "r";
                    QString gName = "g";
                    QString bName = "b";
                    QString aName = "a";
                    const Imf::Channel * r = OpenEXR::find(in, rName);
                    const Imf::Channel * g = OpenEXR::find(in, gName);
                    const Imf::Channel * b = OpenEXR::find(in, bName);
                    const Imf::Channel * a = OpenEXR::find(in, aName);
                    if (r && g && b && a &&
                        compare(QVector<Imf::Channel>() << *r << *g << *b << *a))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(rName, *r) <<
                            OpenEXR::imfToChannel(gName, *g) <<
                            OpenEXR::imfToChannel(bName, *b) <<
                            OpenEXR::imfToChannel(aName, *a));
                        //DJV_DEBUG_PRINT("rgba = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            r << g << b << a;
                    }
                    else if (
                        r && g && b &&
                        compare(QVector<Imf::Channel>() << *r << *g << *b))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(rName, *r) <<
                            OpenEXR::imfToChannel(gName, *g) <<
                            OpenEXR::imfToChannel(bName, *b));
                        //DJV_DEBUG_PRINT("rgb = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            r << g << b;
                    }

                    // Luminance, XYZ.
                    QString yName = "y";
                    QString ryName = "ry";
                    QString byName = "by";
                    QString xName = "x";
                    QString zName = "z";
                    const Imf::Channel * y = OpenEXR::find(in, yName);
                    const Imf::Channel * ry = OpenEXR::find(in, ryName);
                    const Imf::Channel * by = OpenEXR::find(in, byName);
                    const Imf::Channel * x = OpenEXR::find(in, xName);
                    const Imf::Channel * z = OpenEXR::find(in, zName);
                    if (y && a && compare(QVector<Imf::Channel>() << *y << *a))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(yName, *y) <<
                            OpenEXR::imfToChannel(aName, *a));
                        //DJV_DEBUG_PRINT("ya = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            y << a;
                    }
                    else if (y && ry && by &&
                        1 == y->xSampling &&
                        1 == y->ySampling &&
                        2 == ry->xSampling &&
                        2 == ry->ySampling &&
                        2 == by->xSampling &&
                        2 == by->ySampling)
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(yName, *y) <<
                            OpenEXR::imfToChannel(ryName, *ry) <<
                            OpenEXR::imfToChannel(byName, *by),
                            true);
                        //DJV_DEBUG_PRINT("yc = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            y << ry << by;
                    }
                    else if (
                        x && y && z &&
                        compare(QVector<Imf::Channel>() << *x << *y << *z))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(xName, *x) <<
                            OpenEXR::imfToChannel(yName, *y) <<
                            OpenEXR::imfToChannel(zName, *z));
                        //DJV_DEBUG_PRINT("xyz = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            x << y << z;
                    }
                    else if (
                        x && y &&
                        compare(QVector<Imf::Channel>() << *x << *y))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(xName, *x) <<
                            OpenEXR::imfToChannel(yName, *y));
                        //DJV_DEBUG_PRINT("xy = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            x << y;
                    }
                    else if (x)
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(xName, *x));
                        //DJV_DEBUG_PRINT("x = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            x;
                    }
                    else if (y)
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(yName, *y));
                        //DJV_DEBUG_PRINT("y = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            y;
                    }
                    else if (z)
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(zName, *z));
                        //DJV_DEBUG_PRINT("z = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            z;
                    }

                    // Colored mattes.
                    QString arName = "ar";
                    QString agName = "ag";
                    QString abName = "ab";
                    const Imf::Channel * ar = OpenEXR::find(in, arName);
                    const Imf::Channel * ag = OpenEXR::find(in, agName);
                    const Imf::Channel * ab = OpenEXR::find(in, abName);
                    if (ar && ag && ab &&
                        compare(QVector<Imf::Channel>() << *ar << *ag << *ab))
                    {
                        out += OpenEXR::Layer(QVector<OpenEXR::Channel>() <<
                            OpenEXR::imfToChannel(arName, *ar) <<
                            OpenEXR::imfToChannel(agName, *ag) <<
                            OpenEXR::imfToChannel(abName, *ab));
                        //DJV_DEBUG_PRINT("matte = " << out[out.count() - 1].name);
                        reserved += QVector<const Imf::Channel *>() <<
                            ar << ag << ab;
                    }
                }

                // Convert the remainder.
                //DJV_DEBUG_PRINT("reserved list = " << reserved.count());
                for (
                    Imf::ChannelList::ConstIterator i = in.begin();
                    i != in.end();)
                {
                    QVector<OpenEXR::Channel> list;

                    // Add the first channel.
                    const QString & name = i.name();
                    const Imf::Channel & channel = i.channel();
                    ++i;
                    if (reserved.contains(&channel))
                    {
                        continue;
                    }
                    list += OpenEXR::imfToChannel(name, channel);
                    if (OpenEXR::CHANNELS_GROUP_ALL == channels)
                    {
                        // Group as many additional channels as possible.
                        for (
                            ;
                            i != in.end() &&
                            i.channel() == channel &&
                            reserved.contains(&i.channel());
                            ++i)
                        {
                            list += OpenEXR::imfToChannel(i.name(), i.channel());
                        }
                    }

                    // Add the layer.
                    out += OpenEXR::Layer(list);
                    //DJV_DEBUG_PRINT("layer = " << out[out.count() - 1].name);
                }

                return out;
            }

        } // namespace

        QVector<OpenEXR::Layer> OpenEXR::layer(
            const Imf::ChannelList & in,
            CHANNELS                 channels)
        {
            //DJV_DEBUG("layer");

            QVector<Layer> out;

            // Default layer.
            out += _layer(defaultLayer(in), channels);

            // Additional layers.
            std::set<std::string> layers;
            in.layers(layers);

            for (
                std::set<std::string>::const_iterator i = layers.begin();
                i != layers.end();
                ++i)
            {
                Imf::ChannelList list;
                Imf::ChannelList::ConstIterator f, l;
                in.channelsInLayer(*i, f, l);
                for (Imf::ChannelList::ConstIterator j = f; j != l; ++j)
                {
                    list.insert(j.name(), j.channel());
                }
                out += _layer(list, channels);
            }
            //for (int i = 0; i < out.count(); ++i)
            //    DJV_DEBUG_PRINT("layer[" << i << "] = " << out[i].name);

            return out;
        }

        void OpenEXR::loadTags(const Imf::Header & in, ImageIOInfo & out)
        {
            const QStringList & openexrTags = tagLabels();
            const QStringList & tags = ImageTags::tagLabels();
            if (hasOwner(in))
            {
                out.tags[tags[ImageTags::CREATOR]] = ownerAttribute(in).value().c_str();
            }
            if (hasComments(in))
            {
                out.tags[tags[ImageTags::DESCRIPTION]] =
                    commentsAttribute(in).value().c_str();
            }
            if (hasCapDate(in))
            {
                out.tags[tags[ImageTags::TIME]] = capDateAttribute(in).value().c_str();
            }
            if (hasUtcOffset(in))
                out.tags[tags[ImageTags::UTC_OFFSET]] =
                QString::number(utcOffsetAttribute(in).value());
            if (hasLongitude(in))
                out.tags[openexrTags[TAG_LONGITUDE]] =
                QString::number(longitudeAttribute(in).value());
            if (hasLatitude(in))
                out.tags[openexrTags[TAG_LATITUDE]] =
                QString::number(latitudeAttribute(in).value());
            if (hasAltitude(in))
                out.tags[openexrTags[TAG_ALTITUDE]] =
                QString::number(altitudeAttribute(in).value());
            if (hasFocus(in))
                out.tags[openexrTags[TAG_FOCUS]] =
                QString::number(focusAttribute(in).value());
            if (hasExpTime(in))
                out.tags[openexrTags[TAG_EXPOSURE]] =
                QString::number(expTimeAttribute(in).value());
            if (hasAperture(in))
                out.tags[openexrTags[TAG_APERTURE]] =
                QString::number(apertureAttribute(in).value());
            if (hasIsoSpeed(in))
                out.tags[openexrTags[TAG_ISO_SPEED]] =
                QString::number(isoSpeedAttribute(in).value());
            if (hasChromaticities(in))
            {
                const Imf::Chromaticities data = chromaticitiesAttribute(in).value();
                out.tags[openexrTags[TAG_CHROMATICITIES]] = (QStringList() <<
                    QString::number(data.red.x) <<
                    QString::number(data.red.y) <<
                    QString::number(data.green.x) <<
                    QString::number(data.green.y) <<
                    QString::number(data.blue.x) <<
                    QString::number(data.blue.y) <<
                    QString::number(data.white.x) <<
                    QString::number(data.white.y)).join(" ");
            }
            if (hasWhiteLuminance(in))
                out.tags[openexrTags[TAG_WHITE_LUMINANCE]] =
                QString::number(whiteLuminanceAttribute(in).value());
            if (hasXDensity(in))
                out.tags[openexrTags[TAG_X_DENSITY]] =
                QString::number(xDensityAttribute(in).value());
            if (hasKeyCode(in))
            {
                const Imf::KeyCode data = keyCodeAttribute(in).value();
                out.tags[tags[ImageTags::KEYCODE]] =
                    Core::Time::keycodeToString(
                        data.filmMfcCode(),
                        data.filmType(),
                        data.prefix(),
                        data.count(),
                        data.perfOffset());
            }
            if (hasTimeCode(in))
                out.tags[tags[ImageTags::TIMECODE]] = Core::Time::timecodeToString(
                    timeCodeAttribute(in).value().timeAndFlags());
            if (hasFramesPerSecond(in))
            {
                const Imf::Rational data = framesPerSecondAttribute(in).value();
                out.sequence.speed = Core::Speed(data.n, data.d);
            }
        }

        void OpenEXR::saveTags(const ImageIOInfo & in, Imf::Header & out)
        {
            const QStringList & openexrTags = tagLabels();
            const QStringList & tags = ImageTags::tagLabels();
            QString tmp = in.tags[tags[ImageTags::CREATOR]];
            if (tmp.length())
            {
                addOwner(out, tmp.toLatin1().data());
            }
            tmp = in.tags[tags[ImageTags::DESCRIPTION]];
            if (tmp.length())
            {
                addComments(out, tmp.toLatin1().data());
            }
            tmp = in.tags[tags[ImageTags::TIME]];
            if (tmp.length())
            {
                addCapDate(out, tmp.toLatin1().data());
            }
            tmp = in.tags[tags[ImageTags::UTC_OFFSET]];
            if (tmp.length())
            {
                addUtcOffset(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_LONGITUDE]];
            if (tmp.length())
            {
                addLongitude(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_LATITUDE]];
            if (tmp.length())
            {
                addLatitude(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_ALTITUDE]];
            if (tmp.length())
            {
                addAltitude(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_FOCUS]];
            if (tmp.length())
            {
                addFocus(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_EXPOSURE]];
            if (tmp.length())
            {
                addExpTime(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_APERTURE]];
            if (tmp.length())
            {
                addAperture(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_ISO_SPEED]];
            if (tmp.length())
            {
                addIsoSpeed(out, tmp.toFloat());
            }
            tmp = in.tags[openexrTags[TAG_CHROMATICITIES]];
            if (tmp.length())
            {
                const QStringList list = tmp.split(' ', QString::SkipEmptyParts);
                if (8 == list.count())
                {
                    addChromaticities(
                        out,
                        Imf::Chromaticities(
                            Imath::V2f(list[0].toFloat(), list[1].toFloat()),
                            Imath::V2f(list[2].toFloat(), list[3].toFloat()),
                            Imath::V2f(list[4].toFloat(), list[5].toFloat()),
                            Imath::V2f(list[6].toFloat(), list[7].toFloat())));
                }
            }
            tmp = in.tags[openexrTags[TAG_WHITE_LUMINANCE]];
            if (tmp.length())
                addWhiteLuminance(out, tmp.toFloat());
            tmp = in.tags[openexrTags[TAG_X_DENSITY]];
            if (tmp.length())
            {
                addXDensity(out, tmp.toFloat());
            }
            tmp = in.tags[tags[ImageTags::KEYCODE]];
            if (tmp.length())
            {
                int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
                Core::Time::stringToKeycode(tmp, id, type, prefix, count, offset);
                addKeyCode(out, Imf::KeyCode(id, type, prefix, count, offset));
            }
            tmp = in.tags[tags[ImageTags::TIMECODE]];
            if (tmp.length())
            {
                addTimeCode(out, Core::Time::stringToTimecode(tmp));
            }
            addFramesPerSecond(
                out,
                Imf::Rational(in.sequence.speed.scale(), in.sequence.speed.duration()));
        }

        Core::Box2i OpenEXR::imfToBox(const Imath::Box2i & in)
        {
            return Core::Box2i(
                glm::ivec2(in.min.x, in.min.y),
                glm::ivec2(in.max.x, in.max.y) - glm::ivec2(in.min.x, in.min.y) + 1);
        }

        Imf::PixelType OpenEXR::pixelTypeToImf(Pixel::TYPE in)
        {
            switch (in)
            {
            case Pixel::U8:
            case Pixel::U10:
            case Pixel::U16:
            case Pixel::F16: return Imf::HALF;
            default: break;
            }
            return Imf::FLOAT;
        }

        Pixel::TYPE OpenEXR::imfToPixelType(Imf::PixelType in)
        {
            switch (in)
            {
            case Imf::HALF: return Pixel::F16;
            default: break;
            }
            return Pixel::F32;
        }

        OpenEXR::Channel OpenEXR::imfToChannel(
            const QString &      name,
            const Imf::Channel & channel)
        {
            return Channel(
                name,
                imfToPixelType(channel.type),
                glm::ivec2(channel.xSampling, channel.ySampling));
        }

        const QStringList & OpenEXR::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::Graphics::OpenEXR", "Threads Enable") <<
                qApp->translate("djv::Graphics::OpenEXR", "Thread Count") <<
                qApp->translate("djv::Graphics::OpenEXR", "Input Color Profile") <<
                qApp->translate("djv::Graphics::OpenEXR", "Input Gamma") <<
                qApp->translate("djv::Graphics::OpenEXR", "Input Exposure") <<
                qApp->translate("djv::Graphics::OpenEXR", "Channels") <<
                qApp->translate("djv::Graphics::OpenEXR", "Compression")
#if OPENEXR_VERSION_HEX >= 0x02020000
                << qApp->translate("djv::Graphics::OpenEXR", "DWA Compression Level");
#endif // OPENEXR_VERSION_HEX
            ;
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace Graphics

    _DJV_STRING_OPERATOR_LABEL(Graphics::OpenEXR::COLOR_PROFILE, Graphics::OpenEXR::colorProfileLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::OpenEXR::COMPRESSION, Graphics::OpenEXR::compressionLabels());
    _DJV_STRING_OPERATOR_LABEL(Graphics::OpenEXR::CHANNELS, Graphics::OpenEXR::channelsLabels());

    bool compare(const QVector<Imf::Channel> & in)
    {
        for (int i = 1; i < in.count(); ++i)
        {
            if (!(in[0] == in[i]))
            {
                return false;
            }
        }
        return true;
    }

} // namespace djv

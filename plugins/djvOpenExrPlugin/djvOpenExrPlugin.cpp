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

//! \file djvOpenExrPlugin.cpp

#include <djvOpenExrPlugin.h>

#include <djvOpenExrLoad.h>
#include <djvOpenExrSave.h>
#include <djvOpenExrWidget.h>

#include <djvAssert.h>
#include <djvError.h>
#include <djvListUtil.h>
#include <djvTime.h>

#include <ImfFramesPerSecond.h>
#include <ImfStandardAttributes.h>
#include <ImfThreading.h>

#include <QSet>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvOpenExrPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvOpenExrPlugin::Channel
//------------------------------------------------------------------------------

djvOpenExrPlugin::Channel::Channel(
    const QString &     name,
    djvPixel::TYPE      type,
    const djvVector2i & sampling) :
    name    (name),
    type    (type),
    sampling(sampling)
{}

djvOpenExrPlugin::Layer::Layer(
    const QVector<Channel> & channels,
    bool                     luminanceChroma) :
    channels       (channels),
    luminanceChroma(luminanceChroma)
{
    QStringList names;

    for (int i = 0; i < channels.count(); ++i)
    {
        names += channels[i].name;
    }

    name = layerName(names);
}

//------------------------------------------------------------------------------
// djvOpenExrPlugin::Options
//------------------------------------------------------------------------------

djvOpenExrPlugin::Options::Options() :
    threadsEnable      (true),
    threadCount        (4),
    inputColorProfile  (djvOpenExrPlugin::COLOR_PROFILE_GAMMA),
    inputGamma         (2.2),
    channels           (djvOpenExrPlugin::CHANNELS_GROUP_KNOWN),
    compression        (djvOpenExrPlugin::COMPRESSION_NONE)
#if OPENEXR_VERSION_HEX >= 0x02020000
    ,
    dwaCompressionLevel(45.0)
#endif // OPENEXR_VERSION_HEX
{}

//------------------------------------------------------------------------------
// djvOpenExrPlugin
//------------------------------------------------------------------------------

const QString djvOpenExrPlugin::staticName = "OpenEXR";

const QStringList & djvOpenExrPlugin::colorProfileLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "Gamma" <<
        "Exposure";

    DJV_ASSERT(data.count() == COLOR_PROFILE_COUNT);

    return data;
}

const QStringList & djvOpenExrPlugin::compressionLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "RLE" <<
        "ZIPS" <<
        "ZIP" <<
        "PIZ" <<
        "PXR24" <<
        "B44" <<
        "B44A"        
#if OPENEXR_VERSION_HEX >= 0x02020000
        <<
        "DWAA" <<
        "DWAB"
#endif // OPENEXR_VERSION_HEX
        ;

    DJV_ASSERT(data.count() == COMPRESSION_COUNT);

    return data;
}

const QStringList & djvOpenExrPlugin::channelsLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "Known" <<
        "All";

    DJV_ASSERT(data.count() == CHANNELS_COUNT);

    return data;
}

const QStringList & djvOpenExrPlugin::tagLabels()
{
    static const QStringList data = QStringList() <<
        "Longitude" <<
        "Latitude" <<
        "Altitude" <<
        "Focus" <<
        "Exposure" <<
        "Aperture" <<
        "ISO Speed" <<
        "Chromaticities" <<
        "White Luminance" <<
        "XDensity";

    DJV_ASSERT(data.count() == TAG_COUNT);

    return data;
}

QString djvOpenExrPlugin::layerName(const QStringList & in)
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

Imf::ChannelList djvOpenExrPlugin::defaultLayer(const Imf::ChannelList & in)
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

const Imf::Channel * djvOpenExrPlugin::find(
    const Imf::ChannelList & in,
    QString &                channel)
{
    //DJV_DEBUG("djvOpenExrPlugin::find");
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

QVector<djvOpenExrPlugin::Layer> _layer(
    const Imf::ChannelList &   in,
    djvOpenExrPlugin::CHANNELS channels)
{
    //DJV_DEBUG("_layer");

    QVector<djvOpenExrPlugin::Layer> out;

    QVector<const Imf::Channel *> reserved;

    if (channels != djvOpenExrPlugin::CHANNELS_GROUP_NONE)
    {
        // Look for known channel configurations then convert the remainder.

        // RGB / RGBA.

        QString rName = "r";
        QString gName = "g";
        QString bName = "b";
        QString aName = "a";

        const Imf::Channel * r = djvOpenExrPlugin::find (in, rName);
        const Imf::Channel * g = djvOpenExrPlugin::find (in, gName);
        const Imf::Channel * b = djvOpenExrPlugin::find (in, bName);
        const Imf::Channel * a = djvOpenExrPlugin::find (in, aName);

        if (r && g && b && a &&
            compare(QVector<Imf::Channel>() << *r << *g << *b << *a))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(rName, *r) <<
                djvOpenExrPlugin::imfToChannel(gName, *g) <<
                djvOpenExrPlugin::imfToChannel(bName, *b) <<
                djvOpenExrPlugin::imfToChannel(aName, *a));

            //DJV_DEBUG_PRINT("rgba = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                r << g << b << a;
        }
        else if (
            r && g && b &&
            compare(QVector<Imf::Channel>() << *r << *g << *b))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(rName, *r) <<
                djvOpenExrPlugin::imfToChannel(gName, *g) <<
                djvOpenExrPlugin::imfToChannel(bName, *b));

            //DJV_DEBUG_PRINT("rgb = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                r << g << b;
        }

        // Luminance, XYZ.

        QString yName  = "y";
        QString ryName = "ry";
        QString byName = "by";
        QString xName  = "x";
        QString zName  = "z";

        const Imf::Channel * y  = djvOpenExrPlugin::find (in, yName);
        const Imf::Channel * ry = djvOpenExrPlugin::find (in, ryName);
        const Imf::Channel * by = djvOpenExrPlugin::find (in, byName);
        const Imf::Channel * x  = djvOpenExrPlugin::find (in, xName);
        const Imf::Channel * z  = djvOpenExrPlugin::find (in, zName);

        if (y && a && compare(QVector<Imf::Channel>() << *y << *a))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(yName, *y) <<
                djvOpenExrPlugin::imfToChannel(aName, *a));

            //DJV_DEBUG_PRINT("ya = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                y << a;
        }
        else if (y && ry && by &&
            1 ==  y->xSampling &&
            1 ==  y->ySampling &&
            2 == ry->xSampling &&
            2 == ry->ySampling &&
            2 == by->xSampling &&
            2 == by->ySampling)
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(yName, *y) <<
                djvOpenExrPlugin::imfToChannel(ryName, *ry) <<
                djvOpenExrPlugin::imfToChannel(byName, *by),
                true);

            //DJV_DEBUG_PRINT("yc = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                y << ry << by;
        }
        else if (
            x && y && z &&
            compare(QVector<Imf::Channel>() << *x << *y << *z))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(xName, *x) <<
                djvOpenExrPlugin::imfToChannel(yName, *y) <<
                djvOpenExrPlugin::imfToChannel(zName, *z));

            //DJV_DEBUG_PRINT("xyz = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                x << y << z;
        }
        else if (
            x && y &&
            compare(QVector<Imf::Channel>() << *x << *y))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(xName, *x) <<
                djvOpenExrPlugin::imfToChannel(yName, *y));

            //DJV_DEBUG_PRINT("xy = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                x << y;
        }
        else if (x)
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(xName, *x));

            //DJV_DEBUG_PRINT("x = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                x;
        }
        else if (y)
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(yName, *y));

            //DJV_DEBUG_PRINT("y = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                y;
        }
        else if (z)
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(zName, *z));

            //DJV_DEBUG_PRINT("z = " << out[out.count() - 1].name);

            reserved += QVector<const Imf::Channel *>() <<
                z;
        }

        // Colored mattes.

        QString arName = "ar";
        QString agName = "ag";
        QString abName = "ab";

        const Imf::Channel * ar = djvOpenExrPlugin::find (in, arName);
        const Imf::Channel * ag = djvOpenExrPlugin::find (in, agName);
        const Imf::Channel * ab = djvOpenExrPlugin::find (in, abName);

        if (ar && ag && ab &&
            compare(QVector<Imf::Channel>() << *ar << *ag << *ab))
        {
            out += djvOpenExrPlugin::Layer(QVector<djvOpenExrPlugin::Channel>() <<
                djvOpenExrPlugin::imfToChannel(arName, *ar) <<
                djvOpenExrPlugin::imfToChannel(agName, *ag) <<
                djvOpenExrPlugin::imfToChannel(abName, *ab));

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
        QVector<djvOpenExrPlugin::Channel> list;

        // Add the first channel.

        const QString & name = i.name();
        const Imf::Channel & channel = i.channel();
        ++i;

        if (reserved.contains(&channel))
        {
            continue;
        }

        list += djvOpenExrPlugin::imfToChannel(name, channel);

        if (djvOpenExrPlugin::CHANNELS_GROUP_ALL == channels)
        {
            // Group as many additional channels as possible.

            for (
                ;
                i != in.end() &&
                i.channel() == channel &&
                reserved.contains(&i.channel());
                ++i)
            {
                list += djvOpenExrPlugin::imfToChannel(i.name(), i.channel());
            }
        }

        // Add the layer.

        out += djvOpenExrPlugin::Layer(list);

        //DJV_DEBUG_PRINT("layer = " << out[out.count() - 1].name);
    }

    return out;
}

} // namespace

QVector<djvOpenExrPlugin::Layer> djvOpenExrPlugin::layer(
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

void djvOpenExrPlugin::loadTags(const Imf::Header & in, djvImageIoInfo & out)
{
    const QStringList & openexrTags = tagLabels();
    const QStringList & tags = djvImageTags::tagLabels();

    if (hasOwner(in))
    {
        out.tags[tags[djvImageTags::CREATOR]] = ownerAttribute(in).value().c_str();
    }

    if (hasComments(in))
    {
        out.tags[tags[djvImageTags::DESCRIPTION]] =
            commentsAttribute(in).value().c_str();
    }

    if (hasCapDate(in))
    {
        out.tags[tags[djvImageTags::TIME]] = capDateAttribute(in).value().c_str();
    }

    if (hasUtcOffset(in))
        out.tags[tags[djvImageTags::UTC_OFFSET]] =
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

        out.tags[tags[djvImageTags::KEYCODE]] =
            djvTime::keycodeToString(
                data.filmMfcCode(),
                data.filmType(),
                data.prefix(),
                data.count(),
                data.perfOffset());
    }

    if (hasTimeCode(in))
        out.tags[tags[djvImageTags::TIMECODE]] = djvTime::timecodeToString(
                timeCodeAttribute(in).value().timeAndFlags());

    if (hasFramesPerSecond(in))
    {
        const Imf::Rational data = framesPerSecondAttribute(in).value();

        out.sequence.speed = djvSpeed(data.n, data.d);
    }
}

void djvOpenExrPlugin::saveTags(const djvImageIoInfo & in, Imf::Header & out)
{
    const QStringList & openexrTags = tagLabels();
    const QStringList & tags = djvImageTags::tagLabels();

    QString tmp = in.tags[tags[djvImageTags::CREATOR]];

    if (tmp.length())
    {
        addOwner(out, tmp.toLatin1().data());
    }

    tmp = in.tags[tags[djvImageTags::DESCRIPTION]];

    if (tmp.length())
    {
        addComments(out, tmp.toLatin1().data());
    }

    tmp = in.tags[tags[djvImageTags::TIME]];

    if (tmp.length())
    {
        addCapDate(out, tmp.toLatin1().data());
    }

    tmp = in.tags[tags[djvImageTags::UTC_OFFSET]];

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

    tmp = in.tags[tags[djvImageTags::KEYCODE]];

    if (tmp.length())
    {
        int id = 0, type = 0, prefix = 0, count = 0, offset = 0;
        djvTime::stringToKeycode(tmp, id, type, prefix, count, offset);

        addKeyCode(out, Imf::KeyCode(id, type, prefix, count, offset));
    }

    tmp = in.tags[tags[djvImageTags::TIMECODE]];

    if (tmp.length())
    {
        addTimeCode(out, djvTime::stringToTimecode(tmp));
    }

    addFramesPerSecond(
        out,
        Imf::Rational(in.sequence.speed.scale(), in.sequence.speed.duration()));
}

djvBox2i djvOpenExrPlugin::imfToBox(const Imath::Box2i & in)
{
    return djvBox2i(
        djvVector2i(in.min.x, in.min.y),
        djvVector2i(in.max.x, in.max.y) - djvVector2i(in.min.x, in.min.y) + 1);
}

Imf::PixelType djvOpenExrPlugin::pixelTypeToImf(djvPixel::TYPE in)
{
    switch (in)
    {
        case djvPixel::U8:
        case djvPixel::U10:
        case djvPixel::U16:
        case djvPixel::F16: return Imf::HALF;

        default: break;
    }

    return Imf::FLOAT;
}

djvPixel::TYPE djvOpenExrPlugin::imfToPixelType(Imf::PixelType in)
{
    switch (in)
    {
        case Imf::HALF: return djvPixel::F16;

        default: break;
    }

    return djvPixel::F32;
}

djvOpenExrPlugin::Channel djvOpenExrPlugin::imfToChannel(
    const QString &      name,
    const Imf::Channel & channel)
{
    return Channel(
        name,
        imfToPixelType(channel.type),
        djvVector2i(channel.xSampling, channel.ySampling));
}

const QStringList & djvOpenExrPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Threads Enable" <<
        "Thread Count" <<
        "Input Color Profile" <<
        "Input Gamma" <<
        "Input Exposure" <<
        "Channels" <<
        "Compression"
#if OPENEXR_VERSION_HEX >= 0x02020000
        <<
        "DWA Compression Level";
#endif // OPENEXR_VERSION_HEX
        ;

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

namespace
{

int refCount = 0;

} // namespace

void djvOpenExrPlugin::initPlugin() throw (djvError)
{
    ++refCount;

    if (refCount > 1)
        return;

    //DJV_DEBUG("djvOpenExrPlugin::initPlugin");
    //DJV_DEBUG_PRINT("ref count = " << refCount);
    
    threadsUpdate();
}

void djvOpenExrPlugin::releasePlugin()
{
    --refCount;

    if (refCount)
        return;

    //DJV_DEBUG("djvOpenExrPlugin::releasePlugin");
    //DJV_DEBUG_PRINT("ref count = " << refCount);

#if defined(DJV_WINDOWS)

    //! \todo Is is still necessary to reset the global thread cound on
    //! Windows?

    //Imf::setGlobalThreadCount(0);

#endif // DJV_WINDOWS
}

djvPlugin * djvOpenExrPlugin::copyPlugin() const
{
    djvOpenExrPlugin * plugin = new djvOpenExrPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvOpenExrPlugin::pluginName() const
{
    return staticName;
}

QStringList djvOpenExrPlugin::extensions() const
{
    return QStringList() << ".exr";
}

QStringList djvOpenExrPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.threadsEnable;
    }
    else if (0 == in.compare(options()[THREAD_COUNT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.threadCount;
    }
    else if (0 == in.compare(options()[INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputColorProfile;
    }
    else if (0 == in.compare(options()[INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputGamma;
    }
    else if (0 == in.compare(options()[INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputExposure;
    }
    else if (0 == in.compare(options()[CHANNELS_OPTION], Qt::CaseInsensitive))
    {
        out << _options.channels;
    }
    else if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }
#if OPENEXR_VERSION_HEX >= 0x02020000
    else if (0 == in.compare(options()[DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
    {
        out << _options.dwaCompressionLevel;
    }
#endif // OPENEXR_VERSION_HEX

    return out;
}

bool djvOpenExrPlugin::setOption(const QString & in, QStringList & data)
{
    //DJV_DEBUG("djvOpenExrPlugin::setOption");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("data = " << data);
    
    try
    {
        if (0 == in.compare(options()[THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
        {
            bool enable = false;
            
            data >> enable;
            
            if (enable != _options.threadsEnable)
            {
                _options.threadsEnable = enable;
                
                threadsUpdate();

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[THREAD_COUNT_OPTION], Qt::CaseInsensitive))
        {
            int threadCount = 0;
            
            data >> threadCount;
            
            if (threadCount != _options.threadCount)
            {
                _options.threadCount = threadCount;

                threadsUpdate();

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
        {
            COLOR_PROFILE colorProfile = static_cast<COLOR_PROFILE>(0);
            
            data >> colorProfile;
            
            if (colorProfile != _options.inputColorProfile)
            {
                _options.inputColorProfile = colorProfile;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
        {
            double gamma = 0.0;
            
            data >> gamma;
            
            if (gamma != _options.inputGamma)
            {
                _options.inputGamma = gamma;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
        {
            djvColorProfile::Exposure exposure;
            
            data >> exposure;
            
            if (exposure != _options.inputExposure)
            {
                _options.inputExposure = exposure;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[CHANNELS_OPTION], Qt::CaseInsensitive))
        {
            CHANNELS channels = static_cast<CHANNELS>(0);
            
            data >> channels;
            
            if (channels != _options.channels)
            {
                _options.channels = channels;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            COMPRESSION compression = static_cast<COMPRESSION>(0);
            
            data >> compression;
            
            if (compression != _options.compression)
            {
                _options.compression = compression;
                
                Q_EMIT optionChanged(in);
            }
        }
#if OPENEXR_VERSION_HEX >= 0x02020000
        else if (0 == in.compare(options()[DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
        {
            double compressionLevel = 0.0;
            
            data >> compressionLevel;
            
            if (! djvMath::fuzzyCompare(
                compressionLevel,
                _options.dwaCompressionLevel))
            {
                _options.dwaCompressionLevel = compressionLevel;
                
                Q_EMIT optionChanged(in);
            }
        }
#endif // OPENEXR_VERSION_HEX
    }
    catch (const QString &)
    {
        return false;
    }

    return true;
}

QStringList djvOpenExrPlugin::options() const
{
    return optionsLabels();
}

void djvOpenExrPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-exr_threads_enable" == arg)
            {
                in >> _options.threadsEnable;
            }
            else if ("-exr_thread_count" == arg)
            {
                in >> _options.threadCount;
            }
            else if ("-exr_input_color_profile" == arg)
            {
                in >> _options.inputColorProfile;
            }
            else if ("-exr_input_gamma" == arg)
            {
                in >> _options.inputGamma;
            }
            else if ("-exr_input_exposure" == arg)
            {
                in >> _options.inputExposure;
            }
            else if ("-exr_channels" == arg)
            {
                in >> _options.channels;
            }
            else if ("-exr_compression" == arg)
            {
                in >> _options.compression;
            }
#if OPENEXR_VERSION_HEX >= 0x02020000
            else if ("-exr_dwa_compression_level" == arg)
            {
                in >> _options.dwaCompressionLevel;
            }
#endif // OPENEXR_VERSION_HEX
            else
            {
                tmp << arg;
            }
        }
    }
    catch (const QString &)
    {
        throw arg;
    }

    in = tmp;
}

QString djvOpenExrPlugin::commandLineHelp() const
{
    return QString(
"\n"
"OpenEXR Options\n"
"\n"
"    -exr_threads_enable (value)\n"
"        Set whether threading is enabled. Default = %1.\n"
"    -exr_thread_count (value)\n"
"        Set the maximum number of threads to use. Default = %2.\n"
"    -exr_input_color_profile (value)\n"
"        Set the color profile used when loading OpenEXR images. Options = "
"%3. Default = %4.\n"
"    -exr_input_gamma (value)\n"
"        Set the gamma values used when loading OpenEXR images. Default = "
"%5.\n"
"    -exr_input_exposure (value) (defog) (knee low) (knee high)\n"
"        Set the exposure values used when loading OpenEXR images. Default = "
"%6.\n"
"    -exr_channels (value)\n"
"        Set how channels are grouped when loading OpenEXR images. Options = "
"%7. Default = %8.\n"
"    -exr_compression (value)\n"
"        Set the file compression used when saving OpenEXR images. Options = "
"%9. Default = %10.\n"
#if OPENEXR_VERSION_HEX >= 0x02020000
"    -exr_dwa_compression_level (value)\n"
"        Set the DWA compression level used when saving OpenEXR images. "
"Default = %11.\n"
#endif // OPENEXR_VERSION_HEX
    ).
    arg(djvStringUtil::label(_options.threadsEnable).join(", ")).
    arg(djvStringUtil::label(_options.threadCount).join(", ")).
    arg(djvOpenExrPlugin::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.inputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.inputGamma).join(", ")).
    arg(djvStringUtil::label(_options.inputExposure).join(", ")).
    arg(djvOpenExrPlugin::channelsLabels().join(", ")).
    arg(djvStringUtil::label(_options.channels).join(", ")).
    arg(djvOpenExrPlugin::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "))
#if OPENEXR_VERSION_HEX >= 0x02020000
    .
    arg(_options.dwaCompressionLevel)
#endif // OPENEXR_VERSION_HEX
    ;
}

djvImageLoad * djvOpenExrPlugin::createLoad() const
{
    return new djvOpenExrLoad(_options);
}

djvImageSave * djvOpenExrPlugin::createSave() const
{
    return new djvOpenExrSave(_options);
}

djvAbstractPrefsWidget * djvOpenExrPlugin::createWidget()
{
    return new djvOpenExrWidget(this);
}

void djvOpenExrPlugin::threadsUpdate()
{
    //DJV_DEBUG("djvOpenExrPlugin::threadsUpdate");
    //DJV_DEBUG_PRINT("this = " << uint64_t(this));
    //DJV_DEBUG_PRINT("threads = " << _options.threadsEnable);
    //DJV_DEBUG_PRINT("thread count = " << _options.threadsCount);

    Imf::setGlobalThreadCount(
        _options.threadsEnable ? _options.threadCount : 0);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvOpenExrPlugin::COLOR_PROFILE,
    djvOpenExrPlugin::colorProfileLabels())
_DJV_STRING_OPERATOR_LABEL(djvOpenExrPlugin::COMPRESSION,
    djvOpenExrPlugin::compressionLabels())
_DJV_STRING_OPERATOR_LABEL(djvOpenExrPlugin::CHANNELS,
    djvOpenExrPlugin::channelsLabels())

bool compare(const QVector<Imf::Channel> & in)
{
    for (int i = 1; i < in.count(); ++i)
    {
        if (! (in[0] == in[i]))
        {
            return false;
        }
    }

    return true;
}

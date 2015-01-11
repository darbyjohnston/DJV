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

//! \file djvLutPlugin.cpp

#include <djvLutPlugin.h>

#include <djvLutLoad.h>
#include <djvLutSave.h>
#include <djvLutWidget.h>

#include <djvAssert.h>
#include <djvColor.h>
#include <djvError.h>
#include <djvFileIoUtil.h>

#include <QRegExp>

#include <stdio.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvLutPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvLutPlugin::Options
//------------------------------------------------------------------------------

djvLutPlugin::Options::Options() :
    type(TYPE_AUTO)
{}

//------------------------------------------------------------------------------
// djvLutPlugin
//---------------------------------------------------------3---------------------

const QString djvLutPlugin::staticName = "LUT";

const QStringList djvLutPlugin::staticExtensions = QStringList() <<
    ".lut" <<
    ".1dl";

const QStringList & djvLutPlugin::formatLabels()
{
    static const QStringList data = QStringList() <<
        "Inferno" <<
        "Kodak";

    DJV_ASSERT(data.count() == FORMAT_COUNT);

    return data;
}

const QStringList & djvLutPlugin::typeLabels()
{
    static const QStringList data = QStringList() <<
        "Auto" <<
        "U8" <<
        "U10" <<
        "U16";

    DJV_ASSERT(data.count() == TYPE_COUNT);

    return data;
}

namespace
{

int _bitDepth(djvFileIo & io) throw (djvError)
{
    int out = 0;

    const quint64 pos = io.pos();

    char tmp[djvStringUtil::cStringLength] = "";

    while (1)
    {
        try
        {
            djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
        }
        catch (const djvError &)
        {
            break;
        }

        out = djvMath::max(QString(tmp).toInt(), out);
    }

    io.setPos(pos);

    if (out <= djvPixel::u8Max)
    {
        return 8;
    }
    else if (out <= djvPixel::u10Max)
    {
        return 10;
    }

    return 16;
}

} // namespace

void djvLutPlugin::infernoOpen(djvFileIo & io, djvPixelDataInfo & info, TYPE type)
    throw (djvError)
{
    //DJV_DEBUG("infernoOpen");

    // Header.

    char tmp[djvStringUtil::cStringLength] = "";
    djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);

    //DJV_DEBUG_PRINT("magic = " << tmp);

    if (QString(tmp) != "LUT:")
    {
        throw djvError(
            djvLutPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNRECOGNIZED].
            arg(io.fileName()));
    }

    djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
    const int channels = QString(tmp).toInt();

    djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
    const int size = QString(tmp).toInt();

    //DJV_DEBUG_PRINT("size = " << size);
    //DJV_DEBUG_PRINT("channels = " << channels);

    // Information.

    info.size = djvVector2i(size, 1);

    int bitDepth = 0;

    switch (type)
    {
        case TYPE_AUTO: bitDepth = _bitDepth(io); break;
        case TYPE_U8:   bitDepth = 8;             break;
        case TYPE_U10:  bitDepth = 10;            break;
        case TYPE_U16:  bitDepth = 16;            break;

        default: break;
    }

    //DJV_DEBUG_PRINT("bit depth = " << bitDepth);

    if (! djvPixel::pixel(channels, bitDepth, djvPixel::INTEGER, info.pixel))
    {
        throw djvError(
            djvLutPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }
}

void djvLutPlugin::infernoOpen(djvFileIo & io, const djvPixelDataInfo & info)
    throw (djvError)
{
    char tmp[djvStringUtil::cStringLength] = "";
    int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "LUT: %d %d\n\n",
        djvPixel::channels(info.pixel), info.size.x);

    io.set(tmp, size);
}

void djvLutPlugin::kodakOpen(djvFileIo & io, djvPixelDataInfo & info, TYPE type)
    throw (djvError)
{
    //DJV_DEBUG("djvLutPlugin::kodakOpen");

    // Header.

    const quint64 pos = io.pos();
    QString header;
    qint8 c = 0;
    bool comment = false;

    while (1)
    {
        try
        {
            io.get8(&c);
        }
        catch (const djvError &)
        {
            break;
        }

        if ('#' == c)
        {
            comment = true;
        }

        else if ('\n' == c)
        {
            if (comment)
            {
                comment = false;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (! comment)
            {
                header += c;
            }
        }
    }

    //DJV_DEBUG_PRINT("header = " << header);

    const QStringList split = header.split(
        QRegExp("\\s"),
        QString::SkipEmptyParts);

    //DJV_DEBUG_PRINT("split = " << split.join("|"));

    const int channels = split.count();

    //DJV_DEBUG_PRINT("channels = " << channels);

    int size = 1;

    while (1)
    {
        try
        {
            io.get8(&c);
        }
        catch (const djvError &)
        {
            break;
        }

        if ('\n' == c)
        {
            ++size;
        }
    }

    io.setPos(pos);

    //DJV_DEBUG_PRINT("size = " << size);

    // Information.

    info.size = djvVector2i(size, 1);

    int bitDepth = 0;

    switch (type)
    {
        case TYPE_AUTO: bitDepth = _bitDepth(io); break;
        case TYPE_U8:   bitDepth = 8;             break;
        case TYPE_U10:  bitDepth = 10;            break;
        case TYPE_U16:  bitDepth = 16;            break;

        default: break;
    }

    //DJV_DEBUG_PRINT("bit depth = " << bitDepth);

    if (! djvPixel::pixel(channels, bitDepth, djvPixel::INTEGER, info.pixel))
    {
        throw djvError(
            djvLutPlugin::staticName,
            djvImageIo::errorLabels()[djvImageIo::ERROR_UNSUPPORTED].
            arg(io.fileName()));
    }
}

void djvLutPlugin::kodakOpen(djvFileIo &, const djvPixelDataInfo &)
    throw (djvError)
{}

void djvLutPlugin::infernoLoad(djvFileIo & io, djvImage & out)
    throw (djvError)
{
    //DJV_DEBUG("djvLutPlugin::infernoLoad");

    QVector<djvColor> color(out.w());

    for (int x = 0; x < out.w(); ++x)
    {
        color[x].setPixel(out.pixel());
    }

    for (int c = 0; c < djvPixel::channels(out.pixel()); ++c)
        for (int x = 0; x < out.w(); ++x)
        {
            char tmp [djvStringUtil::cStringLength] = "";
            djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
            const int v = QString(tmp).toInt();

            switch (djvPixel::type(out.pixel()))
            {
                case djvPixel::U8:  color[x].setU8(v, c); break;
                case djvPixel::U10: color[x].setU10(v, c); break;
                case djvPixel::U16: color[x].setU16(v, c); break;

                default: break;
            }
        }

    for (int x = 0; x < out.w(); ++x)
    {
        //DJV_DEBUG_PRINT(x << " = " << color[x]);
        
        djvMemory::copy(
            color[x].data(),
            out.data(x, 0),
            djvPixel::byteCount(out.pixel()));
    }
}

void djvLutPlugin::kodakLoad(djvFileIo & io, djvImage & out) throw (djvError)
{
    //DJV_DEBUG("djvLutPlugin::kodakLoad");

    for (int x = 0; x < out.w(); ++x)
    {
        djvColor color(out.pixel());

        for (int c = 0; c < djvPixel::channels(out.pixel()); ++c)
        {
            char tmp[djvStringUtil::cStringLength] = "";
            djvFileIoUtil::word(io, tmp, djvStringUtil::cStringLength);
            const int v = QString(tmp).toInt();

            switch (djvPixel::type(out.pixel()))
            {
                case djvPixel::U8:  color.setU8(v, c); break;
                case djvPixel::U10: color.setU10(v, c); break;
                case djvPixel::U16: color.setU16(v, c); break;

                default: break;
            }
        }

        //DJV_DEBUG_PRINT(x << " = " << color);

        djvMemory::copy(
            color.data(),
            out.data(x, 0),
            djvPixel::byteCount(out.pixel()));
    }
}

void djvLutPlugin::infernoSave(djvFileIo & io, const djvPixelData * out)
    throw (djvError)
{
    QVector<djvColor> color(out->w());

    for (int x = 0; x < out->w(); ++x)
    {
        color[x].setPixel(out->pixel());
        
        djvMemory::copy(
            out->data(x, 0),
            color[x].data(),
            djvPixel::byteCount(out->pixel()));
    }

    for (int c = 0; c < djvPixel::channels(out->pixel()); ++c)
    {
        for (int x = 0; x < out->w(); ++x)
        {
            int v = 0;

            switch (djvPixel::type(out->pixel()))
            {
                case djvPixel::U8:  v = color[x].u8(c);  break;
                case djvPixel::U10: v = color[x].u10(c); break;
                case djvPixel::U16: v = color[x].u16(c); break;

                default: break;
            }

            char tmp[djvStringUtil::cStringLength] = "";

            const int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "%9d\n", v);

            io.set(tmp, size);
        }
    }
}

void djvLutPlugin::kodakSave(djvFileIo & io, const djvPixelData * out)
    throw (djvError)
{
    for (int x = 0; x < out->w(); ++x)
    {
        djvColor color(out->pixel());
        
        djvMemory::copy(
            out->data(x, 0),
            color.data(),
            djvPixel::byteCount(out->pixel()));

        for (int c = 0; c < djvPixel::channels(out->pixel()); ++c)
        {
            int v = 0;

            switch (djvPixel::type(out->pixel()))
            {
                case djvPixel::U8:  v = color.u8(c); break;
                case djvPixel::U10: v = color.u10(c); break;
                case djvPixel::U16: v = color.u16(c); break;

                default: break;
            }

            char tmp[djvStringUtil::cStringLength] = "";
            int size = SNPRINTF(tmp, djvStringUtil::cStringLength, "%6d", v);
            io.set(tmp, size);
        }

        io.set8('\n');
    }
}

const QStringList & djvLutPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Type";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

void djvLutPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-lut_type" == arg)
            {
                in >> _options.type;
            }
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

QString djvLutPlugin::commandLineHelp() const
{
    return QString(
"\n"
"LUT Options\n"
"\n"
"    -lut_type (value)\n"
"        Set the pixel type used when loading LUTs. Options = %1. Default = "
"%2.\n").
    arg(djvLutPlugin::typeLabels().join(", ")).
    arg(djvStringUtil::label(_options.type).join(", "));
}

djvPlugin * djvLutPlugin::copyPlugin() const
{
    djvLutPlugin * plugin = new djvLutPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvLutPlugin::pluginName() const
{
    return staticName;
}

QStringList djvLutPlugin::extensions() const
{
    return staticExtensions;
}

QStringList djvLutPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[TYPE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.type;
    }

    return out;
}

bool djvLutPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[TYPE_OPTION], Qt::CaseInsensitive))
        {
            TYPE type = static_cast<TYPE>(0);

            data >> type;

            if (type != _options.type)
            {
                _options.type = type;

                Q_EMIT optionChanged(in);
            }
        }
    }
    catch (const QString &)
    {
        return false;
    }

    return true;
}

QStringList djvLutPlugin::options() const
{
    return optionsLabels();
}

djvImageLoad * djvLutPlugin::createLoad() const
{
    return new djvLutLoad(_options);
}

djvImageSave * djvLutPlugin::createSave() const
{
    return new djvLutSave(_options);
}

djvAbstractPrefsWidget * djvLutPlugin::createWidget()
{
    return new djvLutWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvLutPlugin::FORMAT, djvLutPlugin::formatLabels())
_DJV_STRING_OPERATOR_LABEL(djvLutPlugin::TYPE, djvLutPlugin::typeLabels())

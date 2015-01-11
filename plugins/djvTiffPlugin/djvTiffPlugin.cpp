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

//! \file djvTiffPlugin.cpp

#include <djvTiffPlugin.h>

#include <djvTiffLoad.h>
#include <djvTiffSave.h>
#include <djvTiffWidget.h>

#include <djvAssert.h>
#include <djvError.h>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvTiffPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvTiffPlugin::Options
//------------------------------------------------------------------------------

djvTiffPlugin::Options::Options() :
    compression(djvTiffPlugin::_COMPRESSION_NONE)
{}

//------------------------------------------------------------------------------
// djvTiffPlugin
//------------------------------------------------------------------------------

const QString djvTiffPlugin::staticName = "TIFF";

const QStringList & djvTiffPlugin::compressionLabels()
{
    static const QStringList data = QStringList() <<
        "None" <<
        "RLE" <<
        "LZW";

    DJV_ASSERT(data.count() == COMPRESSION_COUNT);

    return data;
}

void djvTiffPlugin::paletteLoad(
    quint8 *  in,
    int       size,
    int       bytes,
    quint16 * red,
    quint16 * green,
    quint16 * blue)
{
    switch (bytes)
    {
        case 1:
        {
            const quint8 * inP = in + size - 1;
            quint8 * outP = in + (size - 1) * 3;

            for (int x = 0; x < size; ++x, outP -= 3)
            {
                const quint8 index = *inP--;
                outP[0] = static_cast<quint8>(red[index]);
                outP[1] = static_cast<quint8>(green[index]);
                outP[2] = static_cast<quint8>(blue[index]);
            }
        }
        break;

        case 2:
        {
            const quint16 * inP =
                reinterpret_cast<const quint16 *>(in) + size - 1;
            
            quint16 * outP =
                reinterpret_cast<quint16 *>(in) + (size - 1) * 3;

            for (int x = 0; x < size; ++x, outP -= 3)
            {
                const quint16 index = *inP--;
                outP[0] = red[index];
                outP[1] = green[index];
                outP[2] = blue[index];
            }
        }
        break;
    }
}

const QStringList & djvTiffPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Compression";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

void djvTiffPlugin::initPlugin() throw (djvError)
{
    TIFFSetErrorHandler(0);
    TIFFSetWarningHandler(0);
}

djvPlugin * djvTiffPlugin::copyPlugin() const
{
    djvTiffPlugin * plugin = new djvTiffPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvTiffPlugin::pluginName() const
{
    return staticName;
}

QStringList djvTiffPlugin::extensions() const
{
    return QStringList() <<
        ".tiff" <<
        ".tif";
}

QStringList djvTiffPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }

    return out;
}

bool djvTiffPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            COMPRESSION compression = static_cast<COMPRESSION>(0);
            
            data >> compression;
            
            if (compression != _options.compression)
            {
                _options.compression = compression;
                
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

QStringList djvTiffPlugin::options() const
{
    return optionsLabels();
}

void djvTiffPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-tiff_compression" == arg)
            {
                in >> _options.compression;
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

QString djvTiffPlugin::commandLineHelp() const
{
    return QString(
"\n"
"TIFF Options\n"
"\n"
"    -tiff_compression (value)\n"
"        Set the file compression used when saving TIFF images. Options = %1. "
"Default = %2.\n").
    arg(djvTiffPlugin::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "));
}

djvImageLoad * djvTiffPlugin::createLoad() const
{
    return new djvTiffLoad;
}

djvImageSave * djvTiffPlugin::createSave() const
{
    return new djvTiffSave(_options);
}

djvAbstractPrefsWidget * djvTiffPlugin::createWidget()
{
    return new djvTiffWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvTiffPlugin::COMPRESSION,
    djvTiffPlugin::compressionLabels())

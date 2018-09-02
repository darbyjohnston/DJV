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

#include <djvTiffPlugin.h>

#include <djvTiffLoad.h>
#include <djvTiffSave.h>

#include <djvAssert.h>
#include <djvError.h>

#include <QCoreApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIoEntry(djvCoreContext * context)
{
    return new djvTiffPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvTiffPlugin
//------------------------------------------------------------------------------

djvTiffPlugin::djvTiffPlugin(djvCoreContext * context) :
    djvImageIo(context)
{}

void djvTiffPlugin::initPlugin() throw (djvError)
{
    TIFFSetErrorHandler(0);
    TIFFSetWarningHandler(0);
}

QString djvTiffPlugin::pluginName() const
{
    return djvTiff::staticName;
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
    if (0 == in.compare(options()[djvTiff::COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }
    return out;
}

bool djvTiffPlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[djvTiff::COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            djvTiff::COMPRESSION compression = static_cast<djvTiff::COMPRESSION>(0);
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
    return djvTiff::optionsLabels();
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
            if (qApp->translate("djvTiffPlugin", "-tiff_compression") == arg)
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
    return qApp->translate("djvTiffPlugin",
"\n"
"TIFF Options\n"
"\n"
"    -tiff_compression (value)\n"
"        Set the file compression used when saving TIFF images. Options = %1. "
"Default = %2.\n").
    arg(djvTiff::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "));
}

djvImageLoad * djvTiffPlugin::createLoad() const
{
    return new djvTiffLoad(imageContext());
}

djvImageSave * djvTiffPlugin::createSave() const
{
    return new djvTiffSave(_options, imageContext());
}

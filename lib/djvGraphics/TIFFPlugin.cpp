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

#include <djvGraphics/TIFFPlugin.h>

#include <djvGraphics/TIFFLoad.h>
#include <djvGraphics/TIFFSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        TIFFPlugin::TIFFPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIO(context)
        {}

        void TIFFPlugin::initPlugin()
        {
            TIFFSetErrorHandler(0);
            TIFFSetWarningHandler(0);
        }

        QString TIFFPlugin::pluginName() const
        {
            return TIFF::staticName;
        }

        QStringList TIFFPlugin::extensions() const
        {
            return QStringList() <<
                ".tiff" <<
                ".tif";
        }

        QStringList TIFFPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[TIFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
            {
                out << _options.compression;
            }
            return out;
        }

        bool TIFFPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[TIFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
                {
                    TIFF::COMPRESSION compression = static_cast<TIFF::COMPRESSION>(0);
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

        QStringList TIFFPlugin::options() const
        {
            return TIFF::optionsLabels();
        }

        void TIFFPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::Graphics::TIFFPlugin", "-tiff_compression") == arg)
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

        QString TIFFPlugin::commandLineHelp() const
        {
            QStringList compressionLabel;
            compressionLabel << _options.compression;
            return qApp->translate("djv::Graphics::TIFFPlugin",
                "\n"
                "TIFF Options\n"
                "\n"
                "    -tiff_compression (value)\n"
                "        Set the file compression used when saving TIFF images: %1. "
                "Default = %2.\n").
                arg(TIFF::compressionLabels().join(", ")).
                arg(compressionLabel.join(", "));
        }

        ImageLoad * TIFFPlugin::createLoad() const
        {
            return new TIFFLoad(context());
        }

        ImageSave * TIFFPlugin::createSave() const
        {
            return new TIFFSave(_options, context());
        }

    } // namespace Graphics
} // namespace djv

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

#include <djvGraphics/JPEGPlugin.h>

#include <djvGraphics/JPEGLoad.h>
#include <djvGraphics/JPEGSave.h>

#include <djvCore/Assert.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        JPEGPlugin::JPEGPlugin(Core::CoreContext * context) :
            ImageIO(context)
        {}

        QString JPEGPlugin::pluginName() const
        {
            return JPEG::staticName;
        }

        QStringList JPEGPlugin::extensions() const
        {
            return QStringList() <<
                ".jpeg" <<
                ".jpg" <<
                ".jfif";
        }

        QStringList JPEGPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[JPEG::QUALITY_OPTION], Qt::CaseInsensitive))
            {
                out << _options.quality;
            }
            return out;
        }

        bool JPEGPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[JPEG::QUALITY_OPTION], Qt::CaseInsensitive))
                {
                    int quality = 0;
                    data >> quality;
                    if (quality != _options.quality)
                    {
                        _options.quality = quality;
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

        QStringList JPEGPlugin::options() const
        {
            return JPEG::optionsLabels();
        }

        void JPEGPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::Graphics::JPEGPlugin", "-jpeg_quality") == arg)
                    {
                        in >> _options.quality;
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

        QString JPEGPlugin::commandLineHelp() const
        {
            QStringList qualityLabel;
            qualityLabel << _options.quality;
            return qApp->translate("djv::Graphics::JPEGPlugin",
                "\n"
                "JPEG Options\n"
                "\n"
                "    -jpeg_quality (value)\n"
                "        Set the quality used when saving JPEG images. Default = %1.\n").
                arg(qualityLabel.join(", "));
        }

        ImageLoad * JPEGPlugin::createLoad() const
        {
            return new JPEGLoad(context());
        }

        ImageSave * JPEGPlugin::createSave() const
        {
            return new JPEGSave(_options, context());
        }

    } // namespace Graphics
} // namespace djv

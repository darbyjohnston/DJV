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

#include <djvGraphics/LUTPlugin.h>

#include <djvGraphics/LUTLoad.h>
#include <djvGraphics/LUTSave.h>

#include <djvCore/Assert.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        LUTPlugin::LUTPlugin(Core::CoreContext * context) :
            ImageIO(context)
        {}

        void LUTPlugin::commandLine(QStringList & in) throw (QString)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::Graphics::LUTPlugin", "-lut_type") == arg)
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

        QString LUTPlugin::commandLineHelp() const
        {
            QStringList typeLabel;
            typeLabel << _options.type;
            return qApp->translate("djv::Graphics::LUTPlugin",
                "\n"
                "LUT Options\n"
                "\n"
                "    -lut_type (value)\n"
                "        Set the pixel type used when loading LUTs. Options = %1. Default = "
                "%2.\n").
                arg(LUT::typeLabels().join(", ")).
                arg(typeLabel.join(", "));
        }

        QString LUTPlugin::pluginName() const
        {
            return LUT::staticName;
        }

        QStringList LUTPlugin::extensions() const
        {
            return LUT::staticExtensions;
        }

        QStringList LUTPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[LUT::TYPE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.type;
            }
            return out;
        }

        bool LUTPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[LUT::TYPE_OPTION], Qt::CaseInsensitive))
                {
                    LUT::TYPE type = static_cast<LUT::TYPE>(0);
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

        QStringList LUTPlugin::options() const
        {
            return LUT::optionsLabels();
        }

        ImageLoad * LUTPlugin::createLoad() const
        {
            return new LUTLoad(_options, context());
        }

        ImageSave * LUTPlugin::createSave() const
        {
            return new LUTSave(_options, context());
        }

    } // namespace Graphics
} // namespace djv

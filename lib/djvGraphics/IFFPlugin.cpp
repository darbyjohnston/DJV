//------------------------------------------------------------------------------
// Copyright (c) 2008-2009 Mikael Sundell
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

#include <djvGraphics/IFFPlugin.h>

#include <djvGraphics/IFFLoad.h>
#include <djvGraphics/IFFSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace Graphics
    {
        IFFPlugin::IFFPlugin(const QPointer<Core::CoreContext> & context) :
            ImageIO(context)
        {}

        QString IFFPlugin::pluginName() const
        {
            return IFF::staticName;
        }

        QStringList IFFPlugin::extensions() const
        {
            return QStringList() <<
                ".iff" <<
                ".z";
        }

        QStringList IFFPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[IFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
            {
                out << _options.compression;
            }
            return out;
        }

        bool IFFPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[IFF::COMPRESSION_OPTION], Qt::CaseInsensitive))
                {
                    IFF::COMPRESSION compression = static_cast<IFF::COMPRESSION>(0);
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

        QStringList IFFPlugin::options() const
        {
            return IFF::optionsLabels();
        }

        void IFFPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::Graphics:IFFPlugin", "-iff_compression") == arg)
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

        QString IFFPlugin::commandLineHelp() const
        {
            QStringList compressionLabel;
            compressionLabel << _options.compression;
            return qApp->translate("djv::Graphics::IFFPlugin",
                "\n"
                "IFF Options\n"
                "\n"
                "    -iff_compression (value)\n"
                "        Set the file compression used when saving IFF images: %1. "
                "Default = %2.\n").
                arg(IFF::compressionLabels().join(", ")).
                arg(compressionLabel.join(", "));
        }

        ImageLoad * IFFPlugin::createLoad() const
        {
            return new IFFLoad(context());
        }

        ImageSave * IFFPlugin::createSave() const
        {
            return new IFFSave(_options, context());
        }

    } // namespace Graphics
} // namespace djv

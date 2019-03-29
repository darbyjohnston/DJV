//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/SGIPlugin.h>

#include <djvAV/SGILoad.h>
#include <djvAV/SGISave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        SGIPlugin::SGIPlugin(const QPointer<Core::CoreContext> & context) :
            IOPlugin(context)
        {}

        QString SGIPlugin::pluginName() const
        {
            return SGI::staticName;
        }

        QStringList SGIPlugin::extensions() const
        {
            return QStringList() <<
                ".sgi" <<
                ".rgba" <<
                ".rgb" <<
                ".bw";
        }

        QStringList SGIPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[SGI::COMPRESSION_OPTION], Qt::CaseInsensitive))
            {
                out << _options.compression;
            }
            return out;
        }

        bool SGIPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[SGI::COMPRESSION_OPTION], Qt::CaseInsensitive))
                {
                    SGI::COMPRESSION compression = static_cast<SGI::COMPRESSION>(0);
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

        QStringList SGIPlugin::options() const
        {
            return SGI::optionsLabels();
        }

        void SGIPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::AV::SGIPlugin", "-sgi_compression") == arg)
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

        QString SGIPlugin::commandLineHelp() const
        {
            QStringList compressionLabel;
            compressionLabel << _options.compression;
            return qApp->translate("djv::AV::SGIPlugin",
                "\n"
                "SGI Options\n"
                "\n"
                "    -sgi_compression (value)\n"
                "        Set the file compression used when saving SGI images: %1. "
                "Default = %2.\n").
                arg(SGI::compressionLabels().join(", ")).
                arg(compressionLabel.join(", "));
        }

        std::unique_ptr<Load> SGIPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<Load>(new SGILoad(fileInfo, context()));
        }

        std::unique_ptr<Save> SGIPlugin::createSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            return std::unique_ptr<Save>(new SGISave(fileInfo, ioInfo, _options, context()));
        }

    } // namespace AV
} // namespace djv

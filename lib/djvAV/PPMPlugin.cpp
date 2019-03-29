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

#include <djvAV/PPMPlugin.h>

#include <djvAV/PPMLoad.h>
#include <djvAV/PPMSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>
#include <djvCore/FileIOUtil.h>
#include <djvCore/Math.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        PPMPlugin::PPMPlugin(const QPointer<Core::CoreContext> & context) :
            IOPlugin(context)
        {}

        QString PPMPlugin::pluginName() const
        {
            return PPM::staticName;
        }

        QStringList PPMPlugin::extensions() const
        {
            return QStringList() <<
                ".ppm" <<
                ".pnm" <<
                ".pgm" <<
                ".pbm";
        }

        QStringList PPMPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[PPM::TYPE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.type;
            }
            else if (0 == in.compare(options()[PPM::DATA_OPTION], Qt::CaseInsensitive))
            {
                out << _options.data;
            }
            return out;
        }

        bool PPMPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[PPM::TYPE_OPTION], Qt::CaseInsensitive))
                {
                    PPM::TYPE type = static_cast<PPM::TYPE>(0);
                    data >> type;
                    if (type != _options.type)
                    {
                        _options.type = type;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[PPM::DATA_OPTION], Qt::CaseInsensitive))
                {
                    PPM::DATA tmp = static_cast<PPM::DATA>(0);
                    data >> tmp;
                    if (tmp != _options.data)
                    {
                        _options.data = tmp;
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

        QStringList PPMPlugin::options() const
        {
            return PPM::optionsLabels();
        }

        void PPMPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::AV::PPMPlugin", "-ppm_type") == arg)
                    {
                        in >> _options.type;
                    }
                    else if (qApp->translate("djv::AV::PPMPlugin", "-ppm_data") == arg)
                    {
                        in >> _options.data;
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

        QString PPMPlugin::commandLineHelp() const
        {
            QStringList typeLabel;
            typeLabel << _options.type;
            QStringList dataLabel;
            dataLabel << _options.type;
            return qApp->translate("djv::AV::PPMPlugin",
                "\n"
                "PPM Options\n"
                "\n"
                "    -ppm_type (value)\n"
                "        Set the file type used when saving PPM images: %1. "
                "Default = %2.\n"
                "    -ppm_data (value)\n"
                "        Set the data type used when saving PPM images: %3. "
                "Default = %4.\n").
                arg(PPM::typeLabels().join(", ")).
                arg(typeLabel.join(", ")).
                arg(PPM::dataLabels().join(", ")).
                arg(dataLabel.join(", "));
        }

        std::unique_ptr<Load> PPMPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<Load>(new PPMLoad(fileInfo, context()));
        }

        std::unique_ptr<Save> PPMPlugin::createSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            return std::unique_ptr<Save>(new PPMSave(fileInfo, ioInfo, _options, context()));
        }

    } // namespace AV
} // namespace djv

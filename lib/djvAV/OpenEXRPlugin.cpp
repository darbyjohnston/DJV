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

#include <djvAV/OpenEXRPlugin.h>

#include <djvAV/OpenEXRLoad.h>
#include <djvAV/OpenEXRSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>
#include <djvCore/Error.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        OpenEXRPlugin::OpenEXRPlugin(const QPointer<Core::CoreContext> & context) :
            IOPlugin(context)
        {}

        namespace
        {
            int refCount = 0;

        } // namespace

        void OpenEXRPlugin::initPlugin()
        {
            ++refCount;
            if (refCount > 1)
                return;
            //DJV_DEBUG("OpenEXRPlugin::initPlugin");
            //DJV_DEBUG_PRINT("ref count = " << refCount);
            threadsUpdate();
        }

        void OpenEXRPlugin::releasePlugin()
        {
            --refCount;
            if (refCount)
                return;
            //DJV_DEBUG("OpenEXRPlugin::releasePlugin");
            //DJV_DEBUG_PRINT("ref count = " << refCount);
#if defined(DJV_WINDOWS)
    //! \todo Is is still necessary to reset the global thread cound on
    //! Windows?
    //Imf::setGlobalThreadCount(0);
#endif // DJV_WINDOWS
        }

        QString OpenEXRPlugin::pluginName() const
        {
            return OpenEXR::staticName;
        }

        QStringList OpenEXRPlugin::extensions() const
        {
            return QStringList() << ".exr";
        }

        QStringList OpenEXRPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[OpenEXR::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.threadsEnable;
            }
            else if (0 == in.compare(options()[OpenEXR::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
            {
                out << _options.threadCount;
            }
            else if (0 == in.compare(options()[OpenEXR::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputColorProfile;
            }
            else if (0 == in.compare(options()[OpenEXR::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputGamma;
            }
            else if (0 == in.compare(options()[OpenEXR::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.inputExposure;
            }
            else if (0 == in.compare(options()[OpenEXR::CHANNELS_OPTION], Qt::CaseInsensitive))
            {
                out << _options.channels;
            }
            else if (0 == in.compare(options()[OpenEXR::COMPRESSION_OPTION], Qt::CaseInsensitive))
            {
                out << _options.compression;
            }
#if OPENEXR_VERSION_HEX >= 0x02020000
            else if (0 == in.compare(options()[OpenEXR::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
            {
                out << _options.dwaCompressionLevel;
            }
#endif // OPENEXR_VERSION_HEX
            return out;
        }

        bool OpenEXRPlugin::setOption(const QString & in, QStringList & data)
        {
            //DJV_DEBUG("OpenEXRPlugin::setOption");
            //DJV_DEBUG_PRINT("in = " << in);
            //DJV_DEBUG_PRINT("data = " << data);
            try
            {
                if (0 == in.compare(options()[OpenEXR::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
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
                else if (0 == in.compare(options()[OpenEXR::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
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
                else if (0 == in.compare(options()[OpenEXR::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
                {
                    OpenEXR::COLOR_PROFILE colorProfile = static_cast<OpenEXR::COLOR_PROFILE>(0);
                    data >> colorProfile;
                    if (colorProfile != _options.inputColorProfile)
                    {
                        _options.inputColorProfile = colorProfile;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[OpenEXR::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
                {
                    float gamma = 0.f;
                    data >> gamma;
                    if (gamma != _options.inputGamma)
                    {
                        _options.inputGamma = gamma;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[OpenEXR::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
                {
                    ColorProfile::Exposure exposure;
                    data >> exposure;
                    if (exposure != _options.inputExposure)
                    {
                        _options.inputExposure = exposure;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[OpenEXR::CHANNELS_OPTION], Qt::CaseInsensitive))
                {
                    OpenEXR::CHANNELS channels = static_cast<OpenEXR::CHANNELS>(0);
                    data >> channels;
                    if (channels != _options.channels)
                    {
                        _options.channels = channels;
                        Q_EMIT optionChanged(in);
                    }
                }
                else if (0 == in.compare(options()[OpenEXR::COMPRESSION_OPTION], Qt::CaseInsensitive))
                {
                    OpenEXR::COMPRESSION compression = static_cast<OpenEXR::COMPRESSION>(0);
                    data >> compression;
                    if (compression != _options.compression)
                    {
                        _options.compression = compression;
                        Q_EMIT optionChanged(in);
                    }
                }
#if OPENEXR_VERSION_HEX >= 0x02020000
                else if (0 == in.compare(options()[OpenEXR::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
                {
                    float compressionLevel = 0.f;
                    data >> compressionLevel;
                    if (!Core::Math::fuzzyCompare(
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

        QStringList OpenEXRPlugin::options() const
        {
            return OpenEXR::optionsLabels();
        }

        void OpenEXRPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_threads_enable") == arg)
                    {
                        in >> _options.threadsEnable;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_thread_count") == arg)
                    {
                        in >> _options.threadCount;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_input_color_profile") == arg)
                    {
                        in >> _options.inputColorProfile;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_input_gamma") == arg)
                    {
                        in >> _options.inputGamma;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_input_exposure") == arg)
                    {
                        in >> _options.inputExposure;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_channels") == arg)
                    {
                        in >> _options.channels;
                    }
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_compression") == arg)
                    {
                        in >> _options.compression;
                    }
#if OPENEXR_VERSION_HEX >= 0x02020000
                    else if (
                        qApp->translate("djv::AV::OpenEXRPlugin", "-exr_dwa_compression_level") == arg)
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

        QString OpenEXRPlugin::commandLineHelp() const
        {
            QStringList threadsEnableLabel;
            threadsEnableLabel << _options.threadsEnable;
            QStringList threadCountLabel;
            threadCountLabel << _options.threadCount;
            QStringList inputColorProfileLabel;
            inputColorProfileLabel << _options.inputColorProfile;
            QStringList inputGammaLabel;
            inputGammaLabel << _options.inputGamma;
            QStringList inputExposureLabel;
            inputExposureLabel << _options.inputExposure;
            QStringList channelsLabel;
            channelsLabel << _options.channels;
            QStringList compressionLabel;
            compressionLabel << _options.compression;
            return qApp->translate("djv::AV::OpenEXRPlugin",
                "\n"
                "OpenEXR Options\n"
                "\n"
                "    -exr_threads_enable (value)\n"
                "        Set whether threading is enabled. Default = %1.\n"
                "    -exr_thread_count (value)\n"
                "        Set the maximum number of threads to use. Default = %2.\n"
                "    -exr_input_color_profile (value)\n"
                "        Set the color profile used when loading OpenEXR images: "
                "%3. Default = %4.\n"
                "    -exr_input_gamma (value)\n"
                "        Set the gamma values used when loading OpenEXR images. Default = "
                "%5.\n"
                "    -exr_input_exposure (value) (defog) (knee low) (knee high)\n"
                "        Set the exposure values used when loading OpenEXR images. Default = "
                "%6.\n"
                "    -exr_channels (value)\n"
                "        Set how channels are grouped when loading OpenEXR images: "
                "%7. Default = %8.\n"
                "    -exr_compression (value)\n"
                "        Set the file compression used when saving OpenEXR images: "
                "%9. Default = %10.\n"
#if OPENEXR_VERSION_HEX >= 0x02020000
                "    -exr_dwa_compression_level (value)\n"
                "        Set the DWA compression level used when saving OpenEXR images. "
                "Default = %11.\n"
#endif // OPENEXR_VERSION_HEX
            ).
                arg(threadsEnableLabel.join(", ")).
                arg(threadCountLabel.join(", ")).
                arg(OpenEXR::colorProfileLabels().join(", ")).
                arg(inputColorProfileLabel.join(", ")).
                arg(inputGammaLabel.join(", ")).
                arg(inputExposureLabel.join(", ")).
                arg(OpenEXR::channelsLabels().join(", ")).
                arg(channelsLabel.join(", ")).
                arg(OpenEXR::compressionLabels().join(", ")).
                arg(compressionLabel.join(", "))
#if OPENEXR_VERSION_HEX >= 0x02020000
                .
                arg(_options.dwaCompressionLevel)
#endif // OPENEXR_VERSION_HEX
                ;
        }

        std::unique_ptr<Load> OpenEXRPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<Load>(new OpenEXRLoad(fileInfo, _options, context()));
        }

        std::unique_ptr<Save> OpenEXRPlugin::createSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            return std::unique_ptr<Save>(new OpenEXRSave(fileInfo, ioInfo, _options, context()));
        }

        void OpenEXRPlugin::threadsUpdate()
        {
            //DJV_DEBUG("OpenEXRPlugin::threadsUpdate");
            //DJV_DEBUG_PRINT("this = " << uint64_t(this));
            //DJV_DEBUG_PRINT("threads = " << _options.threadsEnable);
            //DJV_DEBUG_PRINT("thread count = " << _options.threadsCount);
            Imf::setGlobalThreadCount(_options.threadsEnable ? _options.threadCount : 0);
        }

    } // namespace AV
} // namespace djv


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

//! \file djvOpenExrPlugin.cpp

#include <djvOpenExrPlugin.h>

#include <djvOpenExrLoad.h>
#include <djvOpenExrSave.h>

#include <djvAssert.h>
#include <djvError.h>

#include <QCoreApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIoEntry(djvCoreContext * context)
{
    return new djvOpenExrPlugin(context);
}

} // extern "C"

//------------------------------------------------------------------------------
// djvOpenExrPlugin
//------------------------------------------------------------------------------

djvOpenExrPlugin::djvOpenExrPlugin(djvCoreContext * context) :
    djvImageIo(context)
{}

namespace
{

int refCount = 0;

} // namespace

void djvOpenExrPlugin::initPlugin() throw (djvError)
{
    ++refCount;

    if (refCount > 1)
        return;

    //DJV_DEBUG("djvOpenExrPlugin::initPlugin");
    //DJV_DEBUG_PRINT("ref count = " << refCount);
    
    threadsUpdate();
}

void djvOpenExrPlugin::releasePlugin()
{
    --refCount;

    if (refCount)
        return;

    //DJV_DEBUG("djvOpenExrPlugin::releasePlugin");
    //DJV_DEBUG_PRINT("ref count = " << refCount);

#if defined(DJV_WINDOWS)

    //! \todo Is is still necessary to reset the global thread cound on
    //! Windows?

    //Imf::setGlobalThreadCount(0);

#endif // DJV_WINDOWS
}

djvPlugin * djvOpenExrPlugin::copyPlugin() const
{
    djvOpenExrPlugin * plugin = new djvOpenExrPlugin(context());
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvOpenExrPlugin::pluginName() const
{
    return djvOpenExr::staticName;
}

QStringList djvOpenExrPlugin::extensions() const
{
    return QStringList() << ".exr";
}

QStringList djvOpenExrPlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[djvOpenExr::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.threadsEnable;
    }
    else if (0 == in.compare(options()[djvOpenExr::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
    {
        out << _options.threadCount;
    }
    else if (0 == in.compare(options()[djvOpenExr::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputColorProfile;
    }
    else if (0 == in.compare(options()[djvOpenExr::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputGamma;
    }
    else if (0 == in.compare(options()[djvOpenExr::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
    {
        out << _options.inputExposure;
    }
    else if (0 == in.compare(options()[djvOpenExr::CHANNELS_OPTION], Qt::CaseInsensitive))
    {
        out << _options.channels;
    }
    else if (0 == in.compare(options()[djvOpenExr::COMPRESSION_OPTION], Qt::CaseInsensitive))
    {
        out << _options.compression;
    }
#if OPENEXR_VERSION_HEX >= 0x02020000
    else if (0 == in.compare(options()[djvOpenExr::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
    {
        out << _options.dwaCompressionLevel;
    }
#endif // OPENEXR_VERSION_HEX

    return out;
}

bool djvOpenExrPlugin::setOption(const QString & in, QStringList & data)
{
    //DJV_DEBUG("djvOpenExrPlugin::setOption");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("data = " << data);
    
    try
    {
        if (0 == in.compare(options()[djvOpenExr::THREADS_ENABLE_OPTION], Qt::CaseInsensitive))
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
        else if (0 == in.compare(options()[djvOpenExr::THREAD_COUNT_OPTION], Qt::CaseInsensitive))
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
        else if (0 == in.compare(options()[djvOpenExr::INPUT_COLOR_PROFILE_OPTION], Qt::CaseInsensitive))
        {
            djvOpenExr::COLOR_PROFILE colorProfile = static_cast<djvOpenExr::COLOR_PROFILE>(0);
            
            data >> colorProfile;
            
            if (colorProfile != _options.inputColorProfile)
            {
                _options.inputColorProfile = colorProfile;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvOpenExr::INPUT_GAMMA_OPTION], Qt::CaseInsensitive))
        {
            double gamma = 0.0;
            
            data >> gamma;
            
            if (gamma != _options.inputGamma)
            {
                _options.inputGamma = gamma;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvOpenExr::INPUT_EXPOSURE_OPTION], Qt::CaseInsensitive))
        {
            djvColorProfile::Exposure exposure;
            
            data >> exposure;
            
            if (exposure != _options.inputExposure)
            {
                _options.inputExposure = exposure;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvOpenExr::CHANNELS_OPTION], Qt::CaseInsensitive))
        {
            djvOpenExr::CHANNELS channels = static_cast<djvOpenExr::CHANNELS>(0);
            
            data >> channels;
            
            if (channels != _options.channels)
            {
                _options.channels = channels;

                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[djvOpenExr::COMPRESSION_OPTION], Qt::CaseInsensitive))
        {
            djvOpenExr::COMPRESSION compression = static_cast<djvOpenExr::COMPRESSION>(0);
            
            data >> compression;
            
            if (compression != _options.compression)
            {
                _options.compression = compression;
                
                Q_EMIT optionChanged(in);
            }
        }
#if OPENEXR_VERSION_HEX >= 0x02020000
        else if (0 == in.compare(options()[djvOpenExr::DWA_COMPRESSION_LEVEL_OPTION], Qt::CaseInsensitive))
        {
            double compressionLevel = 0.0;
            
            data >> compressionLevel;
            
            if (! djvMath::fuzzyCompare(
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

QStringList djvOpenExrPlugin::options() const
{
    return djvOpenExr::optionsLabels();
}

void djvOpenExrPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (
                qApp->translate("djvOpenExrPlugin", "-exr_threads_enable") == arg)
            {
                in >> _options.threadsEnable;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_thread_count") == arg)
            {
                in >> _options.threadCount;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_input_color_profile") == arg)
            {
                in >> _options.inputColorProfile;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_input_gamma") == arg)
            {
                in >> _options.inputGamma;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_input_exposure") == arg)
            {
                in >> _options.inputExposure;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_channels") == arg)
            {
                in >> _options.channels;
            }
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_compression") == arg)
            {
                in >> _options.compression;
            }
#if OPENEXR_VERSION_HEX >= 0x02020000
            else if (
                qApp->translate("djvOpenExrPlugin", "-exr_dwa_compression_level") == arg)
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

QString djvOpenExrPlugin::commandLineHelp() const
{
    return qApp->translate("djvOpenExrPlugin",
"\n"
"OpenEXR Options\n"
"\n"
"    -exr_threads_enable (value)\n"
"        Set whether threading is enabled. Default = %1.\n"
"    -exr_thread_count (value)\n"
"        Set the maximum number of threads to use. Default = %2.\n"
"    -exr_input_color_profile (value)\n"
"        Set the color profile used when loading OpenEXR images. Options = "
"%3. Default = %4.\n"
"    -exr_input_gamma (value)\n"
"        Set the gamma values used when loading OpenEXR images. Default = "
"%5.\n"
"    -exr_input_exposure (value) (defog) (knee low) (knee high)\n"
"        Set the exposure values used when loading OpenEXR images. Default = "
"%6.\n"
"    -exr_channels (value)\n"
"        Set how channels are grouped when loading OpenEXR images. Options = "
"%7. Default = %8.\n"
"    -exr_compression (value)\n"
"        Set the file compression used when saving OpenEXR images. Options = "
"%9. Default = %10.\n"
#if OPENEXR_VERSION_HEX >= 0x02020000
"    -exr_dwa_compression_level (value)\n"
"        Set the DWA compression level used when saving OpenEXR images. "
"Default = %11.\n"
#endif // OPENEXR_VERSION_HEX
    ).
    arg(djvStringUtil::label(_options.threadsEnable).join(", ")).
    arg(djvStringUtil::label(_options.threadCount).join(", ")).
    arg(djvOpenExr::colorProfileLabels().join(", ")).
    arg(djvStringUtil::label(_options.inputColorProfile).join(", ")).
    arg(djvStringUtil::label(_options.inputGamma).join(", ")).
    arg(djvStringUtil::label(_options.inputExposure).join(", ")).
    arg(djvOpenExr::channelsLabels().join(", ")).
    arg(djvStringUtil::label(_options.channels).join(", ")).
    arg(djvOpenExr::compressionLabels().join(", ")).
    arg(djvStringUtil::label(_options.compression).join(", "))
#if OPENEXR_VERSION_HEX >= 0x02020000
    .
    arg(_options.dwaCompressionLevel)
#endif // OPENEXR_VERSION_HEX
    ;
}

djvImageLoad * djvOpenExrPlugin::createLoad() const
{
    return new djvOpenExrLoad(_options, imageContext());
}

djvImageSave * djvOpenExrPlugin::createSave() const
{
    return new djvOpenExrSave(_options, imageContext());
}

void djvOpenExrPlugin::threadsUpdate()
{
    //DJV_DEBUG("djvOpenExrPlugin::threadsUpdate");
    //DJV_DEBUG_PRINT("this = " << uint64_t(this));
    //DJV_DEBUG_PRINT("threads = " << _options.threadsEnable);
    //DJV_DEBUG_PRINT("thread count = " << _options.threadsCount);

    Imf::setGlobalThreadCount(
        _options.threadsEnable ? _options.threadCount : 0);
}

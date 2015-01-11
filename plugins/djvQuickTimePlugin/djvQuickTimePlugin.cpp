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

//! \file djvQuickTimePlugin.cpp

#if defined(DJV_OSX)

//! \todo Force include order on OS X.

#include <sys/types.h>
#include <sys/stat.h>

#endif

#include <djvQuickTimePlugin.h>

#include <djvQuickTimeLoad.h>
#include <djvQuickTimeSave.h>
#include <djvQuickTimeWidget.h>

#include <djvDebug.h>
#include <djvError.h>

namespace qt
{

#if defined(DJV_OSX)
#include <QuickTime/QTML.h>
#elif defined(DJV_WINDOWS)
#include <QTML.h>
#endif

} // qt

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvQuickTimePlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvQuickTimePlugin::Handle
//------------------------------------------------------------------------------

djvQuickTimePlugin::Handle::Handle(qt::Handle in) :
    p(in)
{}

djvQuickTimePlugin::Handle::~Handle()
{
    if (p)
    {
        qt::DisposeHandle(p);
    }
}

void djvQuickTimePlugin::Handle::init(long in) throw (djvError)
{
    p = qt::NewHandle(in);
}

//------------------------------------------------------------------------------
// djvQuickTimePlugin::CFStringRef
//------------------------------------------------------------------------------

djvQuickTimePlugin::CFStringRef::CFStringRef(const QString & in) :
    p(qt::CFStringCreateWithCString(0, in.toLatin1().data(), 0))
{}

djvQuickTimePlugin::CFStringRef::~CFStringRef()
{
    qt::CFRelease(p);
}

//------------------------------------------------------------------------------
// djvQuickTimePlugin::Options
//------------------------------------------------------------------------------

djvQuickTimePlugin::Options::Options() :
    codec  (CODEC_MJPEGA),
    quality(QUALITY_NORMAL)
{}

//------------------------------------------------------------------------------
// djvQuickTimePlugin
//------------------------------------------------------------------------------

const QString djvQuickTimePlugin::staticName = "QuickTime";

const QStringList & djvQuickTimePlugin::codecLabels()
{
    static const QStringList data = QStringList() <<
        "Raw" <<
        "JPEG" <<
        "MJPEG-A" <<
        "MJPEG-B" <<
        "H263" <<
        "H264" <<
        "DVC-NTSC" <<
        "DVC-PAL";

    DJV_ASSERT(data.count() == CODEC_COUNT);

    return data;
}

const QStringList & djvQuickTimePlugin::qualityLabels()
{
    static const QStringList data = QStringList() <<
        "Lossless" <<
        "Min" <<
        "Max" <<
        "Low" <<
        "Normal" <<
        "High";

    DJV_ASSERT(data.count() == QUALITY_COUNT);

    return data;
}

qt::CodecType djvQuickTimePlugin::toQuickTimeCodec(CODEC in)
{
    switch (in)
    {
        case CODEC_RAW:     return qt::kRawCodecType;
        case CODEC_JPEG:    return qt::kJPEGCodecType;
        case CODEC_MJPEGA:  return qt::kMotionJPEGACodecType;
        case CODEC_MJPEGB:  return qt::kMotionJPEGBCodecType;
        case CODEC_H263:    return qt::kH263CodecType;
        case CODEC_H264:    return qt::kH264CodecType;
        case CODEC_DVCNTSC: return qt::kDVCNTSCCodecType;
        case CODEC_DVCPAL:  return qt::kDVCPALCodecType;

        default: break;
    }

    return 0;
}

qt::CodecQ djvQuickTimePlugin::toQuickTimeQuality(QUALITY in)
{
    switch (in)
    {
        case QUALITY_LOSSLESS: return qt::codecLosslessQuality;
        case QUALITY_MIN:      return qt::codecMinQuality;
        case QUALITY_MAX:      return qt::codecMaxQuality;
        case QUALITY_LOW:      return qt::codecLowQuality;
        case QUALITY_NORMAL:   return qt::codecNormalQuality;
        case QUALITY_HIGH:     return qt::codecHighQuality;

        default: break;
    }

    return 0;
}

const QStringList & djvQuickTimePlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        "Codec" <<
        "Quality";

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

namespace
{

int  _refCount        = 0;
bool _qtmlInit        = false;
bool _enterMoviesInit = false;

} // namespace
    
void djvQuickTimePlugin::initPlugin() throw (djvError)
{
    ++_refCount;

    if (_refCount > 1)
        return;

    //DJV_DEBUG("djvQuickTimePlugin::initPlugin");

    qt::OSErr err = qt::noErr;

# if defined(DJV_WINDOWS)

    //DJV_DEBUG_PRINT("initialize QTML");

    err = qt::InitializeQTML(0); //qt::kInitializeQTMLNoSoundFlag);

    if (err != qt::noErr)
    {
        throw djvError(
            staticName,
            QString("Cannot initialize (#%1)").arg(err));
    }

    _qtmlInit = true;

# endif // DJV_WINDOWS

    //DJV_DEBUG_PRINT("get version");

    qt::SInt32 version = 0;
    
    err = qt::Gestalt(qt::gestaltQuickTime, &version);

    if (err != qt::noErr)
    {
        throw djvError(
            staticName,
            QString("Incompatible version (#%1)").arg(err));
    }

    //DJV_DEBUG_PRINT("version = " << version);

    //DJV_DEBUG_PRINT("enter movies");

    err = qt::EnterMovies();

    if (err != qt::noErr)
    {
        throw djvError(
            staticName,
            QString("Cannot initialize (#%1)").arg(err));
    }

    _enterMoviesInit = true;
}

void djvQuickTimePlugin::releasePlugin()
{
    --_refCount;

    if (_refCount)
        return;

    //DJV_DEBUG("djvQuickTimePlugin::releasePlugin");

    if (_enterMoviesInit)
    {
        //DJV_DEBUG_PRINT("exit movies");

        qt::ExitMovies();

        _enterMoviesInit = false;
    }

# if defined(DJV_WINDOWS)

    if (_qtmlInit)
    {
        //DJV_DEBUG_PRINT("terminate QTML");

        qt::TerminateQTML();

        _qtmlInit = false;
    }

# endif // DJV_WINDOWS
}

djvPlugin * djvQuickTimePlugin::copyPlugin() const
{
    djvQuickTimePlugin * plugin = new djvQuickTimePlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvQuickTimePlugin::pluginName() const
{
    return staticName;
}

QStringList djvQuickTimePlugin::extensions() const
{
    return QStringList() << QStringList() <<
       ".qt"  <<
       ".mov" <<
       ".avi" <<
       ".mp4";
}

bool djvQuickTimePlugin::isSeq() const
{
    return false;
}

QStringList djvQuickTimePlugin::option(const QString & in) const
{
    QStringList out;

    if (0 == in.compare(options()[CODEC_OPTION], Qt::CaseInsensitive))
    {
        out << _options.codec;
    }
    else if (0 == in.compare(options()[QUALITY_OPTION], Qt::CaseInsensitive))
    {
        out << _options.quality;
    }

    return out;
}

bool djvQuickTimePlugin::setOption(const QString & in, QStringList & data)
{
    try
    {
        if (0 == in.compare(options()[CODEC_OPTION], Qt::CaseInsensitive))
        {
            CODEC codec = static_cast<CODEC>(0);
            
            data >> codec;
            
            if (codec != _options.codec)
            {
                _options.codec = codec;
            
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(options()[QUALITY_OPTION], Qt::CaseInsensitive))
        {
            QUALITY quality = static_cast<QUALITY>(0);
            
            data >> quality;
            
            if (quality != _options.quality)
            {
                _options.quality = quality;
            
                Q_EMIT optionChanged(in);
            }
        }
    }
    catch (QString)
    {
        return false;
    }

    return true;
}

QStringList djvQuickTimePlugin::options() const
{
    return optionsLabels();
}

void djvQuickTimePlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if ("-quicktime_codec" == arg)
            {
                in >> _options.codec;
            }
            else if ("-quicktime_quality" == arg)
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

QString djvQuickTimePlugin::commandLineHelp() const
{
    return QString(
"\n"
"QuickTime Options\n"
"\n"
"    -quicktime_codec (value)\n"
"        Set the codec used when saving QuickTime movies. Options = %1. "
"Default = %2.\n"
"    -quicktime_quality (value)\n"
"        Set the quality used when saving QuickTime movies. Options = %3. "
"Default = %4.\n").
    arg(codecLabels().join(", ")).
    arg(djvStringUtil::label(_options.codec).join(", ")).
    arg(qualityLabels().join(", ")).
    arg(djvStringUtil::label(_options.quality).join(", "));
}
    
djvImageLoad * djvQuickTimePlugin::createLoad() const
{
    return new djvQuickTimeLoad(_options);
}

djvImageSave * djvQuickTimePlugin::createSave() const
{
    return new djvQuickTimeSave(_options);
}

djvAbstractPrefsWidget * djvQuickTimePlugin::createWidget()
{
    return new djvQuickTimeWidget(this);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvQuickTimePlugin::CODEC,
    djvQuickTimePlugin::codecLabels())
_DJV_STRING_OPERATOR_LABEL(djvQuickTimePlugin::QUALITY,
    djvQuickTimePlugin::qualityLabels())

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

//! \file djvFFmpegPlugin.cpp

#include <djvFFmpegPlugin.h>

#include <djvFFmpegLoad.h>
#include <djvFFmpegSave.h>

#include <djvAssert.h>
#include <djvDebugLog.h>
#include <djvError.h>
#include <djvFileIoUtil.h>
#include <djvMath.h>

#include <QApplication>

extern "C"
{

DJV_PLUGIN_EXPORT djvPlugin * djvImageIo()
{
    return new djvFFmpegPlugin;
}

} // extern "C"

//------------------------------------------------------------------------------
// djvFFmpegPlugin::Options
//------------------------------------------------------------------------------

djvFFmpegPlugin::Options::Options() :
    format (MPEG4),
    quality(HIGH)
{}

//------------------------------------------------------------------------------
// djvFFmpegPlugin
//------------------------------------------------------------------------------

const QString djvFFmpegPlugin::staticName = "FFmpeg";

const QStringList & djvFFmpegPlugin::formatLabels()
{
    static const QStringList data = QStringList() <<
        //qApp->translate("djvFFmpegPlugin", "H264") <<
        qApp->translate("djvFFmpegPlugin", "MPEG4") <<
        qApp->translate("djvFFmpegPlugin", "ProRes") <<
        qApp->translate("djvFFmpegPlugin", "MJPEG");

    DJV_ASSERT(data.count() == FORMAT_COUNT);

    return data;
}

const QStringList & djvFFmpegPlugin::qualityLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFFmpegPlugin", "Low") <<
        qApp->translate("djvFFmpegPlugin", "Medium") <<
        qApp->translate("djvFFmpegPlugin", "High");

    DJV_ASSERT(data.count() == QUALITY_COUNT);

    return data;
}

const QStringList & djvFFmpegPlugin::optionsLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvFFmpegPlugin", "Format") <<
        qApp->translate("djvFFmpegPlugin", "Quality");

    DJV_ASSERT(data.count() == OPTIONS_COUNT);

    return data;
}

namespace
{

void avLogCallback(void * ptr, int level, const char * fmt, va_list vl)
{
    if (level > av_log_get_level())
        return;

    //! \todo Add multi-thread safe logging.

    /*char s [djvStringUtil::cStringLength];
    
    SNPRINTF(s, djvStringUtil::cStringLength, fmt, vl);
    
    DJV_LOG("djvFFmpegPlugin", s);*/
}

} // namespace

void djvFFmpegPlugin::initPlugin() throw (djvError)
{
    //DJV_DEBUGBUG("djvFFmpegPlugin::initPlugin");
    
    av_log_set_callback(avLogCallback);
    
    av_register_all();

    /*const AVOutputFormat * avFormat = 0;
    
    while ((avFormat = av_oformat_next(avFormat)))
    {
        DJV_DEBUG_PRINT("av format = " << avFormat->name << ", " <<
            avFormat->long_name);
    }*/

    /*const AVCodecDescriptor * avCodec = 0;
    
    while ((avCodec = avcodec_descriptor_next(avCodec)))
    {
        if (AVMEDIA_TYPE_VIDEO == avCodec->type)
        {
            DJV_DEBUG_PRINT("av codec = " << avCodec->name);
        }
    }*/

    /*AVOutputFormat * avFormat = av_oformat_next(0);
    
    while (avFormat)
    {
        DJV_DEBUG_PRINT("av format = " << avFormat->name);
        
        if (avFormat->codec_tag)
        {
            AVCodecID avCodec = static_cast<AVCodecID>(1);
            
            int i = 0;

            while (avCodec != CODEC_ID_NONE)
            {
                avCodec = av_codec_get_id(format->codec_tag, i++);
                
                DJV_DEBUG_PRINT("    " << avCodecId);
            }
        }
        
        avFormat = av_oformat_next(avFormat);
    }*/
}

djvPlugin * djvFFmpegPlugin::copyPlugin() const
{
    djvFFmpegPlugin * plugin = new djvFFmpegPlugin;
    
    plugin->_options = _options;
    
    return plugin;
}

QString djvFFmpegPlugin::pluginName() const
{
    return staticName;
}

QStringList djvFFmpegPlugin::extensions() const
{
    return QStringList() <<
        ".avi"  <<
        ".dv"   <<
        ".gif"  <<
        ".flv"  <<
        ".mkv"  <<
        ".mov"  <<
        ".mpg"  <<
        ".mpeg" <<
        ".mp4"  <<
        ".m4v"  <<
        ".mxf";
}

bool djvFFmpegPlugin::isSequence() const
{
    return false;
}

QStringList djvFFmpegPlugin::option(const QString & in) const
{
    const QStringList & list = options();

    QStringList out;

    if (0 == in.compare(list[OPTIONS_FORMAT], Qt::CaseInsensitive))
    {
        out << _options.format;
    }
    else if (0 == in.compare(list[OPTIONS_QUALITY], Qt::CaseInsensitive))
    {
        out << _options.quality;
    }

    return out;
}

bool djvFFmpegPlugin::setOption(const QString & in, QStringList & data)
{
    const QStringList & list = options();

    try
    {
        if (0 == in.compare(list[OPTIONS_FORMAT], Qt::CaseInsensitive))
        {
            FORMAT format = static_cast<FORMAT>(0);
            
            data >> format;
            
            if (format != _options.format)
            {
                _options.format = format;
                
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(list[OPTIONS_QUALITY], Qt::CaseInsensitive))
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

QStringList djvFFmpegPlugin::options() const
{
    return optionsLabels();
}

void djvFFmpegPlugin::commandLine(QStringList & in) throw (QString)
{
    QStringList tmp;
    QString     arg;

    try
    {
        while (! in.isEmpty())
        {
            in >> arg;

            if (qApp->translate("djvFFmpegPlugin", "-ffmpeg_format") == arg)
            {
                in >> _options.format;
            }
            else if (qApp->translate("djvFFmpegPlugin", "-ffmpeg_quality") == arg)
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

QString djvFFmpegPlugin::commandLineHelp() const
{
    return qApp->translate("djvFFmpegPlugin",
"\n"
"FFmpeg Options\n"
"\n"
"    -ffmpeg_format (value)\n"
"        Set the format used when saving FFmpeg movies. Options = %1. "
"Default = %2.\n"
"    -ffmpeg_quality (value)\n"
"        Set the quality used when saving FFmpeg movies. Options = %3. "
"Default = %4.\n"
    ).
    arg(formatLabels().join(", ")).
    arg(djvStringUtil::label(_options.format).join(", ")).
    arg(qualityLabels().join(", ")).
    arg(djvStringUtil::label(_options.quality).join(", "));
}

djvImageLoad * djvFFmpegPlugin::createLoad() const
{
    return new djvFFmpegLoad;
}

djvImageSave * djvFFmpegPlugin::createSave() const
{
    return new djvFFmpegSave(_options);
}

//------------------------------------------------------------------------------

_DJV_STRING_OPERATOR_LABEL(djvFFmpegPlugin::FORMAT,
    djvFFmpegPlugin::formatLabels())
_DJV_STRING_OPERATOR_LABEL(djvFFmpegPlugin::QUALITY,
    djvFFmpegPlugin::qualityLabels())

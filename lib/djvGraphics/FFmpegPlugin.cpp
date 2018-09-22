//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <djvGraphics/FFmpegPlugin.h>

#include <djvGraphics/FFmpegLoad.h>
#include <djvGraphics/FFmpegSave.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
// djvFFmpegPlugin
//------------------------------------------------------------------------------

djvFFmpegPlugin::djvFFmpegPlugin(djvCoreContext * context) :
    djvImageIO(context)
{}

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

void djvFFmpegPlugin::initPlugin()
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

QString djvFFmpegPlugin::pluginName() const
{
    return djvFFmpeg::staticName;
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
    if (0 == in.compare(list[djvFFmpeg::OPTIONS_FORMAT], Qt::CaseInsensitive))
    {
        out << _options.format;
    }
    else if (0 == in.compare(list[djvFFmpeg::OPTIONS_QUALITY], Qt::CaseInsensitive))
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
        if (0 == in.compare(list[djvFFmpeg::OPTIONS_FORMAT], Qt::CaseInsensitive))
        {
            djvFFmpeg::FORMAT format = static_cast<djvFFmpeg::FORMAT>(0);
            data >> format;
            if (format != _options.format)
            {
                _options.format = format;
                Q_EMIT optionChanged(in);
            }
        }
        else if (0 == in.compare(list[djvFFmpeg::OPTIONS_QUALITY], Qt::CaseInsensitive))
        {
            djvFFmpeg::QUALITY quality = static_cast<djvFFmpeg::QUALITY>(0);
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
    return djvFFmpeg::optionsLabels();
}

void djvFFmpegPlugin::commandLine(QStringList & in)
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
    QStringList formatLabel;
    formatLabel << _options.format;
    QStringList qualityLabel;
    qualityLabel << _options.quality;
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
    arg(djvFFmpeg::formatLabels().join(", ")).
    arg(formatLabel.join(", ")).
    arg(djvFFmpeg::qualityLabels().join(", ")).
    arg(qualityLabel.join(", "));
}

djvImageLoad * djvFFmpegPlugin::createLoad() const
{
    return new djvFFmpegLoad(context());
}

djvImageSave * djvFFmpegPlugin::createSave() const
{
    return new djvFFmpegSave(_options, context());
}

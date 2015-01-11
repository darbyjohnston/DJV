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

//! \file djvQuickTimePlugin.h

#ifndef DJV_QUICKTIME_PLUGIN_H
#define DJV_QUICKTIME_PLUGIN_H

#include <djvImageIo.h>
#include <djvOpenGl.h>

namespace qt
{

#if defined(DJV_OSX)
#include <QuickTime/QuickTime.h>
#elif defined(DJV_WINDOWS)
#include <Movies.h>
#endif

//! \todo Is it still necessary to define this QuickTime variable?

#define qt_fixed1 ((qt::Fixed)0x00010000L)

//! \todo QuickTime defines the macro "check"?

#undef check

} // qt

//! \addtogroup plugins
//@{

//! \defgroup djvQuickTimePlugin djvQuickTimePlugin
//!
//! This plugin provides support for the Apple QuickTime movie file format.
//! Note that this plugin is only available for Apple OS X and Microsoft
//! Windows 32-bit builds.
//!
//! Requires:
//!
//! - QuickTime - http://www.apple.com/quicktime
//!
//! File extensions: .qt, .mov, .mp4
//!
//! Supported features:
//!
//! - 8-bit RGBA
//! - File compression

//@} // plugins

//! \addtogroup djvQuickTimePlugin
//@{

//------------------------------------------------------------------------------
//! \class djvQuickTimePlugin
//!
//! This class provides a QuickTime plugin.
//------------------------------------------------------------------------------

class djvQuickTimePlugin : public djvImageIo
{
public:

    //! Plugin name.
    
    static const QString staticName;

    //! This enumeration provides the video codecs.

    enum CODEC
    {
        CODEC_RAW,
        CODEC_JPEG,
        CODEC_MJPEGA,
        CODEC_MJPEGB,
        CODEC_H263,
        CODEC_H264,
        CODEC_DVCNTSC,
        CODEC_DVCPAL,

        CODEC_COUNT
    };

    //! Get the video codec labels.

    static const QStringList & codecLabels();

    //! This enumeration provides the compression quality.

    enum QUALITY
    {
        QUALITY_LOSSLESS,
        QUALITY_MIN,
        QUALITY_MAX,
        QUALITY_LOW,
        QUALITY_NORMAL,
        QUALITY_HIGH,

        QUALITY_COUNT
    };

    //! Get the compression quality labels.

    static const QStringList & qualityLabels();

    //! This class provides a generic QuickTime handle.

    class Handle
    {
    public:

        Handle(qt::Handle = 0);

        ~Handle();

        void init(long) throw (djvError);

        qt::Handle p;
    };

    //! This class provides a QuickTime string handle.

    class CFStringRef
    {
    public:

        CFStringRef(const QString &);

        ~CFStringRef();

        qt::CFStringRef p;
    };

    //! Convert to a QuickTime codec.

    static qt::CodecType toQuickTimeCodec(CODEC);

    //! Convert to a QuickTime quality.

    static qt::CodecQ toQuickTimeQuality(QUALITY);

    //! This enumeration provides the options.

    enum OPTIONS
    {
        CODEC_OPTION,
        QUALITY_OPTION,

        OPTIONS_COUNT
    };

    //! Get the option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        //! Constructor.

        Options();

        CODEC   codec;
        QUALITY quality;
    };
    
    virtual void initPlugin() throw (djvError);
    
    virtual void releasePlugin();

    virtual djvPlugin * copyPlugin() const;

    virtual QString pluginName() const;

    virtual QStringList extensions() const;

    virtual bool isSeq() const;

    virtual QStringList option(const QString &) const;

    virtual bool setOption(const QString &, QStringList &);

    virtual QStringList options() const;

    virtual void commandLine(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;
    
    virtual djvImageLoad * createLoad() const;
    
    virtual djvImageSave * createSave() const;

    virtual djvAbstractPrefsWidget * createWidget();

private:

    Options _options;
};

//------------------------------------------------------------------------------

DJV_STRING_OPERATOR(, djvQuickTimePlugin::CODEC);
DJV_STRING_OPERATOR(, djvQuickTimePlugin::QUALITY);

//@} // djvQuickTimePlugin

#endif // DJV_QUICKTIME_PLUGIN_H


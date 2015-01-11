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

//! \file djvQuickTimeSave.cpp

#include <djvQuickTimeSave.h>

#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvOpenGlImage.h>

#include <QDir>

namespace qt
{

#if defined(DJV_OSX)
#elif defined(DJV_WINDOWS)
#include <ImageCompression.h>
#include <QTML.h>
#endif

//! \todo Is it still necessary to define this QuickTime variable?

#define qt_anyCodec ((qt::CodecComponent)0)

} // qt

//------------------------------------------------------------------------------
// djvQuickTimeSave
//------------------------------------------------------------------------------

djvQuickTimeSave::djvQuickTimeSave(const djvQuickTimePlugin::Options & options) :
    _options      (options),
    _f            (0),
    _frameDuration(0),
    _movie        (0),
    _track        (0),
    _media        (0),
    _gworld       (0)
{}

djvQuickTimeSave::~djvQuickTimeSave()
{}

void djvQuickTimeSave::open(const djvFileInfo & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeSave::open");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("info = " << info);

    // Open the file.
    
    _open(in, info);

    _track = qt::NewMovieTrack(
        _movie,
        qt::FixRatio(info.size.x, 1),
        qt::FixRatio(info.size.y, 1),
        qt::kNoVolume);

    if (! _track)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot create track: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(qt::GetMoviesError()));
    }
    
    _media = qt::NewTrackMedia(
        _track,
        qt::VideoMediaType,
        info.sequence.speed.scale(),
        0,
        0);

    if (! _media)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot create media: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(qt::GetMoviesError()));
    }

    qt::OSErr err = qt::BeginMediaEdits(_media);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot begin editing media: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(err));
    }

    _file = in;

    _frameDuration = info.sequence.speed.duration();

    // Allocate temporary buffers.

    djvPixelDataInfo _info;
    _info.size     = info.size;
    _info.mirror.y = true;
    _info.pixel    = djvPixel::RGBA_U8;
    _image.set(_info);

    const qt::Rect rect = { 0, 0, _info.size.y, _info.size.x };
    
    err = QTNewGWorldFromPtr(
        &_gworld,
        qt::k32RGBAPixelFormat,
        &rect,
        0,
        0,
        0,
        (char *)_image.data(0, 0),
        _info.size.x * 4);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot create GWorld: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(err));
    }

    qt::SetMovieGWorld(_movie, _gworld, 0);
}

void djvQuickTimeSave::_open(const QString & in, const djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeSave::_open");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("in = " << djvFileInfoUtil::fixPath(in));
    //DJV_DEBUG_PRINT("info = " << info);

    djvQuickTimePlugin::Handle handle;
    qt::OSType                 handleType;
    
    qt::OSErr err = qt::QTNewDataReferenceFromFullPathCFString(
        djvQuickTimePlugin::CFStringRef(djvFileInfoUtil::fixPath(in)).p,
        //qt::kQTNativeDefaultPathStyle,
        qt::kQTPOSIXPathStyle,
        0,
        &handle.p,
        &handleType);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot create data reference: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(err));
    }

    err = qt::CreateMovieStorage(
        handle.p,
        handleType,
        FOUR_CHAR_CODE('TVOD'),
        qt::smCurrentScript,
        qt::createMovieFileDeleteCurFile | qt::createMovieFileDontCreateResFile,
        &_f,
        &_movie);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot open: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(err));
    }
}

namespace
{

struct _LockPixels
{
    _LockPixels(qt::PixMapHandle in) :
        _p(in)
    {
        LockPixels(_p);
    }

    ~_LockPixels()
    {
        UnlockPixels(_p);
    }

private:

    qt::PixMapHandle _p;
};

} // namespace

void djvQuickTimeSave::write(const djvImage & in, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeSave::write");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("frame = " << frame);

    // Copy the input to the temporary buffer.

    djvOpenGlImage::copy(in, _image);

    // Compress the image.

    const qt::PixMapHandle pixmap =
#if defined(DJV_WINDOWS)
        _gworld->portPixMap;
#else
        qt::GetPortPixMap(_gworld);
#endif
    const qt::Rect rect = { 0, 0, _image.h(), _image.w() };
    
    long compressMax = 0;

    qt::OSErr err = qt::GetMaxCompressionSize(
        pixmap,
        &rect,
        0,
        djvQuickTimePlugin::toQuickTimeQuality(_options.quality),
        djvQuickTimePlugin::toQuickTimeCodec(_options.codec),
        (qt::CompressorComponent)qt_anyCodec,
        &compressMax);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot compress frame %1: %2 (#%3)").
                arg(frame.frame).
                arg(QDir::toNativeSeparators(_file)).
                arg(err));
    }

    //DJV_DEBUG_PRINT("compress max = " << static_cast<int>(compressMax));

    djvQuickTimePlugin::Handle compress;
    djvQuickTimePlugin::Handle description;
    compress.init(compressMax);
    description.init(4);

    if (! compress.p || ! description.p)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot compress frame %1: %2 (#%3)").
                arg(frame.frame).
                arg(QDir::toNativeSeparators(_file)).
                arg(err));
    }

    _LockPixels lock_pixels(pixmap);

    err = qt::CompressImage(
        pixmap,
        &rect,
        djvQuickTimePlugin::toQuickTimeQuality(_options.quality),
        djvQuickTimePlugin::toQuickTimeCodec(_options.codec),
        (qt::ImageDescriptionHandle)description.p,
        *compress.p);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot compress frame %1: %2 (#%3)").
                arg(frame.frame).
                arg(QDir::toNativeSeparators(_file)).
                arg(err));
    }

    // Write the image.

    err = qt::AddMediaSample(
        _media,
        compress.p,
        0,
        ((qt::ImageDescription *)*description.p)->dataSize,
        _frameDuration,
        (qt::SampleDescriptionHandle)description.p,
        1,
        0,
        0);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot add frame %1: %2 (#%3)").
                arg(frame.frame).
                arg(QDir::toNativeSeparators(_file)).
                arg(err));
    }
}

void djvQuickTimeSave::close() throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeSave::close");

    if (_track && _media)
    {
        //DJV_DEBUG_PRINT("end");

        qt::OSErr err = qt::EndMediaEdits(_media);

        if (err != qt::noErr)
        {
            throw djvError(
                djvQuickTimePlugin::staticName,
                QString("Cannot end editing media: %1 (#%2)").
                    arg(QDir::toNativeSeparators(_file)).
                    arg(err));
        }

        err = qt::InsertMediaIntoTrack(
            _track,
            0,
            0,
            qt::GetMediaDuration(_media),
            qt_fixed1);

        if (err != qt::noErr)
        {
            throw djvError(
                djvQuickTimePlugin::staticName,
                QString("Cannot add media: %1 (#%2)").
                    arg(QDir::toNativeSeparators(_file)).
                    arg(err));
        }

        qt::AddMovieToStorage(_movie, _f);
    }

    _track = 0;
    _media = 0;

    //if (_f) { qt::CloseMovieFile(_f); _f = 0; }
    
    if (_f)
    {
        qt::CloseMovieStorage(_f);
        
        _f = 0;
    }

    if (_movie)
    {
        qt::DisposeMovie(_movie);
        
        _movie = 0;
    }

    if (_gworld)
    {
        qt::DisposeGWorld(_gworld);
        
        _gworld = 0;
    }
}

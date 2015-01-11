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

//! \file djvQuickTimeLoad.cpp

#include <djvQuickTimeLoad.h>

#include <djvError.h>
#include <djvFileInfoUtil.h>
#include <djvPixelDataUtil.h>

#include <QDir>

namespace qt
{
#if defined(DJV_WINDOWS)
#include <CFString.h>
#include <ImageCompression.h>
#endif
}

//------------------------------------------------------------------------------
// djvQuickTimeLoad
//------------------------------------------------------------------------------

djvQuickTimeLoad::djvQuickTimeLoad(const djvQuickTimePlugin::Options & options) :
    _options   (options),
    _startFrame(1),
    _movie     (0),
    _gworld    (0)
{}

djvQuickTimeLoad::~djvQuickTimeLoad()
{
    close();
}

namespace
{

const qt::OSType mediaList [] = { qt::VisualMediaCharacteristic };

const int mediaListCount = 1;

/*long _frameCount(qt::Movie in)
{
    long out = 0;

    for (qt::TimeValue time = 0; time >= 0; ++out)
    {
        qt::TimeValue tmp = 0;
    
        qt::GetMovieNextInterestingTime(
            in,
            qt::nextTimeStep,
            mediaListCount,
            mediaList,
            time,
            qt_fixed1,
            &tmp,
            0);
    
        time = tmp;
    }

    return out;
}*/

qt::TimeValue _frameDuration(qt::Movie in)
{
    qt::TimeValue out = 0;

    qt::GetMovieNextInterestingTime(
        in,
        qt::nextTimeStep,
        mediaListCount,
        mediaList,
        out,
        qt_fixed1,
        0,
        &out);

    return out;
}

} // namespace

void djvQuickTimeLoad::open(const djvFileInfo & in, djvImageIoInfo & info)
    throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeLoad::open");
    //DJV_DEBUG_PRINT("in = " << in);

    close();

    // Open the file.

    djvQuickTimePlugin::CFStringRef cfStringRef(djvFileInfoUtil::fixPath(in));

    qt::QTNewMoviePropertyElement propList [10];
    qt::ItemCount propCount = 0;

    propList[propCount].propClass = qt::kQTPropertyClass_DataLocation;
    propList[propCount].propID =
        //qt::kQTDataLocationPropertyID_CFStringNativePath;
        qt::kQTDataLocationPropertyID_CFStringPosixPath;
    propList[propCount].propValueSize = sizeof(cfStringRef.p);
    propList[propCount].propValueAddress = (void *)&cfStringRef.p;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::Boolean unresolved = true;
    propList[propCount].propClass = qt::kQTPropertyClass_MovieInstantiation;
    propList[propCount].propID =
        qt::kQTMovieInstantiationPropertyID_DontAskUnresolvedDataRefs;
    propList[propCount].propValueSize = sizeof(unresolved);
    propList[propCount].propValueAddress = &unresolved;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::Boolean active = true;
    propList[propCount].propClass = qt::kQTPropertyClass_NewMovieProperty;
    propList[propCount].propID = qt::kQTNewMoviePropertyID_Active;
    propList[propCount].propValueSize = sizeof(active);
    propList[propCount].propValueAddress = &active;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::Boolean no_interact = true;
    propList[propCount].propClass = qt::kQTPropertyClass_NewMovieProperty;
    propList[propCount].propID =
        qt::kQTNewMoviePropertyID_DontInteractWithUser;
    propList[propCount].propValueSize = sizeof(no_interact);
    propList[propCount].propValueAddress = &no_interact;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::QTVisualContextRef visual = 0;
    propList[propCount].propClass = qt::kQTPropertyClass_Context;
    propList[propCount].propID = qt::kQTContextPropertyID_VisualContext;
    propList[propCount].propValueSize = sizeof(qt::QTVisualContextRef);
    propList[propCount].propValueAddress = &visual;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::QTAudioContextRef audio = 0;
    propList[propCount].propClass = qt::kQTPropertyClass_Context;
    propList[propCount].propID = qt::kQTContextPropertyID_AudioContext;
    propList[propCount].propValueSize = sizeof(qt::QTAudioContextRef);
    propList[propCount].propValueAddress = &audio;
    propList[propCount].propStatus = 0;
    ++propCount;

    qt::OSErr err = qt::NewMovieFromProperties(
        propCount,
        propList,
        0,
        0,
        &_movie);

    if (err != qt::noErr)
    {
        throw djvError(
            djvQuickTimePlugin::staticName,
            QString("Cannot open: %1 (#%2)").
                arg(QDir::toNativeSeparators(in)).
                arg(err));
    }

    // Get the file information.

    _info.fileName = in;

    qt::Rect rect;
    qt::GetMovieBox(_movie, &rect);
    _info.size = djvVector2i(rect.right - rect.left, rect.bottom - rect.top);
    _info.mirror.y = true;
    _info.pixel = djvPixel::RGBA_U8;
    //_info.bgr = true;

    const qt::TimeScale timeScale     = qt::GetMovieTimeScale(_movie);
    const qt::TimeValue timeDuration  = qt::GetMovieDuration(_movie);
    const qt::TimeValue frameDuration = _frameDuration(_movie);
    //const long          frameCount    = _frameCount(_movie);
    //const TimeValue     frameDuration = timeDuration / frameCount;

    //DJV_DEBUG_PRINT("time scale = " << static_cast<int>(timeScale));
    //DJV_DEBUG_PRINT("time duration = " << static_cast<int>(timeDuration));
    //DJV_DEBUG_PRINT("frame count = " << static_cast<int>(frameCount));
    //DJV_DEBUG_PRINT("frame duration = " << static_cast<int>(frameDuration));
    //DJV_DEBUG_PRINT("start frame = " << _startFrame);

    _info.sequence = djvSequence(
        _startFrame,
        frameDuration ?
        (_startFrame + (timeDuration / frameDuration - 1)) :
        _startFrame,
        0,
        djvSpeed(timeScale, frameDuration));

    //DJV_DEBUG_PRINT("time = " << _info.seq);

    info = _info;
    
    //! Allocate the temporary buffer.
    
    _tmp.set(_info);

    unsigned long qtFormat =
#if defined(DJV_OSX)
        qt::k32ARGBPixelFormat;
#else
        qt::k32RGBAPixelFormat;
#endif

    err = qt::NewGWorldFromPtr(
        &_gworld,
        qtFormat,
        &rect,
        0,
        0,
        0,
        (char *)_tmp.data(0, 0),
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

void djvQuickTimeLoad::read(djvImage & image, const djvImageIoFrameInfo & frame)
    throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeLoad::read");
    //DJV_DEBUG_PRINT("frame = " << frame);

    image.colorProfile = djvColorProfile();
    image.tags = djvImageTags();

    int _frame = frame.frame;

    if (-1 == _frame)
    {
        _frame = 0;
    }
    else
    {
        _frame -= _startFrame;
    }

    // Read the image.
    
    const qt::TimeValue time = _frame * _info.sequence.speed.duration();
    
    //DJV_DEBUG_PRINT("time = " << static_cast<int>(time));
    
    qt::SetMovieTimeValue(_movie, time);
    qt::MoviesTask(_movie, 0);
    qt::UpdateMovie(_movie);

    djvPixelDataInfo info = _info;

    if (frame.proxy)
    {
        info.size = djvPixelDataUtil::proxyScale(info.size, frame.proxy);
        info.proxy = frame.proxy;
        
        image.set(info);

        djvPixelDataUtil::proxyScale(_tmp, image, frame.proxy);
    }
    else
    {
        image = _tmp;
    }
    
    // Convert ARGB to RGBA on OS X.

#if defined(DJV_OSX)

    uint32_t * p = reinterpret_cast<uint32_t *>(image.data());
    
    const uint32_t * const end = p + info.size.x * info.size.y;

    switch (Memory::endian())
    {
        case Memory::MSB:
        
            for (; p < end; ++p)
            {
                *p = (*p) << 8;
            }

            break;

        case Memory::LSB:
        
            for (; p < end; ++p)
            {
                *p = (*p) >> 8;
            }

            break;
        
        default: break;
    }

#endif

    //DJV_DEBUG_PRINT("image = " << image);
}

void djvQuickTimeLoad::close() throw (djvError)
{
    //DJV_DEBUG("djvQuickTimeLoad::close");

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

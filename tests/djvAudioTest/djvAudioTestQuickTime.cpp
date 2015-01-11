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

//! \file djvAudioTestQuickTime.cpp

//#define DJV_DEBUG

#include <djvAudioTest.h>

#include <djv_file.h>

namespace qt
{

#if defined(DJV_OSX)
#include <QuickTime/QuickTime.h>
#include <QuickTime/QTML.h>
#include <CoreAudio/CoreAudio.h>
#elif defined(WIN32)
#include <Movies.h>
#include <QTML.h>
#include <CoreAudioTypes.h>
#endif

//! \todo Is this QuickTime variable still needed?

#define qt_fixed1 ((qt::Fixed)0x00010000L)

} // qt

class djvAudioTestMovie::_Movie
{
public:

    _Movie();

    ~_Movie();

    void init(const String & in) throw (Error);

    const Pixel_Data_Info & info() const
    {
        return _info;
    }
    const Image_Time & info_time() const
    {
        return _info_time;
    }
    const Audio_Info & info_audio() const
    {
        return _info_audio;
    }

    const Pixel_Data * image(int) const;
    void audio(int16_t *, ulong) const;
    void audio_seek(int);

private:

    qt::Movie _qt_movie;
    qt::MovieAudioExtractionRef _qt_audio;
    qt::GWorldPtr _qt_gworld;
    Pixel_Data_Info _info;
    Image_Time _info_time;
    Audio_Info _info_audio;
    Pixel_Data _tmp_image;
    Memory_Buffer<int16_t> _tmp_audio;
    int _tmp_frame;
};

djvAudioTestMovie::_Movie::_Movie() :
    _qt_movie(0),
    _qt_audio(0),
    _qt_gworld(0),
    _tmp_frame(-1)
{}

djvAudioTestMovie::_Movie::~_Movie()
{
    if (_qt_audio)
    {
        qt::MovieAudioExtractionEnd(_qt_audio);
    }

    if (_qt_movie)
    {
        qt::DisposeMovie(_qt_movie);
    }

    if (_qt_gworld)
    {
        qt::DisposeGWorld(_qt_gworld);
    }

    qt::ExitMovies();

#if defined(WIN32)
    qt::TerminateQTML();
#endif
}

namespace
{

const qt::OSType media_list [] = { qt::VisualMediaCharacteristic };
const int media_list_size = 1;

/*long _frame_count(qt::Movie in)
{
  long out = 0;
  for (qt::TimeValue time = 0; time >= 0; ++out)
  {
    qt::TimeValue tmp = 0;
    qt::GetMovieNextInterestingTime(in, qt::nextTimeStep, media_list_size,
      media_list, time, qt_fixed1,  &tmp, 0);
    time = tmp;
  }
  return out;
}*/

qt::TimeValue _frame_duration(qt::Movie in)
{
    qt::TimeValue out = 0;
    qt::GetMovieNextInterestingTime(in, qt::nextTimeStep, media_list_size,
        media_list, out, qt_fixed1, 0, &out);
    return out;
}

struct CFStringRef
{
    CFStringRef(const String & in) :
        p(qt::CFStringCreateWithCString(0, in.cStr(), 0))
    {}

    ~CFStringRef()
    {
        qt::CFRelease(p);
    }

    qt::CFStringRef p;
};

}

void djvAudioTestMovie::_Movie::init(const String & in) throw (Error)
{
    DJV_DEBUG("djvAudioTestMovie::_Movie::init");
    DJV_DEBUG_PRINT("in = " << in);

    qt::OSErr err = qt::noErr;

    // Initialize.

#if defined(WIN32)

    err = qt::InitializeQTML(0);

    if (err != qt::noErr)
    {
        throw Error(String_Format("InitializeQTML = %%").arg(err));
    }

#endif

    long version = 0;
    
    err = qt::Gestalt(qt::gestaltQuickTime, &version);

    if (err != qt::noErr)
    {
        throw Error(String_Format("Gestalt = %%").arg(err));
    }

    err = qt::EnterMovies();

    if (err != qt::noErr)
    {
        throw Error( String_Format("EnterMovies = %%").arg(err));
    }

    // Open.

    CFStringRef file(File_Util::path_absolute(in));

    qt::QTNewMoviePropertyElement prop_list [10];
    qt::ItemCount prop_size = 0;

    prop_list[prop_size].propClass = qt::kQTPropertyClass_DataLocation;
    prop_list[prop_size].propID =
        qt::kQTDataLocationPropertyID_CFStringNativePath;
    //qt::kQTDataLocationPropertyID_CFStringPosixPath;
    //qt::kQTDataLocationPropertyID_CFStringWindowsPath;
    prop_list[prop_size].propValueSize = sizeof(qt::CFStringRef);
    prop_list[prop_size].propValueAddress = (void *)&file.p;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;

    qt::Boolean unresolved = true;
    prop_list[prop_size].propClass = qt::kQTPropertyClass_MovieInstantiation;
    prop_list[prop_size].propID =
        qt::kQTMovieInstantiationPropertyID_DontAskUnresolvedDataRefs;
    prop_list[prop_size].propValueSize = sizeof(unresolved);
    prop_list[prop_size].propValueAddress = &unresolved;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;

    qt::Boolean active = true;
    prop_list[prop_size].propClass = qt::kQTPropertyClass_NewMovieProperty;
    prop_list[prop_size].propID = qt::kQTNewMoviePropertyID_Active;
    prop_list[prop_size].propValueSize = sizeof(active);
    prop_list[prop_size].propValueAddress = &active;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;

    qt::Boolean no_interact = true;
    prop_list[prop_size].propClass = qt::kQTPropertyClass_NewMovieProperty;
    prop_list[prop_size].propID =
        qt::kQTNewMoviePropertyID_DontInteractWithUser;
    prop_list[prop_size].propValueSize = sizeof(no_interact);
    prop_list[prop_size].propValueAddress = &no_interact;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;

    qt::QTVisualContextRef visual = 0;
    prop_list[prop_size].propClass = qt::kQTPropertyClass_Context;
    prop_list[prop_size].propID = qt::kQTContextPropertyID_VisualContext;
    prop_list[prop_size].propValueSize = sizeof(qt::QTVisualContextRef);
    prop_list[prop_size].propValueAddress = &visual;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;

    /*qt::QTAudioContextRef audio = 0;
    prop_list[prop_size].propClass = qt::kQTPropertyClass_Context;
    prop_list[prop_size].propID = qt::kQTContextPropertyID_AudioContext;
    prop_list[prop_size].propValueSize = sizeof(qt::QTAudioContextRef);
    prop_list[prop_size].propValueAddress = &audio;
    prop_list[prop_size].propStatus = 0;
    ++prop_size;*/

    err = qt::NewMovieFromProperties(prop_size, prop_list, 0, 0, &_qt_movie);

    if (err != qt::noErr)
    {
        throw Error(String_Format("NewMovieFromProperties = %%").arg(err));
    }

    // Information.

    qt::Rect rect;
    qt::GetMovieBox(_qt_movie, &rect);
    _info.size = V2i(rect.right - rect.left, rect.bottom - rect.top);
    _info.mirror.y = true;
    _info.pixel = Pixel::RGBA_U8;
    _tmp_image.set(_info);
    DJV_DEBUG_PRINT("info = " << _info);

    const qt::TimeScale time_scale = qt::GetMovieTimeScale(_qt_movie);
    const qt::TimeValue time_duration = qt::GetMovieDuration(_qt_movie);
    //const long frame_count = _frame_count(_qt_movie);
    const qt::TimeValue frame_duration = _frame_duration(_qt_movie);
    _info_time = Image_Time(
        0, frame_duration ? (time_duration / frame_duration - 1) : 0,
        Image_Speed(time_scale, frame_duration));
    DJV_DEBUG_PRINT("time scale = " << static_cast<int>(time_scale));
    DJV_DEBUG_PRINT("time duration = " << static_cast<int>(time_duration));
    //DJV_DEBUG_PRINT("frame count = " << static_cast<int>(frame_count));
    DJV_DEBUG_PRINT("frame duration = " << static_cast<int>(frame_duration));
    DJV_DEBUG_PRINT("time = " << _info_time);

    err = MovieAudioExtractionBegin(_qt_movie, 0, &_qt_audio);

    if (err != qt::noErr)
    {
        throw Error(String_Format("MovieAudioExtractionBegin = %%").arg(err));
    }

    qt::UInt32 qt_audio_size = 0;
    err = qt::MovieAudioExtractionGetPropertyInfo(_qt_audio,
        qt::kQTPropertyClass_MovieAudioExtraction_Audio,
        qt::kQTMovieAudioExtractionAudioPropertyID_AudioChannelLayout,
        0,
        &qt_audio_size,
        0);

    if (err != qt::noErr)
        throw Error(String_Format("MovieAudioExtractionGetPropertyInfo = %%").
            arg(err));

    /*struct Audio_Channel
    {
      Audio_Channel() : p(0) {}
      ~Audio_Channel() { if (p) free(p); }

      void init(qt::MovieAudioExtractionRef in) throw (Error)
      {
        qt::UInt32 size = 0;
        qt::OSErr err = qt::MovieAudioExtractionGetPropertyInfo(in,
          qt::kQTPropertyClass_MovieAudioExtraction_Audio,
          qt::kQTMovieAudioExtractionAudioPropertyID_AudioChannelLayout,
          0,
          &size,
          0
        );
        if (err != qt::noErr)
          throw Error(String_Format("MovieAudioExtractionGetPropertyInfo = %%").
            arg(err));
        p = (qt::AudioChannelLayout *)malloc(size);
        err = qt::MovieAudioExtractionGetProperty(in,
          qt::kQTPropertyClass_MovieAudioExtraction_Audio,
          qt::kQTMovieAudioExtractionAudioPropertyID_AudioChannelLayout,
          size,
          p,
          0
        );
        if (err != qt::noErr)
          throw Error(String_Format("MovieAudioExtractionGetProperty = %%").
            arg(err));
      }

      qt::AudioChannelLayout * p;

    };
    Audio_Channel audio_channel;
    audio_channel.init(_qt_audio);*/

    qt::AudioStreamBasicDescription asbd;
    err = qt::MovieAudioExtractionGetProperty(
        _qt_audio,
        qt::kQTPropertyClass_MovieAudioExtraction_Audio,
        qt::kQTMovieAudioExtractionAudioPropertyID_AudioStreamBasicDescription,
        sizeof(asbd),
        &asbd,
        0);

    if (err != qt::noErr)
    {
        throw Error(
            String_Format("MovieAudioExtractionGetProperty = %%").arg(err));
    }

    asbd.mFormatFlags =
        qt::kAudioFormatFlagIsSignedInteger |
        qt::kAudioFormatFlagIsPacked |
        qt::kAudioFormatFlagsNativeEndian;
    asbd.mBitsPerChannel = sizeof(qt::SInt16) * 8;
    asbd.mBytesPerFrame = sizeof(qt::SInt16) * asbd.mChannelsPerFrame;
    //asbd.mChannelsPerFrame = 2;
    asbd.mBytesPerPacket = asbd.mBytesPerFrame;

    err = qt::MovieAudioExtractionSetProperty(
        _qt_audio,
        qt::kQTPropertyClass_MovieAudioExtraction_Audio,
        qt::kQTMovieAudioExtractionAudioPropertyID_AudioStreamBasicDescription,
        sizeof(asbd),
        &asbd);

    if (err != qt::noErr)
    {
        throw Error(
            String_Format("MovieAudioExtractionSetProperty = %%").arg(err));
    }

    /*qt::Boolean discrete = true;
    err = qt::MovieAudioExtractionSetProperty(_qt_audio,
       qt::kQTPropertyClass_MovieAudioExtraction_Movie,
       qt::kQTMovieAudioExtractionMoviePropertyID_AllChannelsDiscrete,
      sizeof(qt::Boolean),
      &discrete
    );
    if (err != qt::noErr)
      throw Error(String_Format("MovieAudioExtractionSetProperty = %%").arg(err));*/

    _info_audio.rate = asbd.mSampleRate;
    _info_audio.channel = asbd.mChannelsPerFrame;
    DJV_DEBUG_PRINT("audio rate = " << _info_audio.rate);
    DJV_DEBUG_PRINT("audio channel = " << _info_audio.channel);

    // GWorld.

    unsigned long qt_format =
#if defined(DJV_OSX)
        qt::k32ARGBPixelFormat;
#else
        qt::k32RGBAPixelFormat;
#endif

    err = qt::NewGWorldFromPtr(
        &_qt_gworld,
        qt_format,
        &rect,
        0,
        0,
        0,
        (char *)_tmp_image.data(0, 0),
        _info.size.x * 4);

    if (err != qt::noErr)
    {
        throw Error(String_Format("NewGWorldFromPtr = %%").arg(err));
    }

    qt::SetMovieGWorld(_qt_movie, _qt_gworld, 0);
}

const Pixel_Data * djvAudioTestMovie::_Movie::image(int in) const
{
    //DJV_DEBUG("djvAudioTestMovie::_Movie::image");
    //DJV_DEBUG_PRINT("in = " << in);

    const qt::TimeValue time = in * _info_time.speed.duration;
    //DJV_DEBUG_PRINT("time = " << static_cast<int>(time));
    qt::SetMovieTimeValue(_qt_movie, time);
    qt::UpdateMovie(_qt_movie);
    qt::MoviesTask(_qt_movie, 0);

    //! \todo ARGB -> RGB

#if defined(DJV_OSX)

    uint32_t * p = (uint32_t *)_tmp_image.data();
    const uint32_t * const end = p + _info.size.x * _info.size.y;

    switch (Memory::endian())
    {
        case Memory::LSB:
            for (; p < end; ++p)
            {
                *p = (*p) >> 8;
            }

            break;

        case Memory::MSB:
            for (; p < end; ++p)
            {
                *p = (*p) << 8;
            }

            break;
    }

#endif

    return &_tmp_image;
}

void djvAudioTestMovie::_Movie::audio(int16_t * in, ulong size) const
{
    //DJV_DEBUG("djvAudioTestMovie::_Movie::audio");
    //DJV_DEBUG_PRINT("size = " << static_cast<int>(size));

    const int channel = _info_audio.channel;

    // Setup buffer.

    qt::UInt32 _size = size;
    qt::AudioBufferList data;
    data.mNumberBuffers = 1;
    data.mBuffers->mNumberChannels = channel;
    data.mBuffers->mDataByteSize = size * channel * sizeof(qt::SInt16);
    data.mBuffers->mData = in;

    // Fill buffer.

    qt::UInt32 flag = 0;
    qt::OSErr err = qt::MovieAudioExtractionFillBuffer(
        _qt_audio,
        &_size,
        &data,
        &flag);
    //if (err != qt::noErr)
    //  DJV_DEBUG_PRINT("MovieAudioExtractionFillBuffer = " << err);
    //DJV_DEBUG_PRINT("size = " << static_cast<int>(_size));

    // Zero remainder.

    in += _size * channel;

    for (qt::UInt32 i = _size; i < size; ++i, in += channel)
    {
        for (int c = 0; c < channel; ++c)
        {
            in[c] = 0;
        }
    }
}

void djvAudioTestMovie::_Movie::audio_seek(int in)
{
    DJV_DEBUG("djvAudioTestMovie::_Movie::audio_seek");
    DJV_DEBUG_PRINT("in = " << in);

    qt::TimeRecord time;
    time.scale = _info_time.speed.scale;
    time.base = 0;
    uint64_t t = Math::floor(
                     (in / Image_Speed::speed_to_float(_info_time.speed)) *
                     _info_time.speed.scale
                 );
    time.value.lo = t & 0xffffffff;
    time.value.hi = t >> 32;

    qt::OSErr err = qt::MovieAudioExtractionSetProperty(
        _qt_audio,
        qt::kQTPropertyClass_MovieAudioExtraction_Movie,
        qt::kQTMovieAudioExtractionMoviePropertyID_CurrentTime,
        sizeof(qt::TimeRecord),
        &time);

    if (err != qt::noErr)
    {
        DJV_DEBUG_PRINT("MovieAudioExtractionSetProperty = " << err);
    }
}

djvAudioTestMovie::Movie() :
    _p(new _Movie)
{}

djvAudioTestMovie::~Movie()
{}

void djvAudioTestMovie::init(const String & in) throw (Error)
{
    _p->init(in);
}

const Pixel_Data_Info & djvAudioTestMovie::info() const
{
    return _p->info();
}
const Image_Time & djvAudioTestMovie::info_time() const
{
    return _p->info_time();
}
const Audio_Info & djvAudioTestMovie::info_audio() const
{
    return _p->info_audio();
}

const Pixel_Data * djvAudioTestMovie::image(int in) const
{
    return _p->image(in);
}
void djvAudioTestMovie::audio(int16_t * in, ulong size) const
{
    _p->audio(in, size);
}
void djvAudioTestMovie::audio_seek(int in)
{
    _p->audio_seek(in);
}

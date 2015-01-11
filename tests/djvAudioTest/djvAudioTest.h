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

//! \file djvAudioTest.h

#ifndef DJV_AUDIO_TEST_H
#define DJV_AUDIO_TEST_H

#include <djv_application.h>
#include <djv_image_view.h>
#include <djv_slider.h>
#include <djv_tool_button.h>
#include <djv_window.h>

#include <djv_image_time.h>
#include <djv_timer.h>

#include <portaudio.h>

using namespace djv;

struct djvAudioTestInfo
{
    djvAudioTestInfo(int rate = 0, int channel = 0) :
        rate(rate),
        channel(channel)
    {}

    int rate;
    int channel;
};

class djvAudioTestBuffer
{
public:

    djvAudioTestBuffer(int chunk = 1024, int min = 100, int max = 1000);

    int16_t * push();
    const int16_t * pop() const;
    void clear();

    int chunk() const
    {
        return _chunk;
    }
    int chunk_bytes() const
    {
        return _chunk * sizeof(int16_t);
    }
    int min() const
    {
        return _min;
    }
    int max() const
    {
        return _max;
    }
    int size() const
    {
        return _size;
    }
    bool fill() const
    {
        return _size < _min;
    }

private:

    List<Memory_Buffer<int16_t> > _tmp;
    int _chunk, _min, _max;
    int _size, _in, _out;
};

class Audio
{
public:

    Audio();

    ~Audio();

    void init(const djvAudioTestInfo &, djvAudioTestBuffer *) throw (Error);
    void start();
    void stop();

    double time() const;

private:

    static int _callback(
        const void *,
        void *,
        ulong,
        const PaStreamCallbackTimeInfo *,
        PaStreamCallbackFlags,
        void *
    );

    int callback(
        const void *,
        void *, ulong,
        const PaStreamCallbackTimeInfo *,
        PaStreamCallbackFlags
    );

    PaStream * _h;
    djvAudioTestInfo _info;
    djvAudioTestBuffer * _buffer;
    double _time_start;
    static int _instance;
};

class djvAudioTestFactory
{
public:

    djvAudioTestFactory();

    ~djvAudioTestFactory();

    void init() throw (Error);
};

class Movie
{
public:

    Movie();

    ~Movie();

    void init(const String & in) throw (Error);

    const Pixel_Data_Info & info() const;
    const Image_Time & info_time() const;
    const djvAudioTestInfo & info_audio() const;

    const Pixel_Data * image(int) const;
    void audio(int16_t *, ulong) const;
    void audio_seek(int);

private:

    class _Movie;
    std::auto_ptr<_Movie> _p;
};

class Window : public djv::Window
{
public:

    Window(const String &);

    void idle();

private:

    void play(bool);

    DJV_CALLBACK(Window, play_callback, bool);
    DJV_CALLBACK(Window, frame_callback, int);
    DJV_CALLBACK(Window, close_callback, bool);

    void play_update();
    void frame_update();

    Movie _movie;
    djvAudioTestBuffer _audio_buffer;
    std::auto_ptr<Audio> _audio;
    bool _play;
    int _frame;
    bool _idle_init;
    djv::Timer _idle_timer;
    int _idle_frame;
    Image_View * _widget;
    Tool_Button * _play_widget;
    Int_Slider * _slider;
};

class Application : public djv::Application
{
public:

    Application(int argc, char ** argv);

    virtual ~Application();

    void window_add(const String &);
    void window_del(Window *);

    void idle_add(Window *);
    void idle_del(Window *);

private:

    DJV_FL_CALLBACK(Application, idle_callback);

    djvAudioTestFactory _audio_factory;
    List<Window *> _window;
    List<Window *> _idle;
};

//! Get the global application instance.

#define DJV_AUDIO_APP \
    static_cast<Application*>(djv::Core_Application::global())

#endif // DJV_AUDIO_TEST_H

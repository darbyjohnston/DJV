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

//! \file djvAudioTestWindow.cpp

//#define DJV_DEBUG

#include <djvAudioTest.h>

#include <djv_layout_row.h>

djvAudioTestWindow::djvAudioTestWindow(const String & in) :
    _play(false),
    _frame(0),
    _idle_init(false),
    _idle_frame(0)
{
    DJV_DEBUG(String_Format("djvAudioTestWindow::djvAudioTestWindow(%%)").
        arg(int64_t(this)));

    // Initialize.

    try
    {
        _movie.init(in);
    }
    catch (Error in)
    {
        DJV_AUDIO_APP->error(in);
    }

    _widget = new Image_View;
    _play_widget = new Tool_Button("playback_forward");
    _play_widget->type(Tool_Button::TOGGLE);
    _slider = new Int_Slider;

    Layout_V * layout = new Layout_V(this);
    layout->margin(0);
    layout->spacing(0);
    layout->add(_widget);
    layout->stretch(_widget);
    Layout_H * layout_h = new Layout_H(layout);
    layout_h->margin(0);
    layout_h->add(_play_widget);
    layout_h->add(_slider);
    layout_h->stretch(_slider);

    _slider->range(0, static_cast<int>(_movie.info_time().list.size()) - 1);

    dirty();
    size(_movie.info().size + V2i(0, size_min().y));

    frame_update();
    play_update();

    _play_widget->signal.set(this, play_callback);
    _slider->signal.set(this, frame_callback);
    close_signal.set(this, close_callback);

    show();
}

void djvAudioTestWindow::play(bool in)
{
    DJV_DEBUG("Window::play");
    DJV_DEBUG_PRINT("in = " << in);

    _play = in;

    play_update();
}

void djvAudioTestWindow::play_callback(bool in)
{
    play(in);
}

void djvAudioTestWindow::play_update()
{
    DJV_DEBUG(String_Format("djvAudioTestWindow::play_update(%%)").
        arg(int64_t(this)));
    DJV_DEBUG_PRINT("play = " << _play);

    callbacks(false);

    _play_widget->set(_play);

    if (_play)
    {
        _audio = std::auto_ptr<Audio>(new Audio);

        try
        {
            _audio->init(_movie.info_audio(), &_audio_buffer);
        }
        catch (Error in)
        {
            _audio.reset();
            DJV_AUDIO_APP->error(in);
        }

        _idle_init = false;

        DJV_AUDIO_APP->idle_add(this);
    }
    else
    {
        _audio.reset();
        _audio_buffer.clear();

        DJV_AUDIO_APP->idle_del(this);
    }

    callbacks(true);
}

void djvAudioTestWindow::frame_callback(int in)
{
    play(false);

    _frame = in;

    frame_update();
}

void djvAudioTestWindow::frame_update()
{
    DJV_DEBUG(String_Format("djvAudioTestWindow::frame_update(%%)").
        arg(int64_t(this)));
    DJV_DEBUG_PRINT("frame = " << _frame);

    callbacks(false);

    _slider->set(_frame);

    _widget->set(_movie.image(_frame));
    _widget->redraw();

    callbacks(true);
}

void djvAudioTestWindow::idle()
{
    DJV_DEBUG(String_Format("djvAudioTestWindow::idle(%%)").arg(int64_t(this)));

    if (_audio.get())
    {
        int size = _audio_buffer.size();
        DJV_DEBUG_PRINT("audio size = " << size);

        if (! size)
        {
            // Reset audio.

            _audio->stop();
            _audio_buffer.clear();
            _movie.audio_seek(_frame);
            _idle_init = false;
        }

        // Fill audio buffer.

        const int min = _audio_buffer.min();

        do
        {
            int16_t * p = _audio_buffer.push();

            if (! p)
            {
                break;
            }

            _movie.audio(
                p,
                _audio_buffer.chunk() / _movie.info_audio().channel);
        }
        while (++size < min)
            ;
    }

    if (! _idle_init)
    {
        DJV_DEBUG_PRINT("init");

        if (_audio.get())
        {
            _audio->start();
        }

        _idle_timer.start();
        _idle_frame = 0;
        _idle_init = true;
    }
    else
    {
        _idle_timer.check();
    }

    const double time = _audio.get() ? _audio->time() : _idle_timer.seconds();
    DJV_DEBUG_PRINT("time = " << time);

    if (time > 0.0)
    {
        const int64_t absolute_frame = Math::floor(time *
            Image_Speed::speed_to_float(_movie.info_time().speed));
        int inc = absolute_frame - _idle_frame;
        _idle_frame = absolute_frame;
        DJV_DEBUG_PRINT("absolute frame = " << absolute_frame);
        DJV_DEBUG_PRINT("inc = " << inc);

        if (inc)
        {
            _frame += inc;

            if (_frame >= static_cast<int>(_movie.info_time().list.size()))
            {
                _frame = 0;

                if (_audio.get())
                {
                    _audio->stop();
                }

                _audio_buffer.clear();

                _idle_init = false;
            }

            frame_update();
        }
    }
}

void djvAudioTestWindow::close_callback(bool)
{
    DJV_AUDIO_APP->window_del(this);
}

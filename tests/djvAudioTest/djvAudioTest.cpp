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

//! \file djvAudioTest.cpp

//#define DJV_DEBUG

#include <djvAudioTest.h>

#include <algorithm>

djvAudioTestBuffer::djvAudioTestBuffer(int chunk, int min, int max) :
    _chunk(chunk),
    _min(min),
    _max(max),
    _size(0),
    _in(0),
    _out(0)
{
    _tmp.resize(_max);

    for (int i = 0; i < _max; ++i)
    {
        _tmp[i].size(_chunk);
    }
}

int16_t * djvAudioTestBuffer::push()
{
    if (_size >= _max)
    {
        return 0;
    }

    int16_t * out = _tmp[_in]();
    _in = (_in + 1) % _max;
    ++_size;
    return out;
}

const int16_t * djvAudioTestBuffer::pop() const
{
    djvAudioTestBuffer * that = const_cast<djvAudioTestBuffer *>(this);

    if (! _size)
    {
        return 0;
    }

    const int16_t * out = _tmp[_out]();
    that->_out = (_out + 1) % _max;
    --that->_size;
    return out;
}

void djvAudioTestBuffer::clear()
{
    _size = _in = _out = 0;
}

Audio::Audio() :
    _h(0),
    _time_start(0.0)
{
    DJV_DEBUG("Audio::Audio");
}

Audio::~Audio()
{
    DJV_DEBUG("Audio::~Audio");

    if (_h)
    {
        Pa_CloseStream(_h);
    }
}

void Audio::init(const djvAudioTestInfo & info, djvAudioTestBuffer * buffer)
    throw (Error)
{
    DJV_DEBUG("Audio::init");

    _info = info;
    _buffer = buffer;

    // Open.

    PaError error = Pa_OpenDefaultStream(
        &_h,
        0,
        _info.channel,
        paInt16,
        _info.rate,
        //paFramesPerBufferUnspecified,
        _info.channel ? (_buffer->chunk() / _info.channel) : 0,
        _callback,
        this);

    if (error != paNoError)
    {
        throw Error(Pa_GetErrorText(error));
    }
}

void Audio::start()
{
    DJV_DEBUG("Audio::start");

    Pa_StartStream(_h);
    _time_start = Pa_GetStreamTime(_h);
}

void Audio::stop()
{
    DJV_DEBUG("Audio::stop");

    //Pa_StopStream(_h);
    Pa_AbortStream(_h);
}

double Audio::time() const
{
    return
        Pa_GetStreamTime(_h) - _time_start -
        Pa_GetStreamInfo(_h)->outputLatency;
}

int Audio::_callback(
    const void * in ,
    void * out,
    ulong size,
    const PaStreamCallbackTimeInfo * time_info,
    PaStreamCallbackFlags flag,
    void * data
)
{
    return static_cast<Audio *>(data)->callback(in, out, size, time_info, flag);
}

int Audio::callback(
    const void * in,
    void * out,
    ulong size,
    const PaStreamCallbackTimeInfo * time_info,
    PaStreamCallbackFlags flag)
{
    DJV_DEBUG("Audio::callback");
    DJV_DEBUG_PRINT("size = " << static_cast<int>(size));

    const int16_t * p = _buffer->pop();

    if (! p)
    {
        DJV_DEBUG_PRINT("xrun");
        Memory::zero(out, _buffer->chunk_bytes());
        //abort();
    }
    else
    {
        Memory::copy(p, out, _buffer->chunk_bytes());
    }

    return 0;
}

djvAudioTestFactory::djvAudioTestFactory()
{
    DJV_DEBUG("djvAudioTestFactory::djvAudioTestFactory");
}

djvAudioTestFactory::~djvAudioTestFactory()
{
    DJV_DEBUG("djvAudioTestFactory::~djvAudioTestFactory");

    Pa_Terminate();
}

void djvAudioTestFactory::init() throw (Error)
{
    DJV_DEBUG("djvAudioTestFactory::init");

    PaError error = Pa_Initialize();

    if (error != paNoError)
    {
        throw Error(Pa_GetErrorText(error));
    }
}

djvAudioTestApplication::djvAudioTestApplication(int argc, char ** argv) :
    djv::Application("djvAudioTest", argc, argv)
{
    DJV_DEBUG("djvAudioTestApplication::djvAudioTestApplication");

    // Command line.

    if (argc < 2)
    {
        print("Usage: djvAudioTest (input)...");
        
        exit(EXIT_ERROR);
        
        return;
    }

    // Initialize.

    try
    {
        _audio_factory.init();
    }
    catch (Error in)
    {
        error(in);
    }

    is_valid(true);

    for (int i = 1; i < argc; ++i)
    {
        window_add(argv[i]);
    }
}

djvAudioTestApplication::~djvAudioTestApplication()
{
    DJV_DEBUG("djvAudioTestApplication::~djvAudioTestApplication");

    Fl::remove_timeout(idle_callback, this);

    DJV_DEBUG_PRINT("window = " << uint(_window.size()));

    for (size_t i = 0; i < _window.size(); ++i)
    {
        _window[i]->del();
    }
}

void djvAudioTestApplication::window_add(const String & in)
{
    DJV_DEBUG("djvAudioTestApplication::window_add");
    DJV_DEBUG_PRINT("in = " << in);

    _window += new Window(in);
}

void djvAudioTestApplication::window_del(Window * in)
{
    DJV_DEBUG("djvAudioTestApplication::window_del");

    idle_del(in);
    in->del();
    _window.erase(std::find(_window.begin(), _window.end(), in));
}

const double idle_timeout = 0.001;

void djvAudioTestApplication::idle_add(Window * in)
{
    DJV_DEBUG("djvAudioTestApplication::idle_add");

    _idle += in;

    if (1 == _idle.size())
    {
        Fl::add_timeout(idle_timeout, idle_callback, this);
    }
}

void djvAudioTestApplication::idle_del(Window * in)
{
    DJV_DEBUG("djvAudioTestApplication::idle_del");

    List<Window *>::iterator i = std::find(_idle.begin(), _idle.end(), in);

    if (i != _idle.end())
    {
        _idle.erase(i);
    }

    if (! _idle.size())
    {
        Fl::remove_timeout(idle_callback, this);
    }
}

void djvAudioTestApplication::idle_callback()
{
    DJV_DEBUG("djvAudioTestApplication::idle_callback");

    for (size_t i = 0; i < _idle.size(); ++i)
    {
        _idle[i]->idle();
    }

    Fl::add_timeout(idle_timeout, idle_callback, this);
}

int main(int argc, char ** argv)
{
    return djvAudioTestApplication(argc, argv).run();
}

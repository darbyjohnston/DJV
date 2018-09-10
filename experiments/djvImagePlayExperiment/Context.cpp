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

#include <Context.h>

#include <Load.h>

#include <djvCore/Debug.h>

//------------------------------------------------------------------------------
// Context
//------------------------------------------------------------------------------

Context::Context(QObject * parent) :
    djvUIContext(parent),
    _load(0)
{
    //DJV_DEBUG("Context::Context");
    
    _load = new Load(this);
    _load->moveToThread(&_thread);

    _playback.reset(new Playback);
    //_playback->moveToThread(&_thread);

    _load->connect(
        _playback.data(),
        SIGNAL(frameChanged(qint64)),
        SLOT(read(qint64)));
    _load->connect(
        &_thread,
        SIGNAL(started()),
        SLOT(start()));
}

Context::~Context()
{
    _thread.quit();
    _thread.wait();
}

Load * Context::load() const
{
    return _load;
}

Playback * Context::playback() const
{
    return _playback.data();
}

QThread * Context::thread()
{
    return &_thread;
}

QMutex * Context::mutex()
{
    return &_mutex;
}


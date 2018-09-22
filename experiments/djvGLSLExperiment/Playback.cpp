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

#include <Playback.h>

#include <djvCore/Math.h>

//------------------------------------------------------------------------------
// Playback
//------------------------------------------------------------------------------

Playback::Playback(QObject * parent) :
    QObject(parent),
    _frame   (0),
    _playback(djvPlaybackUtil::STOP),
    _timerId (0)
{
    playbackUpdate();
}

Playback::~Playback()
{
    if (_timerId)
    {
        killTimer(_timerId);
        _timerId = 0;
    }
}

const djvSequence & Playback::sequence() const
{
    return _sequence;
}

qint64 Playback::frame() const
{
    return _frame < _sequence.frames.size() ? _sequence.frames[_frame] : 0;
}

djvPlaybackUtil::PLAYBACK Playback::playback() const
{
    return _playback;
}

void Playback::setSequence(const djvSequence & sequence)
{
    if (sequence == _sequence)
        return;
    _sequence = sequence;
    playbackUpdate();
    Q_EMIT sequenceChanged(_sequence);
}

void Playback::setFrame(qint64 frame)
{
    qint64 f = djvMath::wrap<qint64>(frame, 0, _sequence.frames.count() - 1);
    if (f == _frame)
        return;
    _frame = f;
    Q_EMIT frameChanged(_frame);
}

void Playback::setPlayback(djvPlaybackUtil::PLAYBACK playback)
{
    if (playback == _playback)
        return;
    _playback = playback;
    playbackUpdate();
    Q_EMIT playbackChanged(_playback);
}

void Playback::timerEvent(QTimerEvent *)
{
    //DJV_DEBUG("Playback::timerEvent");
    //DJV_DEBUG_PRINT("frame = " << _frame);
    int inc = 0;
    switch (_playback)
    {
        case djvPlaybackUtil::FORWARD: inc =  1; break;
        case djvPlaybackUtil::REVERSE: inc = -1; break;
        default: break;
    }
    setFrame(_frame + inc);
}

void Playback::playbackUpdate()
{
    if (_timerId)
    {
        killTimer(_timerId);
        _timerId = 0;
    }
    switch (_playback)
    {
        case djvPlaybackUtil::FORWARD:
        case djvPlaybackUtil::REVERSE:
            _timerId = startTimer(1000 * 1 / djvSpeed::speedToFloat(_sequence.speed));
            break;
        default: break;
    }
}

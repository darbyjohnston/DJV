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

#include <djvImagePlay2TestPlayback.h>

#include <djvDebug.h>
#include <djvMath.h>

//------------------------------------------------------------------------------
// djvImagePlay2TestPlayback
//------------------------------------------------------------------------------

djvImagePlay2TestPlayback::djvImagePlay2TestPlayback(QObject * parent) :
    QObject(parent),
    _playback(djvPlaybackUtil::STOP),
    _frame   (0),
    _timer   (0)
{
    playbackUpdate();
}

djvImagePlay2TestPlayback::~djvImagePlay2TestPlayback()
{
    if (_timer)
    {
        killTimer(_timer);
        _timer = 0;
    }
}

const djvFrameList & djvImagePlay2TestPlayback::frameList() const
{
    return _frameList;
}

const djvSpeed & djvImagePlay2TestPlayback::speed() const
{
    return _speed;
}

qint64 djvImagePlay2TestPlayback::frame() const
{
    return _frame >= 0 && _frame < _frameList.count() ? _frameList[_frame] : 0;
}
    
djvPlaybackUtil::PLAYBACK djvImagePlay2TestPlayback::playback() const
{
    return _playback;
}

void djvImagePlay2TestPlayback::setFrameList(const djvFrameList & frameList)
{
    if (frameList == _frameList)
        return;
    _frameList = frameList;
    Q_EMIT frameListChanged(_frameList);
}

void djvImagePlay2TestPlayback::setSpeed(const djvSpeed & speed)
{
    if (speed == _speed)
        return;
    _speed = speed;
    Q_EMIT speedChanged(_speed);
}

void djvImagePlay2TestPlayback::setFrame(qint64 frame)
{
    if (frame == _frame)
        return;
    //DJV_DEBUG("djvImagePlay2TestPlayback::setFrame");
    //DJV_DEBUG_PRINT("frame = " << frame);
    _frame = frame;
    Q_EMIT frameChanged(_frame);
}

void djvImagePlay2TestPlayback::setPlayback(djvPlaybackUtil::PLAYBACK playback)
{
    if (playback == _playback)
        return;
    _playback = playback;
    playbackUpdate();
    Q_EMIT playbackChanged(_playback);
}

void djvImagePlay2TestPlayback::timerEvent(QTimerEvent *)
{
    qint64 frame = _frame;
    switch (_playback)
    {
        case djvPlaybackUtil::REVERSE: --frame; break;
        case djvPlaybackUtil::FORWARD: ++frame; break;
        default: break;
    }
    setFrame(frame);
}

void djvImagePlay2TestPlayback::playbackUpdate()
{
    switch (_playback)
    {
        case djvPlaybackUtil::STOP:
            if (_timer)
            {
                killTimer(_timer);
                _timer = 0;
            }
        
            break;
        case djvPlaybackUtil::REVERSE:
        case djvPlaybackUtil::FORWARD:
            _timer = startTimer(0);
            break;
        default: break;
    }
}

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

#pragma once

#ifndef DJV_IMAGE_PLAY2_TEST_PLAYBACK_H
#define DJV_IMAGE_PLAY2_TEST_PLAYBACK_H

#include <djvPlaybackUtil.h>

#include <djvSequence.h>

#include <QObject>

//------------------------------------------------------------------------------
// djvImagePlay2TestPlayback
//------------------------------------------------------------------------------

class djvImagePlay2TestPlayback : public QObject
{
    Q_OBJECT

public:

    explicit djvImagePlay2TestPlayback(QObject * parent = nullptr);
    
    virtual ~djvImagePlay2TestPlayback();

    const djvFrameList & frameList() const;
    
    const djvSpeed & speed() const;

    qint64 frame() const;
    
    djvPlaybackUtil::PLAYBACK playback() const;

public Q_SLOTS:

    void setFrameList(const djvFrameList &);
    
    void setSpeed(const djvSpeed &);

    void setFrame(qint64);
    
    void setPlayback(djvPlaybackUtil::PLAYBACK);
    
Q_SIGNALS:

    void frameListChanged(const djvFrameList &);
    
    void speedChanged(const djvSpeed &);

    void frameChanged(qint64);

    void playbackChanged(djvPlaybackUtil::PLAYBACK);
    
protected:

    virtual void timerEvent(QTimerEvent *);

private Q_SLOTS:

    void playbackUpdate();

private:

    djvFrameList              _frameList;
    djvSpeed                  _speed;
    qint64                    _frame;
    djvPlaybackUtil::PLAYBACK _playback;
    int                       _timer;
};

#endif // DJV_IMAGE_PLAY2_TEST_PLAYBACK_H

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

#include <PlaybackToolBar.h>

#include <Context.h>

#include <djvViewMiscWidget.h>

#include <djvPlaybackButtons.h>

#include <QHBoxLayout>

//------------------------------------------------------------------------------
// PlaybackToolBar
//------------------------------------------------------------------------------

PlaybackToolBar::PlaybackToolBar(
    Playback * playback,
    Context *  context,
    QWidget * parent) :
    QToolBar(parent),
    _playback(playback),
    _buttons(0),
    _slider (0)
{
    QWidget * widget = new QWidget;
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    _buttons = new djvPlaybackButtons(context);
    
    _slider = new djvViewFrameSlider(context);
    
    QHBoxLayout * layout = new QHBoxLayout(widget);
    layout->setMargin(5);
    layout->setSpacing(5);
    layout->addWidget(_buttons);
    layout->addWidget(_slider);
    
    addWidget(widget);
    setMovable(false);
    setFloatable(false);
    
    _buttons->setPlayback(playback->playback());
    
    _slider->setFrameList(playback->sequence().frames);
    _slider->setSpeed(playback->sequence().speed);
    
    _slider->connect(
        _playback,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
    _playback->connect(
        _buttons,
        SIGNAL(playbackChanged(djvPlaybackUtil::PLAYBACK)),
        SLOT(setPlayback(djvPlaybackUtil::PLAYBACK)));
    _playback->connect(
        _slider,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
}

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

#include <PlayBar.h>

#include <Context.h>
#include <Load.h>
#include <Playback.h>

#include <djvViewLib/MiscWidget.h>

#include <djvUI/PlaybackButtons.h>

#include <QGridLayout>
#include <QHBoxLayout>

//------------------------------------------------------------------------------
// PlayBar
//------------------------------------------------------------------------------

PlayBar::PlayBar(Context * context) :
    _playbackButtons(0),
    _frameWidget    (0),
    _frameSlider    (0),
    _context        (context)
{
    _playbackButtons = new djvPlaybackButtons(context);

    _frameWidget = new djvViewFrameWidget(context);

    _frameSlider = new djvViewFrameSlider(context);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->addWidget(_playbackButtons);
    layout->addWidget(_frameWidget);
    layout->addWidget(_frameSlider);

    updateSequence();
    updateFrame();
    updatePlayback();

    connect(
        context->playback(),
        SIGNAL(frameListChanged(const djvFrameList &)),
        SLOT(updateSequence()));
    connect(
        context->playback(),
        SIGNAL(speedChanged(const djvSpeed &)),
        SLOT(updateSequence()));
    connect(
        context->playback(),
        SIGNAL(frameChanged(qint64)),
        SLOT(updateFrame()));
    connect(
        context->playback(),
        SIGNAL(playbackChanged(djvPlaybackUtil::PLAYBACK)),
        SLOT(updatePlayback()));
    context->playback()->connect(
        _playbackButtons,
        SIGNAL(playbackChanged(djvPlaybackUtil::PLAYBACK)),
        SLOT(setPlayback(djvPlaybackUtil::PLAYBACK)));
    context->playback()->connect(
        _frameWidget,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
    context->playback()->connect(
        _frameSlider,
        SIGNAL(frameChanged(qint64)),
        SLOT(setFrame(qint64)));
    connect(
        _frameSlider,
        SIGNAL(pressed(bool)),
        SLOT(sliderPressedCallback(bool)));
}

void PlayBar::sliderPressedCallback(bool pressed)
{
    _context->load()->setEveryFrame(! pressed);
}

void PlayBar::updateSequence()
{
    _frameWidget->setFrameList(_context->playback()->frameList());
    _frameWidget->setSpeed(_context->playback()->speed());
    _frameSlider->setFrameList(_context->playback()->frameList());
    _frameSlider->setSpeed(_context->playback()->speed());
}

void PlayBar::updateFrame()
{
    _frameWidget->setFrame(_context->playback()->frame());
    _frameSlider->setFrame(_context->playback()->frame());
}

void PlayBar::updatePlayback()
{
    _playbackButtons->setPlayback(_context->playback()->playback());
}

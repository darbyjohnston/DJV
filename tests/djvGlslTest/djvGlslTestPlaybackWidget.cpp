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

//! \file djvGlslTestPlaybackWidget.cpp

#include <djvGlslTestPlaybackWidget.h>

#include <djvGlslTestContext.h>
#include <djvGlslTestPlaybackButtons.h>

#include <djvIntEditSlider.h>

#include <QHBoxLayout>

//------------------------------------------------------------------------------
// djvGlslTestPlaybackWidget
//------------------------------------------------------------------------------

djvGlslTestPlaybackWidget::djvGlslTestPlaybackWidget(
    djvGlslTestPlayback * playback,
    djvGlslTestContext *  context,
    QWidget *             parent) :
    QWidget(parent),
    _playback(playback),
    _buttons(0),
    _slider (0)
{
    _buttons = new djvGlslTestPlaybackButtons(context);
    
    _slider = new djvIntEditSlider(context);
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(5);
    layout->addWidget(_buttons);
    layout->addWidget(_slider);
    
    _buttons->setPlayback(playback->playback());
    
    _slider->setRange(playback->start(), playback->end());
    _slider->setValue(playback->frame());
    
    _playback->connect(
        _buttons,
        SIGNAL(playbackChanged(djvGlslTestPlayback::PLAYBACK)),
        SLOT(setPlayback(djvGlslTestPlayback::PLAYBACK)));
    
    connect(
        _slider,
        SIGNAL(valueChanged(int)),
        SLOT(sliderCallback(int)));
}

void djvGlslTestPlaybackWidget::sliderCallback(int value)
{
    _playback->setFrame(value);
}

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

//! \file djvGlslTestPlaybackButtons.cpp

#include <djvGlslTestPlaybackButtons.h>

#include <djvGlslTestContext.h>

#include <djvIconLibrary.h>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QToolButton>

//------------------------------------------------------------------------------
// djvGlslTestPlaybackButtons
//------------------------------------------------------------------------------

djvGlslTestPlaybackButtons::djvGlslTestPlaybackButtons(
    djvGlslTestContext * context,
    QWidget *            parent) :
    QWidget(parent),
    _playback   (djvGlslTestPlayback::STOP),
    _buttonGroup(0)
{
    _buttonGroup = new QButtonGroup(this);
    _buttonGroup->setExclusive(true);
    
    const QStringList icons = QStringList() <<
        "djvPlayReverseIcon.png" <<
        "djvPlayStopIcon.png" <<
        "djvPlayForwardIcon.png";
    
    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    for (int i = 0; i < icons.count(); ++i)
    {
        QToolButton * button = new QToolButton;
        button->setCheckable(true);
        button->setIcon(context->iconLibrary()->icon(icons[i]));
        button->setIconSize(context->iconLibrary()->defaultSize());
        button->setAutoRaise(true);
        
        _buttonGroup->addButton(button, i);

        layout->addWidget(button);
    }
    
    _buttonGroup->buttons()[_playback]->setChecked(true);
    
    connect(
        _buttonGroup,
        SIGNAL(buttonClicked(int)),
        SLOT(buttonCallback(int)));
}

djvGlslTestPlayback::PLAYBACK djvGlslTestPlaybackButtons::playback() const
{
    return _playback;
}

void djvGlslTestPlaybackButtons::setPlayback(djvGlslTestPlayback::PLAYBACK playback)
{
    if (playback == _playback)
        return;
    
    _playback = playback;
    
    _buttonGroup->buttons()[_playback]->setChecked(true);
    
    Q_EMIT playbackChanged(_playback);
}

void djvGlslTestPlaybackButtons::buttonCallback(int id)
{
    setPlayback(static_cast<djvGlslTestPlayback::PLAYBACK>(id));
}


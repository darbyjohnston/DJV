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

//! \file djvToolButtonTest.cpp

#include <djvToolButtonTest.h>

#include <djvChoiceButton.h>
#include <djvIconLibrary.h>
#include <djvShuttleButton.h>
#include <djvToolButton.h>

#include <djvSystem.h>

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

QString djvToolButtonTest::name()
{
    return "djvToolButtonTest";
}

void djvToolButtonTest::run(const QStringList & args)
{
    QWidget * window = new QWidget;
    
    djvToolButton * reverseButton = new djvToolButton;
    reverseButton->setCheckable(true);
    reverseButton->setIconSize(djvIconLibrary::global()->defaultSize());
    reverseButton->setIcon(
        djvIconLibrary::global()->icon("djvPlayReverseIcon.png"));
    
    djvToolButton * stopButton = new djvToolButton;
    stopButton->setCheckable(true);
    stopButton->setIconSize(djvIconLibrary::global()->defaultSize());
    stopButton->setIcon(
        djvIconLibrary::global()->icon("djvPlayStopIcon.png"));

    djvToolButton * forwardButton = new djvToolButton;
    forwardButton->setCheckable(true);
    forwardButton->setIconSize(djvIconLibrary::global()->defaultSize());
    forwardButton->setIcon(
        djvIconLibrary::global()->icon("djvPlayForwardIcon.png"));
    
    QButtonGroup * buttonGroup = new QButtonGroup(window);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(reverseButton, REVERSE);
    buttonGroup->addButton(stopButton, STOP);
    buttonGroup->addButton(forwardButton, FORWARD);

    djvShuttleButton * shuttle = new djvShuttleButton(djvShuttleButton::iconsDefault());

    const QStringList loopLabel = QStringList() <<
        "Once" <<
        "Repeat" <<
        "PongPong";

    const QList<QIcon> loopIcon = QList<QIcon>() <<
        djvIconLibrary::global()->icon("djvPlayLoopOnceIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayLoopRepeatIcon.png") <<
        djvIconLibrary::global()->icon("djvPlayLoopPingPongIcon.png");

    QActionGroup * loopActionGroup = new QActionGroup(this);
    loopActionGroup->setExclusive(true);

    for (int i = 0; i < loopLabel.count(); ++i)
        new QAction(loopIcon[i], loopLabel[i], loopActionGroup);

    djvChoiceButton * choiceButton = new djvChoiceButton(loopActionGroup);

    djvToolButton * disabledToolButton = new djvToolButton;
    disabledToolButton->setIconSize(djvIconLibrary::global()->defaultSize());
    disabledToolButton->setIcon(
        djvIconLibrary::global()->icon("djvPlayReverseIcon.png"));
    disabledToolButton->setEnabled(false);

    djvShuttleButton * disabledShuttle = new djvShuttleButton(djvShuttleButton::iconsDefault());
    disabledShuttle->setEnabled(false);

    djvChoiceButton * disabledChoiceButton = new djvChoiceButton(loopActionGroup);
    disabledChoiceButton->setEnabled(false);
    
    QVBoxLayout * layout = new QVBoxLayout(window);
    
    QHBoxLayout * hLayout = new QHBoxLayout;

    QHBoxLayout * hLayout2 = new QHBoxLayout;
    hLayout2->setMargin(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(reverseButton);
    hLayout2->addWidget(stopButton);
    hLayout2->addWidget(forwardButton);
    hLayout->addLayout(hLayout2);
    hLayout->addWidget(shuttle);
    hLayout->addWidget(choiceButton);
    layout->addLayout(hLayout);

    hLayout = new QHBoxLayout;
    hLayout->addWidget(disabledToolButton);
    hLayout->addWidget(disabledShuttle);
    hLayout->addWidget(disabledChoiceButton);
    layout->addLayout(hLayout);

    buttonGroup->buttons()[STOP]->setChecked(true);
    
    connect(
        buttonGroup,
        SIGNAL(buttonToggled(int, bool)),
        SLOT(playbackCallback(int, bool)));
    
    connect(
        shuttle,
        SIGNAL(valueChanged(int)),
        SLOT(shuttleCallback(int)));
    
    connect(
        choiceButton,
        SIGNAL(currentIndexChanged(int)),
        SLOT(choiceCallback(int)));
    
    window->show();
}

void djvToolButtonTest::playbackCallback(int index, bool checked)
{
    djvSystem::print(QString("%1").arg(index));
}

void djvToolButtonTest::shuttleCallback(int value)
{
    djvSystem::print(QString("%1").arg(value));
}

void djvToolButtonTest::choiceCallback(int index)
{
    djvSystem::print(QString("%1").arg(index));
}

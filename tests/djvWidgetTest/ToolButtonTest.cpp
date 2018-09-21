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

#include <djvWidgetTest/ToolButtonTest.h>

#include <djvUI/ChoiceButton.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/ShuttleButton.h>
#include <djvUI/ToolButton.h>
#include <djvUI/UIContext.h>

#include <djvCore/System.h>

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace djv
{
    namespace WidgetTest
    {
        ToolButtonTest::ToolButtonTest(UI::UIContext * context) :
            AbstractWidgetTest(context)
        {}

        QString ToolButtonTest::name()
        {
            return "ToolButtonTest";
        }

        void ToolButtonTest::run(const QStringList & args)
        {
            QWidget * window = new QWidget;
      
            UI::ToolButton * reverseButton = new UI::ToolButton(context());
            reverseButton->setCheckable(true);
            reverseButton->setIcon(context()->iconLibrary()->icon("djvPlayReverseIcon"));

            UI::ToolButton * stopButton = new UI::ToolButton(context());
            stopButton->setCheckable(true);
            stopButton->setIcon(context()->iconLibrary()->icon("djvPlayStopIcon"));

            UI::ToolButton * forwardButton = new UI::ToolButton(context());
            forwardButton->setCheckable(true);
            forwardButton->setIcon(context()->iconLibrary()->icon("djvPlayForwardIcon"));

            QButtonGroup * buttonGroup = new QButtonGroup(window);
            buttonGroup->setExclusive(true);
            buttonGroup->addButton(reverseButton, REVERSE);
            buttonGroup->addButton(stopButton, STOP);
            buttonGroup->addButton(forwardButton, FORWARD);

            UI::ShuttleButton * shuttle = new UI::ShuttleButton(context());

            const QStringList loopLabel = QStringList() <<
                "Once" <<
                "Repeat" <<
                "PongPong";
            const QList<QIcon> loopIcon = QList<QIcon>() <<
                context()->iconLibrary()->icon("djvPlayLoopOnceIcon") <<
                context()->iconLibrary()->icon("djvPlayLoopRepeatIcon") <<
                context()->iconLibrary()->icon("djvPlayLoopPingPongIcon");
            QActionGroup * loopActionGroup = new QActionGroup(this);
            loopActionGroup->setExclusive(true);
            for (int i = 0; i < loopLabel.count(); ++i)
                new QAction(loopIcon[i], loopLabel[i], loopActionGroup);
            UI::ChoiceButton * choiceButton = new UI::ChoiceButton(loopActionGroup, context());

            UI::ToolButton * disabledToolButton = new UI::ToolButton(context());
            disabledToolButton->setIcon(context()->iconLibrary()->icon("djvPlayReverseIcon"));
            disabledToolButton->setEnabled(false);

            UI::ShuttleButton * disabledShuttle = new UI::ShuttleButton(context());
            disabledShuttle->setEnabled(false);

            UI::ChoiceButton * disabledChoiceButton = new UI::ChoiceButton(loopActionGroup, context());
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

        void ToolButtonTest::playbackCallback(int index, bool checked)
        {
            Core::System::print(QString("%1").arg(index));
        }

        void ToolButtonTest::shuttleCallback(int value)
        {
            Core::System::print(QString("%1").arg(value));
        }

        void ToolButtonTest::choiceCallback(int index)
        {
            Core::System::print(QString("%1").arg(index));
        }

    } // namespace WidgetTest
} // namespace djv

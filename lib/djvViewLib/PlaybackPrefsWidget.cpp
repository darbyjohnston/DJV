//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvViewLib/PlaybackPrefsWidget.h>

#include <djvViewLib/PlaybackPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/PrefsGroupBox.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QPointer>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackPrefsWidget::Private
        {
            QPointer<QCheckBox> autoStartWidget;
            QPointer<QComboBox> loopWidget;
            QPointer<QCheckBox> everyFrameWidget;
            QPointer<QComboBox> layoutWidget;
        };

        PlaybackPrefsWidget::PlaybackPrefsWidget(const QPointer<ViewContext> & context) :
            AbstractPrefsWidget(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Playback"), context),
            _p(new Private)
        {
            // Create the widgets.
            _p->autoStartWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Start playback when opening files"));

            _p->loopWidget = new QComboBox;
            _p->loopWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->loopWidget->addItems(Enum::loopLabels());

            _p->everyFrameWidget = new QCheckBox(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Play every frame"));

            _p->layoutWidget = new QComboBox;
            _p->layoutWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            _p->layoutWidget->addItems(Enum::layoutLabels());

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Playback"), context.data());
            auto formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(_p->autoStartWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Loop mode:"),
                _p->loopWidget);
            formLayout->addRow(_p->everyFrameWidget);
            layout->addWidget(prefsGroupBox);

            prefsGroupBox = new UI::PrefsGroupBox(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Layout"), context.data());
            formLayout = prefsGroupBox->createLayout();
            formLayout->addRow(
                qApp->translate("djv::ViewLib::PlaybackPrefsWidget", "Playback controls:"),
                _p->layoutWidget);
            layout->addWidget(prefsGroupBox);

            layout->addStretch();

            // Initialize.
            widgetUpdate();

            // Setup the callbacks.
            connect(
                _p->autoStartWidget,
                SIGNAL(toggled(bool)),
                SLOT(autoStartCallback(bool)));
            connect(
                _p->loopWidget,
                SIGNAL(activated(int)),
                SLOT(loopCallback(int)));
            connect(
                _p->everyFrameWidget,
                SIGNAL(toggled(bool)),
                SLOT(everyFrameCallback(bool)));
            connect(
                _p->layoutWidget,
                SIGNAL(activated(int)),
                SLOT(layoutCallback(int)));
            connect(
                context->playbackPrefs(),
                SIGNAL(prefChanged()),
                SLOT(widgetUpdate()));
        }

        PlaybackPrefsWidget::~PlaybackPrefsWidget()
        {}

        void PlaybackPrefsWidget::resetPreferences()
        {
            context()->playbackPrefs()->setAutoStart(PlaybackPrefs::autoStartDefault());
            context()->playbackPrefs()->setLoop(PlaybackPrefs::loopDefault());
            context()->playbackPrefs()->setEveryFrame(PlaybackPrefs::everyFrameDefault());
            context()->playbackPrefs()->setLayout(PlaybackPrefs::layoutDefault());
        }

        void PlaybackPrefsWidget::autoStartCallback(bool in)
        {
            context()->playbackPrefs()->setAutoStart(in);
        }

        void PlaybackPrefsWidget::loopCallback(int in)
        {
            context()->playbackPrefs()->setLoop(static_cast<Enum::LOOP>(in));
        }

        void PlaybackPrefsWidget::everyFrameCallback(bool in)
        {
            context()->playbackPrefs()->setEveryFrame(in);
        }

        void PlaybackPrefsWidget::layoutCallback(int in)
        {
            context()->playbackPrefs()->setLayout(static_cast<Enum::LAYOUT>(in));
        }

        void PlaybackPrefsWidget::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->autoStartWidget <<
                _p->loopWidget <<
                _p->everyFrameWidget <<
                _p->layoutWidget);
            _p->autoStartWidget->setChecked(
                context()->playbackPrefs()->hasAutoStart());
            _p->loopWidget->setCurrentIndex(
                context()->playbackPrefs()->loop());
            _p->everyFrameWidget->setChecked(
                context()->playbackPrefs()->hasEveryFrame());
            _p->layoutWidget->setCurrentIndex(
                context()->playbackPrefs()->layout());
        }

    } // namespace ViewLib
} // namespace djv

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

#include <djvUI/PlaybackButtons.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/UIContext.h>

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QToolButton>

namespace djv
{
    namespace UI
    {
        PlaybackButtons::PlaybackButtons(UIContext * context, QWidget * parent) :
            QWidget(parent)
        {
            _buttonGroup = new QButtonGroup(this);
            _buttonGroup->setExclusive(true);

            const QStringList toolTips = QStringList() <<
                qApp->translate("djv::UI::PlaybackButtons", "Reverse playback") <<
                qApp->translate("djv::UI::PlaybackButtons", "Stop playback") <<
                qApp->translate("djv::UI::PlaybackButtons", "Forward playback");

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);

            for (int i = 0; i < PlaybackUtil::PLAYBACK_COUNT; ++i)
            {
                QToolButton * button = new QToolButton;
                button->setCheckable(true);
                button->setAutoRaise(true);
                button->setToolTip(toolTips[i]);
                _buttonGroup->addButton(button, i);
                layout->addWidget(button);
            }

            styleUpdate();
            _buttonGroup->buttons()[_playback]->setChecked(true);

            connect(
                _buttonGroup,
                SIGNAL(buttonClicked(int)),
                SLOT(buttonCallback(int)));
        }

        PlaybackUtil::PLAYBACK PlaybackButtons::playback() const
        {
            return _playback;
        }

        void PlaybackButtons::setPlayback(PlaybackUtil::PLAYBACK playback)
        {
            if (playback == _playback)
                return;
            _playback = playback;
            _buttonGroup->buttons()[_playback]->setChecked(true);
            Q_EMIT playbackChanged(_playback);
        }

        bool PlaybackButtons::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void PlaybackButtons::buttonCallback(int id)
        {
            setPlayback(static_cast<PlaybackUtil::PLAYBACK>(id));
        }

        void PlaybackButtons::styleUpdate()
        {
            const QList<QAbstractButton *> & buttons = _buttonGroup->buttons();
            for (int i = 0; i < buttons.size(); ++i)
            {
                buttons[i]->setIcon(_context->iconLibrary()->icon(PlaybackUtil::playbackIcons()[i]));
            }
        }

    } // namespace UI
} // namespace djv

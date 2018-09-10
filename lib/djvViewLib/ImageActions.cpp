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

#include <djvViewLib/ImageActions.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/Util.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Shortcut.h>

#include <djvGraphics/OpenGLImage.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>

namespace djv
{
    namespace ViewLib
    {
        struct ImageActions::Private
        {};

        ImageActions::ImageActions(
            Context * context,
            QObject * parent) :
            AbstractActions(context, parent),
            _p(new Private)
        {
            // Create the actions.
            for (int i = 0; i < ACTION_COUNT; ++i)
            {
                _actions[i] = new QAction(this);
            }
            _actions[FRAME_STORE]->setText(
                qApp->translate("djv::ViewLib::ImageActions", "Show &Frame Store"));
            _actions[FRAME_STORE]->setCheckable(true);
            _actions[LOAD_FRAME_STORE]->setText(
                qApp->translate("djv::ViewLib::ImageActions", "&Load Frame Store"));
            _actions[MIRROR_H]->setText(
                qApp->translate("vImageActions", "Mirror &Horizontal"));
            _actions[MIRROR_H]->setCheckable(true);
            _actions[MIRROR_V]->setText(
                qApp->translate("djv::ViewLib::ImageActions", "Mirror &Vertical"));
            _actions[MIRROR_V]->setCheckable(true);
            _actions[COLOR_PROFILE]->setText(
                qApp->translate("djv::ViewLib::ImageActions", "&Color Profile"));
            _actions[COLOR_PROFILE]->setCheckable(true);
            _actions[DISPLAY_PROFILE_VISIBLE]->setCheckable(true);
            _actions[DISPLAY_PROFILE_VISIBLE]->setText(
                qApp->translate("djv::ViewLib::ImageActions", "Show Display Profile"));
            _actions[DISPLAY_PROFILE_VISIBLE]->setIcon(context->iconLibrary()->icon(
                "djvDisplayProfileIcon.png"));

            // Create the action groups.
            for (int i = 0; i < GROUP_COUNT; ++i)
            {
                _groups[i] = new QActionGroup(this);
            }
            _groups[SCALE_GROUP]->setExclusive(true);
            for (int i = 0; i < Util::imageScaleLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(Util::imageScaleLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[SCALE_GROUP]->addAction(action);
            }
            _groups[ROTATE_GROUP]->setExclusive(true);
            for (int i = 0; i < Util::imageRotateLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(Util::imageRotateLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[ROTATE_GROUP]->addAction(action);
            }
            _groups[CHANNEL_GROUP]->setExclusive(true);
            for (int i = 0; i < djvOpenGLImageOptions::channelLabels().count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(djvOpenGLImageOptions::channelLabels()[i]);
                action->setCheckable(true);
                action->setData(i);
                _groups[CHANNEL_GROUP]->addAction(action);
            }

            // Initialize.
            update();

            // Setup the callbacks.
            connect(
                context->imagePrefs(),
                SIGNAL(displayProfilesChanged(const QVector<djv::ViewLib::DisplayProfile> &)),
                SLOT(update()));
            connect(
                context->shortcutPrefs(),
                SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
                SLOT(update()));
        }

        ImageActions::~ImageActions()
        {}

        void ImageActions::update()
        {
            const QVector<djvShortcut> & shortcuts =
                context()->shortcutPrefs()->shortcuts();

            // Update the actions.
            _actions[FRAME_STORE]->setShortcut(
                shortcuts[Util::SHORTCUT_IMAGE_FRAME_STORE].value);
            _actions[LOAD_FRAME_STORE]->setShortcut(
                shortcuts[Util::SHORTCUT_IMAGE_FRAME_STORE_LOAD].value);
            _actions[MIRROR_H]->setShortcut(
                shortcuts[Util::SHORTCUT_IMAGE_MIRROR_HORIZONTAL].value);
            _actions[MIRROR_V]->setShortcut(
                shortcuts[Util::SHORTCUT_IMAGE_MIRROR_VERTICAL].value);
            _actions[COLOR_PROFILE]->setShortcut(
                shortcuts[Util::SHORTCUT_IMAGE_COLOR_PROFILE].value);

            // Update the action groups.
            const QVector<QKeySequence> scaleShortcuts = QVector<QKeySequence>() <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_NONE].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_16_9].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_1_0].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_1_33].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_1_78].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_1_85].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_2_0].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_2_35].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_2_39].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_2_40].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_1_1].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_2_1].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_3_2].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_4_3].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_5_3].value <<
                shortcuts[Util::SHORTCUT_IMAGE_SCALE_5_4].value;
            for (int i = 0; i < Util::imageScaleLabels().count(); ++i)
            {
                _groups[SCALE_GROUP]->actions()[i]->setShortcut(scaleShortcuts[i]);
            }

            const QVector<QKeySequence> rotateShortcuts = QVector<QKeySequence>() <<
                shortcuts[Util::SHORTCUT_IMAGE_ROTATE_0].value <<
                shortcuts[Util::SHORTCUT_IMAGE_ROTATE_90].value <<
                shortcuts[Util::SHORTCUT_IMAGE_ROTATE_180].value <<
                shortcuts[Util::SHORTCUT_IMAGE_ROTATE_270].value;
            for (int i = 0; i < Util::imageRotateLabels().count(); ++i)
            {
                _groups[ROTATE_GROUP]->actions()[i]->setShortcut(rotateShortcuts[i]);
            }
            Q_FOREACH(QAction * action, _groups[DISPLAY_PROFILE_GROUP]->actions())
                delete action;

            QStringList displayProfileNames;
            displayProfileNames += qApp->translate("djv::ViewLib::ImageActions", "Reset");
            displayProfileNames += context()->imagePrefs()->displayProfileNames();

            const QVector<QKeySequence> displayProfileShortcuts = QVector<QKeySequence>() <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_RESET].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_1].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_2].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_3].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_4].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_5].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_6].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_7].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_8].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_9].value <<
                shortcuts[Util::SHORTCUT_IMAGE_DISPLAY_PROFILE_10].value;
            for (int i = 0; i < displayProfileNames.count(); ++i)
            {
                QAction * action = new QAction(this);
                action->setText(displayProfileNames[i]);
                if (i < displayProfileShortcuts.count())
                    action->setShortcut(displayProfileShortcuts[i]);
                action->setData(i);
                _groups[DISPLAY_PROFILE_GROUP]->addAction(action);
            }

            const QVector<QKeySequence> channelShortcuts = QVector<QKeySequence>() <<
                QKeySequence() <<
                shortcuts[Util::SHORTCUT_IMAGE_CHANNEL_RED].value <<
                shortcuts[Util::SHORTCUT_IMAGE_CHANNEL_GREEN].value <<
                shortcuts[Util::SHORTCUT_IMAGE_CHANNEL_BLUE].value <<
                shortcuts[Util::SHORTCUT_IMAGE_CHANNEL_ALPHA].value;
            for (int i = 0; i < djvOpenGLImageOptions::channelLabels().count(); ++i)
            {
                _groups[CHANNEL_GROUP]->actions()[i]->setShortcut(channelShortcuts[i]);
            }

            // Fix up the actions.
            osxMenuHack();

            // Emit changed signal.
            Q_EMIT changed();
        }

    } // namespace ViewLib
} // namespace djv

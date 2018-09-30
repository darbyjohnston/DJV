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

#include <djvViewLib/ImageMenu.h>

#include <djvViewLib/ImageActions.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct ImageMenu::Private
        {
            QPointer<QMenu> displayProfileMenu;
        };

        ImageMenu::ImageMenu(
            const QPointer<AbstractActions> & actions,
            QWidget * parent) :
            AbstractMenu(actions, parent),
            _p(new Private)
        {
            // Create the menus.
            addAction(actions->action(ImageActions::SHOW_FRAME_STORE));
            addAction(actions->action(ImageActions::SET_FRAME_STORE));

            addSeparator();

            addAction(actions->action(ImageActions::MIRROR_H));
            addAction(actions->action(ImageActions::MIRROR_V));

            auto scaleMenu = addMenu(
                qApp->translate("djv::ViewLib::ImageMenu", "&Scale"));
            scaleMenu->addActions(
                actions->group(ImageActions::SCALE_GROUP)->actions());

            auto rotateMenu = addMenu(
                qApp->translate("djv::ViewLib::ImageMenu", "&Rotate"));
            rotateMenu->addActions(
                actions->group(ImageActions::ROTATE_GROUP)->actions());

            addSeparator();

            addAction(actions->action(ImageActions::COLOR_PROFILE));

            addAction(actions->action(ImageActions::DISPLAY_PROFILE_VISIBLE));

            _p->displayProfileMenu = addMenu(
                qApp->translate("djv::ViewLib::ImageMenu", "&Display Profile"));

            addSeparator();

            auto channelMenu = addMenu(
                qApp->translate("djv::ViewLib::ImageMenu", "C&hannel"));
            channelMenu->addActions(
                actions->group(ImageActions::CHANNEL_GROUP)->actions());

            // Initialize.
            setTitle(qApp->translate("djv::ViewLib::ImageMenu", "&Image"));
            menuUpdate();
        }

        ImageMenu::~ImageMenu()
        {}

        void ImageMenu::menuUpdate()
        {
            _p->displayProfileMenu->clear();
            _p->displayProfileMenu->addActions(actions()->group(ImageActions::DISPLAY_PROFILE_GROUP)->actions());
        }

    } // namespace ViewLib
} // namespace djv

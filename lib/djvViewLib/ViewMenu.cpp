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

#include <djvViewLib/ViewMenu.h>

#include <djvViewLib/ViewActions.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        ViewMenu::ViewMenu(
            const QPointer<AbstractActions> & actions,
            QWidget * parent) :
            AbstractMenu(actions, parent)
        {
            addAction(actions->action(ViewActions::LEFT));
            addAction(actions->action(ViewActions::RIGHT));
            addAction(actions->action(ViewActions::UP));
            addAction(actions->action(ViewActions::DOWN));
            addAction(actions->action(ViewActions::CENTER));
            addAction(actions->action(ViewActions::ZOOM_IN));
            addAction(actions->action(ViewActions::ZOOM_OUT));
            addAction(actions->action(ViewActions::ZOOM_RESET));
            addAction(actions->action(ViewActions::RESET));
            addAction(actions->action(ViewActions::FIT));
            addSeparator();
            auto gridMenu = addMenu(qApp->translate("djv::ViewLib::ViewMenu", "&Grid"));
            Q_FOREACH(QAction * action, actions->group(ViewActions::GRID_GROUP)->actions())
                gridMenu->addAction(action);
            addAction(actions->action(ViewActions::HUD));

            setTitle(qApp->translate("djv::ViewLib::ViewMenu", "&View"));
        }

        ViewMenu::~ViewMenu()
        {}

    } // namespace ViewLib
} // namespace djv

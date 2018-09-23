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

#include <djvViewLib/FileMenu.h>

#include <djvViewLib/FileActions.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        struct FileMenu::Private
        {
            QPointer<QMenu> recent;
            QPointer<QMenu> layer;
        };

        FileMenu::FileMenu(
            const QPointer<AbstractActions> & actions,
            QWidget * parent) :
            AbstractMenu(actions, parent),
            _p(new Private)
        {
            // Create the menus.
            addAction(actions->action(FileActions::OPEN));
            _p->recent = addMenu(qApp->translate("djv::ViewLib::FileMenu", "&Recent"));
            addAction(actions->action(FileActions::RELOAD));
            addAction(actions->action(FileActions::RELOAD_FRAME));
            addAction(actions->action(FileActions::CLOSE));
            addAction(actions->action(FileActions::SAVE));
            addAction(actions->action(FileActions::SAVE_FRAME));
            addSeparator();
            _p->layer = addMenu(qApp->translate("djv::ViewLib::FileMenu", "La&yer"));
            addAction(actions->action(FileActions::LAYER_PREV));
            addAction(actions->action(FileActions::LAYER_NEXT));
            auto proxyMenu = addMenu(qApp->translate("djv::ViewLib::FileMenu", "Pro&xy Scale"));
            Q_FOREACH(QAction * action, actions->group(FileActions::PROXY_GROUP)->actions())
                proxyMenu->addAction(action);
            addAction(actions->action(FileActions::U8_CONVERSION));
            addAction(actions->action(FileActions::CACHE));
            addAction(actions->action(FileActions::PRELOAD));
            addAction(actions->action(FileActions::CLEAR_CACHE));
            addSeparator();
            addAction(actions->action(FileActions::MESSAGES));
            addAction(actions->action(FileActions::PREFS));
            addAction(actions->action(FileActions::DEBUG_LOG));
            addSeparator();
            addAction(actions->action(FileActions::EXIT));

            // Initialize.
            setTitle(qApp->translate("djv::ViewLib::FileMenu", "&File"));
            menuUpdate();
        }

        FileMenu::~FileMenu()
        {}

        void FileMenu::menuUpdate()
        {
            _p->recent->clear();
            Q_FOREACH(QAction * action, actions()->group(FileActions::RECENT_GROUP)->actions())
                _p->recent->addAction(action);
            _p->layer->clear();
            Q_FOREACH(QAction * action, actions()->group(FileActions::LAYER_GROUP)->actions())
                _p->layer->addAction(action);
        }

    } // namespace ViewLib
} // namespace djv

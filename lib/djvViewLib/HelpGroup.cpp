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

#include <djvViewLib/HelpGroup.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/HelpActions.h>
#include <djvViewLib/HelpMenu.h>
#include <djvViewLib/MainWindow.h>

#include <djvUI/AboutDialog.h>
#include <djvUI/InfoDialog.h>

#include <djvCore/Debug.h>
#include <djvCore/FileInfoUtil.h>

#include <QAction>
#include <QMenuBar>

namespace djv
{
    namespace ViewLib
    {
        struct HelpGroup::Private
        {
            HelpActions * actions = nullptr;
            HelpMenu *    menu = nullptr;
        };

        HelpGroup::HelpGroup(
            const HelpGroup * copy,
            MainWindow *      mainWindow,
            Context *         context) :
            AbstractGroup(mainWindow, context),
            _p(new Private)
        {
            //DJV_DEBUG("HelpGroup::HelpGroup");

            // Create the actions.
            _p->actions = new HelpActions(context, this);

            // Create the menus.
            _p->menu = new HelpMenu(_p->actions, mainWindow->menuBar());

            mainWindow->menuBar()->addMenu(_p->menu);

            // Setup the action callbacks.
            connect(
                _p->actions->action(HelpActions::HELP),
                SIGNAL(triggered()),
                SLOT(helpCallback()));
            connect(
                _p->actions->action(HelpActions::INFO),
                SIGNAL(triggered()),
                SLOT(infoCallback()));
            connect(
                _p->actions->action(HelpActions::ABOUT),
                SIGNAL(triggered()),
                SLOT(aboutCallback()));
        }

        HelpGroup::~HelpGroup()
        {
            //DJV_DEBUG("HelpGroup::~HelpGroup");
        }

        void HelpGroup::helpCallback()
        {
            //DJV_DEBUG("HelpGroup::helpCallback");
            context()->help();
        }

        void HelpGroup::infoCallback()
        {
            context()->infoDialog()->show();
        }

        void HelpGroup::aboutCallback()
        {
            context()->aboutDialog()->show();
        }

    } // namespace ViewLib
} // namespace djv

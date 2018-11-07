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

#include <djvViewLib/ViewGroup.h>

#include <djvViewLib/ImageView.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewActions.h>
#include <djvViewLib/ViewContext.h>
#include <djvViewLib/ViewMenu.h>
#include <djvViewLib/ViewPrefs.h>
#include <djvViewLib/ViewToolBar.h>

#include <djvUI/FloatEdit.h>

#include <djvCore/ListUtil.h>

namespace djv
{
    namespace ViewLib
    {
        struct ViewGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                grid(context->viewPrefs()->grid()),
                hudEnabled(context->viewPrefs()->isHudEnabled())
            {}

            Enum::GRID grid;
            bool       hudEnabled;

            QPointer<ViewActions> actions;
        };

        ViewGroup::ViewGroup(
            const QPointer<ViewGroup> & copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            if (copy)
            {
                _p->grid = copy->_p->grid;
                _p->hudEnabled = copy->_p->hudEnabled;
            }

            // Create the actions.
            _p->actions = new ViewActions(context, this);

            // Initialize.
            update();

            // Setup the action callbacks.
            connect(
                _p->actions->action(ViewActions::LEFT),
                SIGNAL(triggered()),
                SLOT(leftCallback()));
            connect(
                _p->actions->action(ViewActions::RIGHT),
                SIGNAL(triggered()),
                SLOT(rightCallback()));
            connect(
                _p->actions->action(ViewActions::UP),
                SIGNAL(triggered()),
                SLOT(upCallback()));
            connect(
                _p->actions->action(ViewActions::DOWN),
                SIGNAL(triggered()),
                SLOT(downCallback()));
            connect(
                _p->actions->action(ViewActions::CENTER),
                SIGNAL(triggered()),
                SLOT(centerCallback()));
            connect(
                _p->actions->action(ViewActions::ZOOM_IN),
                SIGNAL(triggered()),
                SLOT(zoomInCallback()));
            connect(
                _p->actions->action(ViewActions::ZOOM_OUT),
                SIGNAL(triggered()),
                SLOT(zoomOutCallback()));
            connect(
                _p->actions->action(ViewActions::ZOOM_RESET),
                SIGNAL(triggered()),
                SLOT(zoomResetCallback()));
            connect(
                _p->actions->action(ViewActions::RESET),
                SIGNAL(triggered()),
                SLOT(resetCallback()));
            connect(
                _p->actions->action(ViewActions::FIT),
                SIGNAL(triggered()),
                SLOT(fitCallback()));
            connect(
                _p->actions->action(ViewActions::HUD),
                SIGNAL(toggled(bool)),
                SLOT(hudEnabledCallback(bool)));

            // Setup the action group callbacks.
            connect(
                _p->actions->group(ViewActions::GRID_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(gridCallback(QAction *)));

            // Setup the preferences callbacks.
            connect(
                context->viewPrefs(),
                SIGNAL(gridChanged(djv::ViewLib::Enum::GRID)),
                SLOT(gridCallback(djv::ViewLib::Enum::GRID)));
            connect(
                context->viewPrefs(),
                SIGNAL(hudEnabledChanged(bool)),
                SLOT(hudEnabledCallback(bool)));
        }

        ViewGroup::~ViewGroup()
        {}

        djv::ViewLib::Enum::GRID ViewGroup::grid() const
        {
            return _p->grid;
        }

        bool ViewGroup::isHudEnabled() const
        {
            return _p->hudEnabled;
        }

        QPointer<QMenu> ViewGroup::createMenu() const
        {
            return new ViewMenu(_p->actions.data());
        }

        QPointer<QToolBar> ViewGroup::createToolBar() const
        {
            auto toolBar = new ViewToolBar(_p->actions.data(), context());
            auto viewWidget = session()->viewWidget();
            toolBar->setZoom(viewWidget->viewZoom());
            connect(
                toolBar,
                SIGNAL(zoomChanged(float)),
                SLOT(zoomCallback(float)));
            connect(
                viewWidget,
                SIGNAL(viewZoomChanged(float)),
                toolBar,
                SLOT(setZoom(float)));
            return toolBar;
        }

        void ViewGroup::leftCallback()
        {
            viewMove(glm::ivec2(-10, 0));
        }

        void ViewGroup::rightCallback()
        {
            viewMove(glm::ivec2(10, 0));
        }

        void ViewGroup::upCallback()
        {
            viewMove(glm::ivec2(0, 10));
        }

        void ViewGroup::downCallback()
        {
            viewMove(glm::ivec2(0, -10));
        }

        void ViewGroup::centerCallback()
        {
            session()->viewWidget()->viewCenter();
        }

        void ViewGroup::resetCallback()
        {
            session()->viewWidget()->setViewPosZoom(glm::ivec2(0, 0), 1.f);
        }

        void ViewGroup::zoomCallback(float value)
        {
            session()->viewWidget()->setZoomFocus(value);
        }

        void ViewGroup::zoomInCallback()
        {
            viewZoom(2.f);
        }

        void ViewGroup::zoomOutCallback()
        {
            viewZoom(0.5);
        }

        void ViewGroup::zoomIncCallback()
        {
            viewZoom(1.1f);
        }

        void ViewGroup::zoomDecCallback()
        {
            viewZoom(0.9f);
        }

        void ViewGroup::zoomResetCallback()
        {
            session()->viewWidget()->setZoomFocus(1.f);
        }

        void ViewGroup::fitCallback()
        {
            session()->viewWidget()->viewFit();
        }

        void ViewGroup::gridCallback(Enum::GRID grid)
        {
            _p->grid = grid;
            update();
        }

        void ViewGroup::gridCallback(QAction * action)
        {
            gridCallback(static_cast<Enum::GRID>(action->data().toInt()));
        }

        void ViewGroup::hudEnabledCallback(bool value)
        {
            _p->hudEnabled = value;
            update();
        }

        void ViewGroup::update()
        {
            _p->actions->action(ViewActions::HUD)->setChecked(_p->hudEnabled);
            _p->actions->group(ViewActions::GRID_GROUP)->actions()[_p->grid]->setChecked(true);
            auto viewWidget = session()->viewWidget();
            viewWidget->setHudEnabled(_p->hudEnabled);
            viewWidget->setGrid(_p->grid);
        }

        void ViewGroup::viewMove(const glm::ivec2 & offset)
        {
            auto viewWidget = session()->viewWidget();
            viewWidget->setViewPos(viewWidget->viewPos() + offset);
        }

        void ViewGroup::viewZoom(float zoom)
        {
            //DJV_DEBUG("ViewGroup::viewZoom");
            //DJV_DEBUG_PRINT("zoom = " << zoom);
            auto viewWidget = session()->viewWidget();
            viewWidget->setZoomFocus(viewWidget->viewZoom() * zoom);
        }

    } // namespace ViewLib
} // namespace djv

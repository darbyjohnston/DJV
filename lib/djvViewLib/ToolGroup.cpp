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

#include <djvViewLib/ToolGroup.h>

#include <djvViewLib/ColorPickerTool.h>
#include <djvViewLib/HistogramTool.h>
#include <djvViewLib/InfoTool.h>
#include <djvViewLib/MagnifyTool.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ToolActions.h>
#include <djvViewLib/ToolMenu.h>
#include <djvViewLib/ToolToolBar.h>

#include <QDockWidget>
#include <QEvent>
#include <QLabel>

namespace djv
{
    namespace ViewLib
    {
        struct ToolGroup::Private
        {
            QMap<djv::ViewLib::Enum::TOOL, bool> toolsVisible;
            QPointer<ToolActions>                actions;
            QPointer<MagnifyTool>                magnifyTool;
            QPointer<ColorPickerTool>            colorPickerTool;
            QPointer<HistogramTool>              histogramTool;
            QPointer<InfoTool>                   infoTool;
            QVector<QPointer<QDockWidget> >      dockWidgets;
        };

        ToolGroup::ToolGroup(
            const QPointer<ToolGroup> & copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private)
        {
            //DJV_DEBUG("ToolGroup::ToolGroup");

            // Create the actions.
            _p->actions = new ToolActions(context, this);

            _p->magnifyTool = new MagnifyTool(session, context);
            _p->colorPickerTool = new ColorPickerTool(session, context);
            _p->histogramTool = new HistogramTool(session, context);
            _p->infoTool = new InfoTool(session, context);

            QList<QWidget *> widgets = QList<QWidget *>() <<
                _p->magnifyTool <<
                _p->colorPickerTool <<
                _p->histogramTool <<
                _p->infoTool;
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                auto dockWidget = new QDockWidget(Enum::toolLabels()[i]);
                dockWidget->setWidget(widgets[i]);
                _p->dockWidgets.append(dockWidget);
            }

            // Initialize.
            if (copy)
            {
                _p->toolsVisible = copy->_p->toolsVisible;
            }
            update();

            // Setup the action group callbacks.
            //! \bug Why isn't QActionGroup::triggered() firing in all cases?
            //connect(
            //    _p->actions->group(ToolActions::TOOL_GROUP),
            //    SIGNAL(triggered(QAction *)),
            //    SLOT(toolsVisibleCallback(QAction *)));
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                connect(
                    _p->actions->group(ToolActions::TOOL_GROUP)->actions()[i],
                    SIGNAL(toggled(bool)),
                    SLOT(toolsVisibleCallback()));
            }

            // Setup widget callbacks.
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                _p->actions->group(ToolActions::TOOL_GROUP)->actions()[i]->connect(
                    _p->dockWidgets[i],
                    SIGNAL(visibilityChanged(bool)),
                    SLOT(setChecked(bool)));
            }
        }

        ToolGroup::~ToolGroup()
        {
            //DJV_DEBUG("ToolGroup::~ToolGroup");
        }

        const QMap<Enum::TOOL, bool> & ToolGroup::toolsVisible() const
        {
            return _p->toolsVisible;
        }

        QVector<QPointer<QDockWidget> > ToolGroup::dockWidgets() const
        {
            return _p->dockWidgets;
        }

        QPointer<QMenu> ToolGroup::createMenu() const
        {
            return new ToolMenu(_p->actions.data());
        }

        QPointer<QToolBar> ToolGroup::createToolBar() const
        {
            return new ToolToolBar(_p->actions.data(), context());
        }

        void ToolGroup::setToolsVisible(const QMap<Enum::TOOL, bool> & value)
        {
            if (value == _p->toolsVisible)
                return;
            _p->toolsVisible = value;
            update();
            Q_EMIT toolsVisibleChanged(_p->toolsVisible);
        }

        void ToolGroup::toolsVisibleCallback()
        {
            QMap<Enum::TOOL, bool> visible;
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                visible[static_cast<Enum::TOOL>(i)] = _p->actions->group(ToolActions::TOOL_GROUP)->actions()[i]->isChecked();
            }
            setToolsVisible(visible);
        }

        void ToolGroup::update()
        {
            for (int i = 0; i < Enum::TOOL_COUNT; ++i)
            {
                const auto tool = static_cast<Enum::TOOL>(i);
                _p->actions->group(ToolActions::TOOL_GROUP)->actions()[i]->setChecked(_p->toolsVisible[tool]);
                _p->dockWidgets[i]->setVisible(_p->toolsVisible[tool]);
            }
        }

    } // namespace ViewLib
} // namespace djv

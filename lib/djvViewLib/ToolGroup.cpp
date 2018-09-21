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

#include <djvViewLib/ToolGroup.h>

#include <djvViewLib/ColorPickerTool.h>
#include <djvViewLib/HistogramTool.h>
#include <djvViewLib/InfoTool.h>
#include <djvViewLib/MagnifyTool.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/ToolActions.h>
#include <djvViewLib/ToolMenu.h>
#include <djvViewLib/ToolToolBar.h>

#include <QAbstractButton>
#include <QAction>
#include <QDockWidget>
#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>

namespace djv
{
    namespace ViewLib
    {
        struct ToolGroup::Private
        {
            Private() :
                tools(Util::TOOL_COUNT, false)
            {}

            QVector<bool>     tools;
            ToolActions *     actions = nullptr;
            ToolMenu *        menu = nullptr;
            ToolToolBar *     toolBar = nullptr;
            MagnifyTool *     magnifyTool = nullptr;
            ColorPickerTool * colorPickerTool = nullptr;
            HistogramTool *   histogramTool = nullptr;
            InfoTool *        infoTool = nullptr;
            QDockWidget *     dockWidgets[Util::TOOL_COUNT];
        };

        ToolGroup::ToolGroup(
            const ToolGroup * copy,
            MainWindow *      mainWindow,
            Context *         context) :
            AbstractGroup(mainWindow, context),
            _p(new Private)
        {
            //DJV_DEBUG("ToolGroup::ToolGroup");

            // Create the actions.
            _p->actions = new ToolActions(context, this);

            // Create the menus.
            _p->menu = new ToolMenu(_p->actions, mainWindow->menuBar());
            mainWindow->menuBar()->addMenu(_p->menu);

            // Create the widgets.
            _p->toolBar = new ToolToolBar(_p->actions, context);

            mainWindow->addToolBar(_p->toolBar);

            _p->magnifyTool = new MagnifyTool(mainWindow, context);
            _p->colorPickerTool = new ColorPickerTool(mainWindow, context);
            _p->histogramTool = new HistogramTool(mainWindow, context);
            _p->infoTool = new InfoTool(mainWindow, context);

            QList<QWidget *> widgets = QList<QWidget *>() <<
                _p->magnifyTool <<
                _p->colorPickerTool <<
                _p->histogramTool <<
                _p->infoTool;

            QList<Qt::DockWidgetArea> areas = QList<Qt::DockWidgetArea>() <<
                Qt::LeftDockWidgetArea <<
                Qt::LeftDockWidgetArea <<
                Qt::RightDockWidgetArea <<
                Qt::RightDockWidgetArea;

            for (int i = 0; i < Util::TOOL_COUNT; ++i)
            {
                _p->dockWidgets[i] = new QDockWidget(Util::toolLabels()[i]);
                _p->dockWidgets[i]->setWidget(widgets[i]);

                mainWindow->addDockWidget(areas[i], _p->dockWidgets[i]);
            }

            // Initialize.
            if (copy)
            {
                _p->tools = copy->_p->tools;
            }
            update();

            // Setup the action group callbacks.
            connect(
                _p->actions->group(ToolActions::TOOL_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(toolsCallback(QAction *)));

            // Setup widget callbacks.
            for (int i = 0; i < Util::TOOL_COUNT; ++i)
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

        const QVector<bool> & ToolGroup::tools() const
        {
            return _p->tools;
        }

        QToolBar * ToolGroup::toolBar() const
        {
            return _p->toolBar;
        }

        void ToolGroup::setTools(const QVector<bool> & tools)
        {
            if (tools == _p->tools)
                return;
            _p->tools = tools;
            update();
            Q_EMIT toolsChanged(_p->tools);
        }

        void ToolGroup::toolsCallback(QAction *)
        {
            QVector<bool> tools;
            for (int i = 0; i < Util::TOOL_COUNT; ++i)
            {
                tools += _p->actions->group(ToolActions::TOOL_GROUP)->
                    actions()[i]->isChecked();
            }
            setTools(tools);
        }

        void ToolGroup::update()
        {
            // Update action groups.
            for (int i = 0; i < Util::TOOL_COUNT; ++i)
            {
                _p->actions->group(ToolActions::TOOL_GROUP)->
                    actions()[i]->setChecked(_p->tools[i]);
            }

            // Update widgets.
            for (int i = 0; i < Util::TOOL_COUNT; ++i)
            {
                _p->dockWidgets[i]->setVisible(_p->tools[i]);
            }
        }

    } // namespace ViewLib
} // namespace djv

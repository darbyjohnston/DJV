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

//! \file djvViewToolGroup.cpp

#include <djvViewToolGroup.h>

#include <djvViewColorPickerTool.h>
#include <djvViewHistogramTool.h>
#include <djvViewInfoTool.h>
#include <djvViewMagnifyTool.h>
#include <djvViewMainWindow.h>
#include <djvViewToolActions.h>
#include <djvViewToolMenu.h>
#include <djvViewToolToolBar.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>

//------------------------------------------------------------------------------
// djvViewToolGroup::P
//------------------------------------------------------------------------------

struct djvViewToolGroup::P
{
    P() :
        tools          (djvViewUtil::TOOL_COUNT, false),
        actions        (0),
        menu           (0),
        toolBar        (0),
        magnifyTool    (0),
        colorPickerTool(0),
        histogramTool  (0),
        infoTool       (0)
    {}
    
    QVector<bool>            tools;
    
    djvViewToolActions *     actions;
    djvViewToolMenu *        menu;
    djvViewToolToolBar *     toolBar;
    djvViewMagnifyTool *     magnifyTool;
    djvViewColorPickerTool * colorPickerTool;
    djvViewHistogramTool *   histogramTool;
    djvViewInfoTool *        infoTool;
    QDockWidget *            dockWidgets[djvViewUtil::TOOL_COUNT];
};

//------------------------------------------------------------------------------
// djvViewToolGroup
//------------------------------------------------------------------------------

djvViewToolGroup::djvViewToolGroup(
    djvViewMainWindow *      mainWindow,
    const djvViewToolGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    //DJV_DEBUG("djvViewToolGroup::djvViewToolGroup");
    
    // Create the actions.

    _p->actions = new djvViewToolActions(this);
    
    // Create the menus.

    _p->menu = new djvViewToolMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.

    _p->toolBar = new djvViewToolToolBar(_p->actions);

    mainWindow->addToolBar(_p->toolBar);

    _p->magnifyTool = new djvViewMagnifyTool(mainWindow);

    _p->colorPickerTool = new djvViewColorPickerTool(mainWindow);

    _p->histogramTool = new djvViewHistogramTool(mainWindow);

    _p->infoTool = new djvViewInfoTool(mainWindow);

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
    
    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        _p->dockWidgets[i] = new QDockWidget(djvViewUtil::toolLabels()[i]);
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
        _p->actions->group(djvViewToolActions::TOOL_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(toolsCallback(QAction *)));

    // Setup widget callbacks.
    
    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        _p->actions->group(djvViewToolActions::TOOL_GROUP)->actions()[i]->connect(
            _p->dockWidgets[i],
            SIGNAL(visibilityChanged(bool)),
            SLOT(setChecked(bool)));
    }
}

djvViewToolGroup::~djvViewToolGroup()
{
    //DJV_DEBUG("djvViewToolGroup::~djvViewToolGroup");

    delete _p;
}
    
const QVector<bool> & djvViewToolGroup::tools() const
{
    return _p->tools;
}

QToolBar * djvViewToolGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewToolGroup::setTools(const QVector<bool> & tools)
{
    if (tools == _p->tools)
        return;

    _p->tools = tools;
    
    update();
    
    Q_EMIT toolsChanged(_p->tools);
}

void djvViewToolGroup::toolsCallback(QAction *)
{
    QVector<bool> tools;
    
    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        tools += _p->actions->group(djvViewToolActions::TOOL_GROUP)->
            actions()[i]->isChecked();
    }

    setTools(tools);
}

void djvViewToolGroup::update()
{
    // Update action groups.

    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        _p->actions->group(djvViewToolActions::TOOL_GROUP)->
            actions()[i]->setChecked(_p->tools[i]);
    }
    
    // Update widgets.

    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        _p->dockWidgets[i]->setVisible(_p->tools[i]);
    }
}

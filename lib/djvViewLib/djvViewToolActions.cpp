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

//! \file djvViewToolActions.cpp

#include <djvViewToolActions.h>

#include <djvViewShortcutPrefs.h>
#include <djvViewUtil.h>

#include <djvIconLibrary.h>
#include <djvShortcut.h>

#include <QAction>
#include <QActionGroup>

//------------------------------------------------------------------------------
// djvViewToolActions::P
//------------------------------------------------------------------------------

struct djvViewToolActions::P
{
};

//------------------------------------------------------------------------------
// djvViewToolActions
//------------------------------------------------------------------------------

djvViewToolActions::djvViewToolActions(QObject * parent) :
	djvViewAbstractActions(parent),
    _p(new P)
{
    // Create the action groups.

    for (int i = 0; i < GROUP_COUNT; ++i)
    {
        _groups[i] = new QActionGroup(this);
    }

    _groups[TOOL_GROUP]->setExclusive(false);
    
    const QStringList toolText = QStringList() <<
        tr("&Magnify") <<
        tr("&Color Picker") <<
        tr("&Histogram") <<
        tr("&Information");
    
    const QList<bool> toolCheckable = QList<bool>() <<
        true <<
        true <<
        true <<
        true;
    
    const QList<QIcon> toolIcons = QList<QIcon>() <<
        djvIconLibrary::global()->icon("djvMagnifyIcon.png") <<
        djvIconLibrary::global()->icon("djvColorPickerIcon.png") <<
        djvIconLibrary::global()->icon("djvHistogramIcon.png") <<
        djvIconLibrary::global()->icon("djvInfoIcon.png");
    
    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
        QAction * action = new QAction(this);
        action->setText(toolText[i]);
        action->setCheckable(toolCheckable[i]);
        action->setIcon(toolIcons[i]);
        action->setData(i);

        _groups[TOOL_GROUP]->addAction(action);
    }

    // Initialize.

    update();

    // Setup the callbacks.

    connect(
        djvViewShortcutPrefs::global(),
        SIGNAL(shortcutsChanged(const QVector<djvShortcut> &)),
        SLOT(update()));
}

djvViewToolActions::~djvViewToolActions()
{
    delete _p;
}

void djvViewToolActions::update()
{
    const QVector<djvShortcut> & shortcuts =
        djvViewShortcutPrefs::global()->shortcuts();

    // Update the action groups.
    
    const QVector<QKeySequence> toolShortcuts = QVector<QKeySequence>() <<
        shortcuts[djvViewUtil::SHORTCUT_TOOL_MAGNIFY].value <<
        shortcuts[djvViewUtil::SHORTCUT_TOOL_COLOR_PICKER].value <<
        shortcuts[djvViewUtil::SHORTCUT_TOOL_HISTOGRAM].value <<
        shortcuts[djvViewUtil::SHORTCUT_TOOL_INFO].value;
    
    const QStringList toolToolTips = QStringList() <<
        tr("Toggle the magnify tool\n\nShortcut: %1") <<
        tr("Toggle the color picker tool\n\nShortcut: %1") <<
        tr("Toggle the histogram tool\n\nShortcut: %1") <<
        tr("Toggle the information tool\n\nShortcut: %1");
    
    for (int i = 0; i < djvViewUtil::TOOL_COUNT; ++i)
    {
         _groups[TOOL_GROUP]->actions()[i]->setShortcut(toolShortcuts[i]);
         _groups[TOOL_GROUP]->actions()[i]->setToolTip(
            toolToolTips[i].arg(toolShortcuts[i].toString()));
    }

    // Fix up the actions.
    
    osxMenuHack();

    // Emit changed signal.

    Q_EMIT changed();
}

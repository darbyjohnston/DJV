//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <Actions.h>

#include <Util.h>
#include <View.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>

//------------------------------------------------------------------------------
// Actions
//------------------------------------------------------------------------------

QAction * Actions::upAction(QObject * parent)
{
    QAction * action = new QAction(parent);
    action->setText(qApp->translate("djvFileBrowserExperiment", "Up"));
    action->setToolTip(qApp->translate("djvFileBrowserExperiment",
        "Go up a directory"));
    action->setIcon(QPixmap(":djvDirUpIcon.png"));
    return action;
}

QAction * Actions::backAction(QObject * parent)
{
    QAction * action = new QAction(parent);
    action->setText(qApp->translate("djvFileBrowserExperiment", "Back"));
    action->setToolTip(qApp->translate("djvFileBrowserExperiment",
        "Go to the previous directory"));
    action->setIcon(QPixmap(":djvDirPrevIcon.png"));
    return action;
}

QAction * Actions::reloadAction(QObject * parent)
{
    QAction * action = new QAction(parent);
    action->setText(qApp->translate("djvFileBrowserExperiment", "Reload"));
    action->setToolTip(qApp->translate("djvFileBrowserExperiment",
        "Reload the current directory"));
    action->setIcon(QPixmap(":djvDirReloadIcon.png"));
    return action;
}

QActionGroup * Actions::sequenceActionGroup(QObject * parent)
{
    QActionGroup * group = new QActionGroup(parent);
    group->setExclusive(true);
    const ActionDataList & data = Util::sequenceData();
    for (int i = 0; i < data.count(); ++i)
    {
        QAction * action = group->addAction(data[i].text);
        action->setCheckable(true);
        action->setToolTip(data[i].toolTip);
        action->setIcon(QPixmap(data[i].icon));
    }
    return group;
}

QActionGroup * Actions::viewActionGroup(QObject * parent)
{
    QActionGroup * group = new QActionGroup(parent);
    group->setExclusive(true);
    const ActionDataList & data = View::modeData();
    for (int i = 0; i < data.count(); ++i)
    {
        QAction * action = group->addAction(data[i].text);
        action->setCheckable(true);
        action->setToolTip(data[i].toolTip);
        action->setIcon(QPixmap(data[i].icon));
    }
    return group;
}

QAction * Actions::leftDockAction(QObject * parent)
{
    QAction * action = new QAction(parent);
    action->setCheckable(true);
    action->setText(qApp->translate("djvFileBrowserExperiment", "Left Dock"));
    action->setToolTip(qApp->translate("djvFileBrowserExperiment",
        "Toggle the left dock"));
    action->setIcon(QPixmap(":djvLeftDockIcon.png"));
    return action;
}

QAction * Actions::rightDockAction(QObject * parent)
{
    QAction * action = new QAction(parent);
    action->setCheckable(true);
    action->setText(qApp->translate("djvFileBrowserExperiment", "Right Dock"));
    action->setToolTip(qApp->translate("djvFileBrowserExperiment",
        "Toggle the right dock"));
    action->setIcon(QPixmap(":djvRightDockIcon.png"));
    return action;
}


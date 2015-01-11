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

//! \file djvViewImageGroup.cpp

#include <djvViewImageGroup.h>

#include <djvViewDisplayProfile.h>
#include <djvViewDisplayProfileWidget.h>
#include <djvViewImageActions.h>
#include <djvViewImageMenu.h>
#include <djvViewImagePrefs.h>
#include <djvViewImageToolBar.h>
#include <djvViewMainWindow.h>
#include <djvViewShortcutPrefs.h>

#include <djvIconLibrary.h>
#include <djvInputDialog.h>
#include <djvMultiChoiceDialog.h>
#include <djvQuestionDialog.h>

#include <djvListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

//------------------------------------------------------------------------------
// djvViewImageGroup::P
//------------------------------------------------------------------------------

struct djvViewImageGroup::P
{
    P() :
        frameStore               (false),
        mirror                   (djvViewImagePrefs::global()->mirror()),
        scale                    (djvViewImagePrefs::global()->scale()),
        rotate                   (djvViewImagePrefs::global()->rotate()),
        colorProfile             (djvViewImagePrefs::global()->hasColorProfile()),
        displayProfile           (djvViewImagePrefs::global()->displayProfile()),
        channel                  (djvViewImagePrefs::global()->channel()),
        actions                  (0),
        menu                     (0),
        toolBar                  (0),
        displayProfileWidget     (0),
        displayProfileDockWidget (0)
    {}
    
    bool                           frameStore;
    djvPixelDataInfo::Mirror       mirror;
    djvViewUtil::IMAGE_SCALE       scale;
    djvViewUtil::IMAGE_ROTATE      rotate;
    bool                           colorProfile;
    djvViewDisplayProfile          displayProfile;
    djvOpenGlImageOptions::CHANNEL channel;
    
    djvViewImageActions *          actions;
    djvViewImageMenu *             menu;
    djvViewImageToolBar *          toolBar;
    djvViewDisplayProfileWidget *  displayProfileWidget;
    QDockWidget *                  displayProfileDockWidget;
};

//------------------------------------------------------------------------------
// djvViewImageGroup
//------------------------------------------------------------------------------

djvViewImageGroup::djvViewImageGroup(
    djvViewMainWindow *       mainWindow,
    const djvViewImageGroup * copy) :
    djvViewAbstractGroup(mainWindow),
    _p(new P)
{
    //DJV_DEBUG("djvViewImageGroup::djvViewImageGroup");

    if (copy)
    {
        _p->frameStore     = copy->_p->frameStore;
        _p->mirror         = copy->_p->mirror;
        _p->scale          = copy->_p->scale;
        _p->rotate         = copy->_p->rotate;
        _p->colorProfile   = copy->_p->colorProfile;
        _p->displayProfile = copy->_p->displayProfile;
        _p->channel        = copy->_p->channel;
    }

    // Create the actions.

    _p->actions = new djvViewImageActions(this);
    
    // Create the menus.

    _p->menu = new djvViewImageMenu(_p->actions, mainWindow->menuBar());

    mainWindow->menuBar()->addMenu(_p->menu);

    // Create the widgets.

    _p->toolBar = new djvViewImageToolBar(_p->actions);

    mainWindow->addToolBar(_p->toolBar);
    
    _p->displayProfileWidget =
        new djvViewDisplayProfileWidget(mainWindow->viewWidget());
    _p->displayProfileDockWidget = new QDockWidget(tr("Display Profile"));
    _p->displayProfileDockWidget->setWidget(_p->displayProfileWidget);

    mainWindow->addDockWidget(
        Qt::RightDockWidgetArea,
        _p->displayProfileDockWidget);
    
    // Initialize.
    
    update();

    // Setup action callbacks.

    connect(
        _p->actions->action(djvViewImageActions::FRAME_STORE),
        SIGNAL(toggled(bool)),
        SLOT(frameStoreCallback(bool)));

    connect(
        _p->actions->action(djvViewImageActions::LOAD_FRAME_STORE),
        SIGNAL(triggered()),
        SIGNAL(loadFrameStore()));

    connect(
        _p->actions->action(djvViewImageActions::MIRROR_H),
        SIGNAL(toggled(bool)),
        SLOT(mirrorHCallback(bool)));

    connect(
        _p->actions->action(djvViewImageActions::MIRROR_V),
        SIGNAL(toggled(bool)),
        SLOT(mirrorVCallback(bool)));

    connect(
        _p->actions->action(djvViewImageActions::COLOR_PROFILE),
        SIGNAL(toggled(bool)),
        SLOT(colorProfileCallback(bool)));

    connect(
        _p->actions->action(djvViewImageActions::DISPLAY_PROFILE_VISIBLE),
        SIGNAL(toggled(bool)),
        SLOT(update()));

    // Setup action group callbacks.

    connect(
        _p->actions->group(djvViewImageActions::SCALE_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(scaleCallback(QAction *)));

    connect(
        _p->actions->group(djvViewImageActions::ROTATE_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(rotateCallback(QAction *)));

    connect(
        _p->actions->group(djvViewImageActions::DISPLAY_PROFILE_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(displayProfileCallback(QAction *)));

    connect(
        _p->actions->group(djvViewImageActions::CHANNEL_GROUP),
        SIGNAL(triggered(QAction *)),
        SLOT(channelCallback(QAction *)));

    // Setup widget callbacks.

    _p->actions->action(djvViewImageActions::DISPLAY_PROFILE_VISIBLE)->connect(
        _p->displayProfileDockWidget,
        SIGNAL(visibilityChanged(bool)),
        SLOT(setChecked(bool)));

    connect(
        _p->displayProfileWidget,
        SIGNAL(displayProfileChanged(const djvViewDisplayProfile &)),
        SLOT(setDisplayProfile(const djvViewDisplayProfile &)));

    _p->displayProfileWidget->connect(
        this,
        SIGNAL(displayProfileChanged(const djvViewDisplayProfile &)),
        SLOT(setDisplayProfile(const djvViewDisplayProfile &)));

    // Setup preferences callbacks.

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(mirrorChanged(djvPixelDataInfo::Mirror)),
        SLOT(mirrorCallback(djvPixelDataInfo::Mirror)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(scaleChanged(djvViewUtil::IMAGE_SCALE)),
        SLOT(scaleCallback(djvViewUtil::IMAGE_SCALE)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(rotateChanged(djvViewUtil::IMAGE_ROTATE)),
        SLOT(rotateCallback(djvViewUtil::IMAGE_ROTATE)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(colorProfileChanged(bool)),
        SLOT(colorProfileCallback(bool)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(displayProfileChanged(const djvViewDisplayProfile &)),
        SLOT(setDisplayProfile(const djvViewDisplayProfile &)));

    connect(
        djvViewImagePrefs::global(),
        SIGNAL(channelChanged(djvOpenGlImageOptions::CHANNEL)),
        SLOT(channelCallback(djvOpenGlImageOptions::CHANNEL)));
}

djvViewImageGroup::~djvViewImageGroup()
{
    delete _p;
}

bool djvViewImageGroup::hasFrameStore() const
{
    return _p->frameStore;
}

const djvPixelDataInfo::Mirror & djvViewImageGroup::mirror() const
{
    return _p->mirror;
}

djvViewUtil::IMAGE_SCALE djvViewImageGroup::scale() const
{
    return _p->scale;
}

djvViewUtil::IMAGE_ROTATE djvViewImageGroup::rotate() const
{
    return _p->rotate;
}

bool djvViewImageGroup::hasColorProfile() const
{
    return _p->colorProfile;
}

const djvViewDisplayProfile & djvViewImageGroup::displayProfile() const
{
    return _p->displayProfile;
}

void djvViewImageGroup::setDisplayProfile(const djvViewDisplayProfile & in)
{
    if (in == _p->displayProfile)
        return;
    
    _p->displayProfile = in;
    
    Q_EMIT displayProfileChanged(_p->displayProfile);
}

djvOpenGlImageOptions::CHANNEL djvViewImageGroup::channel() const
{
    return _p->channel;
}

QToolBar * djvViewImageGroup::toolBar() const
{
    return _p->toolBar;
}

void djvViewImageGroup::frameStoreCallback(bool in)
{
    //DJV_DEBUG("djvViewImageGroup::frameStoreCallback");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->frameStore = in;

    Q_EMIT frameStoreChanged(_p->frameStore);
}

void djvViewImageGroup::mirrorCallback(const djvPixelDataInfo::Mirror & in)
{
    _p->mirror = in;

    Q_EMIT redrawNeeded();
}

void djvViewImageGroup::mirrorHCallback(bool in)
{
    mirrorCallback(djvPixelDataInfo::Mirror(in, _p->mirror.y));
}

void djvViewImageGroup::mirrorVCallback(bool in)
{
    mirrorCallback(djvPixelDataInfo::Mirror(_p->mirror.x, in));
}

void djvViewImageGroup::scaleCallback(djvViewUtil::IMAGE_SCALE scale)
{
    //DJV_DEBUG("djvViewImageGroup::scaleCallback");
    //DJV_DEBUG_PRINT("scale = " << scale);
    
    _p->scale = scale;

    Q_EMIT redrawNeeded();
    Q_EMIT resizeNeeded();
}

void djvViewImageGroup::scaleCallback(QAction * action)
{
    scaleCallback(
        static_cast<djvViewUtil::IMAGE_SCALE>(action->data().toInt()));
}

void djvViewImageGroup::rotateCallback(djvViewUtil::IMAGE_ROTATE rotate)
{
    //DJV_DEBUG("djvViewImageGroup::rotateCallback");
    //DJV_DEBUG_PRINT("rotate = " << rotate);

    _p->rotate = rotate;

    Q_EMIT redrawNeeded();
    Q_EMIT resizeNeeded();
}

void djvViewImageGroup::rotateCallback(QAction * action)
{
    rotateCallback(
        static_cast<djvViewUtil::IMAGE_ROTATE>(action->data().toInt()));
}

void djvViewImageGroup::colorProfileCallback(bool in)
{
    //DJV_DEBUG("djvViewImageGroup::colorProfileCallback");
    //DJV_DEBUG_PRINT("in = " << in);

    _p->colorProfile = in;

    Q_EMIT redrawNeeded();
}

void djvViewImageGroup::displayProfileCallback(QAction * action)
{
    const int index = action->data().toInt();

    setDisplayProfile(
        index > 0 ?
        djvViewImagePrefs::global()->displayProfiles()[index - 1] :
        djvViewDisplayProfile());
}

void djvViewImageGroup::channelCallback(djvOpenGlImageOptions::CHANNEL channel)
{
    if (channel == _p->channel)
    {
        _p->channel = djvOpenGlImageOptions::CHANNEL_DEFAULT;
    }
    else
    {
        _p->channel = channel;
    }

    Q_EMIT redrawNeeded();
}

void djvViewImageGroup::channelCallback(QAction * action)
{
    channelCallback(
        static_cast<djvOpenGlImageOptions::CHANNEL>(action->data().toInt()));
}

void djvViewImageGroup::update()
{
    // Update actions.

    _p->actions->action(djvViewImageActions::FRAME_STORE)->
        setChecked(_p->frameStore);

    _p->actions->action(djvViewImageActions::MIRROR_H)->
        setChecked(_p->mirror.x);

    _p->actions->action(djvViewImageActions::MIRROR_V)->
        setChecked(_p->mirror.y);

    _p->actions->action(djvViewImageActions::COLOR_PROFILE)->
        setChecked(_p->colorProfile);

    // Update action groups.

    if (! _p->actions->group(djvViewImageActions::SCALE_GROUP)->
        actions()[_p->scale]->isChecked())
    {
        _p->actions->group(djvViewImageActions::SCALE_GROUP)->
            actions()[_p->scale]->trigger();
    }

    if (! _p->actions->group(djvViewImageActions::ROTATE_GROUP)->
        actions()[_p->rotate]->isChecked())
    {
        _p->actions->group(djvViewImageActions::ROTATE_GROUP)->
            actions()[_p->rotate]->trigger();
    }

    if (! _p->actions->group(djvViewImageActions::CHANNEL_GROUP)->
        actions()[_p->channel]->isChecked())
    {
        _p->actions->group(djvViewImageActions::CHANNEL_GROUP)->
            actions()[_p->channel]->trigger();
    }

    // Update widgets.

    _p->displayProfileDockWidget->setVisible(
        _p->actions->action(djvViewImageActions::DISPLAY_PROFILE_VISIBLE)->
        isChecked());

    _p->displayProfileWidget->setDisplayProfile(
        _p->displayProfile);
}

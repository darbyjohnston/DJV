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

#include <djvViewLib/ImageGroup.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/DisplayProfileWidget.h>
#include <djvViewLib/ImageActions.h>
#include <djvViewLib/ImageMenu.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageToolBar.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/ShortcutPrefs.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/InputDialog.h>
#include <djvUI/MultiChoiceDialog.h>
#include <djvUI/QuestionDialog.h>

#include <djvCore/ListUtil.h>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>

namespace djv
{
    namespace ViewLib
    {
        struct ImageGroup::Private
        {
            Private(Context * context) :
                mirror(context->imagePrefs()->mirror()),
                scale(context->imagePrefs()->scale()),
                rotate(context->imagePrefs()->rotate()),
                colorProfile(context->imagePrefs()->hasColorProfile()),
                displayProfile(context->imagePrefs()->displayProfile()),
                channel(context->imagePrefs()->channel()),
                actions(0),
                menu(0),
                toolBar(0),
                displayProfileWidget(0),
                displayProfileDockWidget(0)
            {}

            bool                           frameStore = false;
            djvPixelDataInfo::Mirror       mirror;
            Util::IMAGE_SCALE              scale = static_cast<Util::IMAGE_SCALE>(0);
            Util::IMAGE_ROTATE             rotate = static_cast<Util::IMAGE_ROTATE>(0);
            bool                           colorProfile = false;
            DisplayProfile                 displayProfile;
            djvOpenGLImageOptions::CHANNEL channel = static_cast<djvOpenGLImageOptions::CHANNEL>(0);

            ImageActions *          actions = nullptr;
            ImageMenu *             menu = nullptr;
            ImageToolBar *          toolBar = nullptr;
            DisplayProfileWidget *  displayProfileWidget = nullptr;
            QDockWidget *           displayProfileDockWidget = nullptr;
        };

        ImageGroup::ImageGroup(
            const ImageGroup * copy,
            MainWindow *       mainWindow,
            Context *          context) :
            AbstractGroup(mainWindow, context),
            _p(new Private(context))
        {
            //DJV_DEBUG("ImageGroup::ImageGroup");

            if (copy)
            {
                _p->frameStore = copy->_p->frameStore;
                _p->mirror = copy->_p->mirror;
                _p->scale = copy->_p->scale;
                _p->rotate = copy->_p->rotate;
                _p->colorProfile = copy->_p->colorProfile;
                _p->displayProfile = copy->_p->displayProfile;
                _p->channel = copy->_p->channel;
            }

            // Create the actions.
            _p->actions = new ImageActions(context, this);

            // Create the menus.
            _p->menu = new ImageMenu(_p->actions, mainWindow->menuBar());
            mainWindow->menuBar()->addMenu(_p->menu);

            // Create the widgets.
            _p->toolBar = new ImageToolBar(_p->actions, context);

            mainWindow->addToolBar(_p->toolBar);

            _p->displayProfileWidget =
                new DisplayProfileWidget(mainWindow->viewWidget(), context);
            _p->displayProfileDockWidget = new QDockWidget(
                qApp->translate("djv::ViewLib::ImageGroup", "Display Profile"));
            _p->displayProfileDockWidget->setWidget(_p->displayProfileWidget);

            mainWindow->addDockWidget(
                Qt::RightDockWidgetArea,
                _p->displayProfileDockWidget);

            // Initialize.
            update();

            // Setup action callbacks.
            connect(
                _p->actions->action(ImageActions::FRAME_STORE),
                SIGNAL(toggled(bool)),
                SLOT(frameStoreCallback(bool)));
            connect(
                _p->actions->action(ImageActions::LOAD_FRAME_STORE),
                SIGNAL(triggered()),
                SIGNAL(loadFrameStore()));
            connect(
                _p->actions->action(ImageActions::MIRROR_H),
                SIGNAL(toggled(bool)),
                SLOT(mirrorHCallback(bool)));
            connect(
                _p->actions->action(ImageActions::MIRROR_V),
                SIGNAL(toggled(bool)),
                SLOT(mirrorVCallback(bool)));
            connect(
                _p->actions->action(ImageActions::COLOR_PROFILE),
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->actions->action(ImageActions::DISPLAY_PROFILE_VISIBLE),
                SIGNAL(toggled(bool)),
                SLOT(update()));

            // Setup action group callbacks.
            connect(
                _p->actions->group(ImageActions::SCALE_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(scaleCallback(QAction *)));
            connect(
                _p->actions->group(ImageActions::ROTATE_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(rotateCallback(QAction *)));
            connect(
                _p->actions->group(ImageActions::DISPLAY_PROFILE_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(displayProfileCallback(QAction *)));
            connect(
                _p->actions->group(ImageActions::CHANNEL_GROUP),
                SIGNAL(triggered(QAction *)),
                SLOT(channelCallback(QAction *)));

            // Setup widget callbacks.
            _p->actions->action(ImageActions::DISPLAY_PROFILE_VISIBLE)->connect(
                _p->displayProfileDockWidget,
                SIGNAL(visibilityChanged(bool)),
                SLOT(setChecked(bool)));
            connect(
                _p->displayProfileWidget,
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(setDisplayProfile(const djv::ViewLib::DisplayProfile &)));
            _p->displayProfileWidget->connect(
                this,
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(setDisplayProfile(const djv::ViewLib::DisplayProfile &)));

            // Setup preferences callbacks.
            connect(
                context->imagePrefs(),
                SIGNAL(mirrorChanged(djvPixelDataInfo::Mirror)),
                SLOT(mirrorCallback(djvPixelDataInfo::Mirror)));
            connect(
                context->imagePrefs(),
                SIGNAL(scaleChanged(djv::ViewLib::Util::IMAGE_SCALE)),
                SLOT(scaleCallback(djv::ViewLib::Util::IMAGE_SCALE)));
            connect(
                context->imagePrefs(),
                SIGNAL(rotateChanged(djv::ViewLib::Util::IMAGE_ROTATE)),
                SLOT(rotateCallback(djv::ViewLib::Util::IMAGE_ROTATE)));
            connect(
                context->imagePrefs(),
                SIGNAL(colorProfileChanged(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                context->imagePrefs(),
                SIGNAL(displayProfileChanged(const djv::ViewLib::DisplayProfile &)),
                SLOT(setDisplayProfile(const djv::ViewLib::DisplayProfile &)));
            connect(
                context->imagePrefs(),
                SIGNAL(channelChanged(djvOpenGLImageOptions::CHANNEL)),
                SLOT(channelCallback(djvOpenGLImageOptions::CHANNEL)));
        }

        ImageGroup::~ImageGroup()
        {}

        bool ImageGroup::hasFrameStore() const
        {
            return _p->frameStore;
        }

        const djvPixelDataInfo::Mirror & ImageGroup::mirror() const
        {
            return _p->mirror;
        }

        Util::IMAGE_SCALE ImageGroup::scale() const
        {
            return _p->scale;
        }

        Util::IMAGE_ROTATE ImageGroup::rotate() const
        {
            return _p->rotate;
        }

        bool ImageGroup::hasColorProfile() const
        {
            return _p->colorProfile;
        }

        const DisplayProfile & ImageGroup::displayProfile() const
        {
            return _p->displayProfile;
        }

        void ImageGroup::setDisplayProfile(const DisplayProfile & in)
        {
            if (in == _p->displayProfile)
                return;
            _p->displayProfile = in;
            Q_EMIT displayProfileChanged(_p->displayProfile);
        }

        djvOpenGLImageOptions::CHANNEL ImageGroup::channel() const
        {
            return _p->channel;
        }

        QToolBar * ImageGroup::toolBar() const
        {
            return _p->toolBar;
        }

        void ImageGroup::frameStoreCallback(bool in)
        {
            //DJV_DEBUG("ImageGroup::frameStoreCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->frameStore = in;
            Q_EMIT frameStoreChanged(_p->frameStore);
        }

        void ImageGroup::mirrorCallback(const djvPixelDataInfo::Mirror & in)
        {
            _p->mirror = in;
            Q_EMIT redrawNeeded();
        }

        void ImageGroup::mirrorHCallback(bool in)
        {
            mirrorCallback(djvPixelDataInfo::Mirror(in, _p->mirror.y));
        }

        void ImageGroup::mirrorVCallback(bool in)
        {
            mirrorCallback(djvPixelDataInfo::Mirror(_p->mirror.x, in));
        }

        void ImageGroup::scaleCallback(Util::IMAGE_SCALE scale)
        {
            //DJV_DEBUG("ImageGroup::scaleCallback");
            //DJV_DEBUG_PRINT("scale = " << scale);
            _p->scale = scale;
            Q_EMIT redrawNeeded();
            Q_EMIT resizeNeeded();
        }

        void ImageGroup::scaleCallback(QAction * action)
        {
            scaleCallback(
                static_cast<Util::IMAGE_SCALE>(action->data().toInt()));
        }

        void ImageGroup::rotateCallback(Util::IMAGE_ROTATE rotate)
        {
            //DJV_DEBUG("ImageGroup::rotateCallback");
            //DJV_DEBUG_PRINT("rotate = " << rotate);
            _p->rotate = rotate;
            Q_EMIT redrawNeeded();
            Q_EMIT resizeNeeded();
        }

        void ImageGroup::rotateCallback(QAction * action)
        {
            rotateCallback(static_cast<Util::IMAGE_ROTATE>(action->data().toInt()));
        }

        void ImageGroup::colorProfileCallback(bool in)
        {
            //DJV_DEBUG("ImageGroup::colorProfileCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->colorProfile = in;
            Q_EMIT redrawNeeded();
        }

        void ImageGroup::displayProfileCallback(QAction * action)
        {
            const int index = action->data().toInt();
            setDisplayProfile(
                index > 0 ?
                context()->imagePrefs()->displayProfiles()[index - 1] :
                DisplayProfile());
        }

        void ImageGroup::channelCallback(djvOpenGLImageOptions::CHANNEL channel)
        {
            if (channel == _p->channel)
            {
                _p->channel = djvOpenGLImageOptions::CHANNEL_DEFAULT;
            }
            else
            {
                _p->channel = channel;
            }

            Q_EMIT redrawNeeded();
        }

        void ImageGroup::channelCallback(QAction * action)
        {
            channelCallback(
                static_cast<djvOpenGLImageOptions::CHANNEL>(action->data().toInt()));
        }

        void ImageGroup::update()
        {
            // Update actions.
            _p->actions->action(ImageActions::FRAME_STORE)->
                setChecked(_p->frameStore);
            _p->actions->action(ImageActions::MIRROR_H)->
                setChecked(_p->mirror.x);
            _p->actions->action(ImageActions::MIRROR_V)->
                setChecked(_p->mirror.y);
            _p->actions->action(ImageActions::COLOR_PROFILE)->
                setChecked(_p->colorProfile);

            // Update action groups.
            if (!_p->actions->group(ImageActions::SCALE_GROUP)->
                actions()[_p->scale]->isChecked())
            {
                _p->actions->group(ImageActions::SCALE_GROUP)->
                    actions()[_p->scale]->trigger();
            }
            if (!_p->actions->group(ImageActions::ROTATE_GROUP)->
                actions()[_p->rotate]->isChecked())
            {
                _p->actions->group(ImageActions::ROTATE_GROUP)->
                    actions()[_p->rotate]->trigger();
            }
            if (!_p->actions->group(ImageActions::CHANNEL_GROUP)->
                actions()[_p->channel]->isChecked())
            {
                _p->actions->group(ImageActions::CHANNEL_GROUP)->
                    actions()[_p->channel]->trigger();
            }

            // Update widgets.
            _p->displayProfileDockWidget->setVisible(
                _p->actions->action(ImageActions::DISPLAY_PROFILE_VISIBLE)->
                isChecked());
            _p->displayProfileWidget->setDisplayProfile(
                _p->displayProfile);
        }

    } // namespace ViewLib
} // namespace djv

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

#include <djvViewLib/ImageGroup.h>

#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/DisplayProfileWidget.h>
#include <djvViewLib/ImageActions.h>
#include <djvViewLib/ImageMenu.h>
#include <djvViewLib/ImagePrefs.h>
#include <djvViewLib/ImageToolBar.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ShortcutPrefs.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/InputDialog.h>
#include <djvUI/MultiChoiceDialog.h>
#include <djvUI/QuestionDialog.h>

#include <djvCore/ListUtil.h>

#include <QApplication>
#include <QDockWidget>
#include <QHBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct ImageGroup::Private
        {
            Private(const QPointer<ViewContext> & context) :
                mirror(context->imagePrefs()->mirror()),
                scale(context->imagePrefs()->scale()),
                rotate(context->imagePrefs()->rotate()),
                premultipliedAlpha(context->imagePrefs()->hasPremultipliedAlpha()),
                colorProfile(context->imagePrefs()->hasColorProfile()),
                displayProfile(context->imagePrefs()->displayProfile()),
                channel(context->imagePrefs()->channel())
            {}

            bool                                  frameStoreVisible = false;
            AV::PixelDataInfo::Mirror       mirror;
            Enum::IMAGE_SCALE                     scale = static_cast<Enum::IMAGE_SCALE>(0);
            Enum::IMAGE_ROTATE                    rotate = static_cast<Enum::IMAGE_ROTATE>(0);
            bool                                  premultipliedAlpha = true;
            bool                                  colorProfile = false;
            DisplayProfile                        displayProfile;
            AV::OpenGLImageOptions::CHANNEL channel = static_cast<AV::OpenGLImageOptions::CHANNEL>(0);

            QPointer<ImageActions>         actions;
            QPointer<DisplayProfileWidget> displayProfileWidget;
            QPointer<QDockWidget>          displayProfileDockWidget;
        };

        ImageGroup::ImageGroup(
            const QPointer<ImageGroup>& copy,
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context) :
            AbstractGroup(session, context),
            _p(new Private(context))
        {
            //DJV_DEBUG("ImageGroup::ImageGroup");

            if (copy)
            {
                _p->frameStoreVisible = copy->_p->frameStoreVisible;
                _p->mirror = copy->_p->mirror;
                _p->scale = copy->_p->scale;
                _p->rotate = copy->_p->rotate;
                _p->premultipliedAlpha = copy->_p->premultipliedAlpha;
                _p->colorProfile = copy->_p->colorProfile;
                _p->displayProfile = copy->_p->displayProfile;
                _p->channel = copy->_p->channel;
            }

            // Create the actions.
            _p->actions = new ImageActions(context, this);

            // Create the widgets.
            _p->displayProfileWidget = new DisplayProfileWidget(session->viewWidget(), context);
            _p->displayProfileDockWidget = new QDockWidget(
                qApp->translate("djv::ViewLib::ImageGroup", "Display Profile"));
            _p->displayProfileDockWidget->setWidget(_p->displayProfileWidget);

            // Initialize.
            update();

            // Setup action callbacks.
            connect(
                _p->actions->action(ImageActions::SHOW_FRAME_STORE),
                SIGNAL(toggled(bool)),
                SLOT(showFrameStoreCallback(bool)));
            connect(
                _p->actions->action(ImageActions::SET_FRAME_STORE),
                SIGNAL(triggered()),
                SIGNAL(setFrameStore()));
            connect(
                _p->actions->action(ImageActions::MIRROR_H),
                SIGNAL(toggled(bool)),
                SLOT(mirrorHCallback(bool)));
            connect(
                _p->actions->action(ImageActions::MIRROR_V),
                SIGNAL(toggled(bool)),
                SLOT(mirrorVCallback(bool)));
            connect(
                _p->actions->action(ImageActions::PREMULTIPLIED_ALPHA),
                SIGNAL(toggled(bool)),
                SLOT(premultipliedAlphaCallback(bool)));
            connect(
                _p->actions->action(ImageActions::COLOR_PROFILE),
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->actions->action(ImageActions::DISPLAY_PROFILE_EDITOR),
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
            _p->actions->action(ImageActions::DISPLAY_PROFILE_EDITOR)->connect(
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
                SIGNAL(mirrorChanged(djv::AV::PixelDataInfo::Mirror)),
                SLOT(mirrorCallback(djv::AV::PixelDataInfo::Mirror)));
            connect(
                context->imagePrefs(),
                SIGNAL(scaleChanged(djv::ViewLib::Enum::IMAGE_SCALE)),
                SLOT(scaleCallback(djv::ViewLib::Enum::IMAGE_SCALE)));
            connect(
                context->imagePrefs(),
                SIGNAL(rotateChanged(djv::ViewLib::Enum::IMAGE_ROTATE)),
                SLOT(rotateCallback(djv::ViewLib::Enum::IMAGE_ROTATE)));
            connect(
                context->imagePrefs(),
                SIGNAL(premultipliedAlphaChanged(bool)),
                SLOT(premultipliedAlphaCallback(bool)));
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
                SIGNAL(channelChanged(djv::AV::OpenGLImageOptions::CHANNEL)),
                SLOT(channelCallback(djv::AV::OpenGLImageOptions::CHANNEL)));
        }

        ImageGroup::~ImageGroup()
        {}

        bool ImageGroup::isFrameStoreVisible() const
        {
            return _p->frameStoreVisible;
        }

        const AV::PixelDataInfo::Mirror & ImageGroup::mirror() const
        {
            return _p->mirror;
        }

        Enum::IMAGE_SCALE ImageGroup::scale() const
        {
            return _p->scale;
        }

        Enum::IMAGE_ROTATE ImageGroup::rotate() const
        {
            return _p->rotate;
        }

        bool ImageGroup::hasPremultipliedAlpha() const
        {
            return _p->premultipliedAlpha;
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
            update();
            Q_EMIT displayProfileChanged(_p->displayProfile);
        }

        AV::OpenGLImageOptions::CHANNEL ImageGroup::channel() const
        {
            return _p->channel;
        }

        QPointer<QDockWidget> ImageGroup::displayProfileDockWidget() const
        {
            return _p->displayProfileDockWidget;
        }

        QPointer<QMenu> ImageGroup::createMenu() const
        {
            return new ImageMenu(_p->actions.data());
        }

        QPointer<QToolBar> ImageGroup::createToolBar() const
        {
            return new ImageToolBar(_p->actions.data(), context());
        }

        void ImageGroup::showFrameStoreCallback(bool in)
        {
            //DJV_DEBUG("ImageGroup::showFrameStoreCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->frameStoreVisible = in;
            Q_EMIT showFrameStoreChanged(_p->frameStoreVisible);
        }

        void ImageGroup::mirrorCallback(const AV::PixelDataInfo::Mirror & in)
        {
            _p->mirror = in;
            update();
            Q_EMIT redrawNeeded();
        }

        void ImageGroup::mirrorHCallback(bool in)
        {
            mirrorCallback(AV::PixelDataInfo::Mirror(in, _p->mirror.y));
        }

        void ImageGroup::mirrorVCallback(bool in)
        {
            mirrorCallback(AV::PixelDataInfo::Mirror(_p->mirror.x, in));
        }

        void ImageGroup::scaleCallback(Enum::IMAGE_SCALE scale)
        {
            //DJV_DEBUG("ImageGroup::scaleCallback");
            //DJV_DEBUG_PRINT("scale = " << scale);
            _p->scale = scale;
            update();
            Q_EMIT redrawNeeded();
            Q_EMIT resizeNeeded();
        }

        void ImageGroup::scaleCallback(QAction * action)
        {
            scaleCallback(static_cast<Enum::IMAGE_SCALE>(action->data().toInt()));
        }

        void ImageGroup::rotateCallback(Enum::IMAGE_ROTATE rotate)
        {
            //DJV_DEBUG("ImageGroup::rotateCallback");
            //DJV_DEBUG_PRINT("rotate = " << rotate);
            _p->rotate = rotate;
            update();
            Q_EMIT redrawNeeded();
            Q_EMIT resizeNeeded();
        }

        void ImageGroup::rotateCallback(QAction * action)
        {
            rotateCallback(static_cast<Enum::IMAGE_ROTATE>(action->data().toInt()));
        }

        void ImageGroup::premultipliedAlphaCallback(bool value)
        {
            _p->premultipliedAlpha = value;
            update();
            Q_EMIT redrawNeeded();
        }

        void ImageGroup::colorProfileCallback(bool in)
        {
            //DJV_DEBUG("ImageGroup::colorProfileCallback");
            //DJV_DEBUG_PRINT("in = " << in);
            _p->colorProfile = in;
            update();
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

        void ImageGroup::channelCallback(AV::OpenGLImageOptions::CHANNEL channel)
        {
            if (channel == _p->channel)
            {
                _p->channel = AV::OpenGLImageOptions::CHANNEL_DEFAULT;
            }
            else
            {
                _p->channel = channel;
            }
            update();
            Q_EMIT redrawNeeded();
        }

        void ImageGroup::channelCallback(QAction * action)
        {
            channelCallback(static_cast<AV::OpenGLImageOptions::CHANNEL>(action->data().toInt()));
        }

        void ImageGroup::update()
        {
            _p->actions->action(ImageActions::SHOW_FRAME_STORE)->setChecked(_p->frameStoreVisible);
            _p->actions->action(ImageActions::MIRROR_H)->setChecked(_p->mirror.x);
            _p->actions->action(ImageActions::MIRROR_V)->setChecked(_p->mirror.y);
            _p->actions->action(ImageActions::PREMULTIPLIED_ALPHA)->setChecked(_p->premultipliedAlpha);
            _p->actions->action(ImageActions::COLOR_PROFILE)->setChecked(_p->colorProfile);

            _p->actions->group(ImageActions::SCALE_GROUP)->actions()[_p->scale]->setChecked(true);
            _p->actions->group(ImageActions::ROTATE_GROUP)->actions()[_p->rotate]->setChecked(true);
            _p->actions->group(ImageActions::CHANNEL_GROUP)->actions()[_p->channel]->setChecked(true);
            _p->actions->group(ImageActions::CHANNEL_GROUP)->actions()[_p->channel]->setChecked(true);

            _p->displayProfileDockWidget->setVisible(
                _p->actions->action(ImageActions::DISPLAY_PROFILE_EDITOR)->isChecked());
            _p->displayProfileWidget->setDisplayProfile(_p->displayProfile);
        }

    } // namespace ViewLib
} // namespace djv

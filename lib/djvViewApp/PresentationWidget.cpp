// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/PresentationWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct PresentationWidget::Private
        {
            ViewLock viewLock = ViewLock::First;
            uint32_t pressedID = Event::invalidID;

            //std::shared_ptr<ViewWidget> viewWidget;
            std::shared_ptr<UI::StackLayout> layout;

            std::function<void(void)> closeCallback;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<AV::Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<UI::ImageRotate> > imageRotateObserver;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > imageAspectRatioObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<ViewBackgroundOptions> > backgroundOptionsObserver;
            std::shared_ptr<ValueObserver<ViewLock> > viewLockObserver;
        };

        void PresentationWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::PresentationWidget");
            setPointerEnabled(true);

            //p.viewWidget = ViewWidget::create(context);

            p.layout = UI::StackLayout::create(context);
            //p.layout->addChild(p.viewWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<PresentationWidget>(std::dynamic_pointer_cast<PresentationWidget>(shared_from_this()));
            /*if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value)
                            {
                                auto viewWidget = value->getViewWidget();
                                widget->_p->imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                                    viewWidget->observeImage(),
                                    [weak](const std::shared_ptr<AV::Image::Image>& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setImage(value);
                                        }
                                    });

                                widget->_p->imageOptionsObserver = ValueObserver<AV::Render2D::ImageOptions>::create(
                                    viewWidget->observeImageOptions(),
                                    [weak](const AV::Render2D::ImageOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setImageOptions(value);
                                        }
                                    });

                                widget->_p->imageRotateObserver = ValueObserver<UI::ImageRotate>::create(
                                    viewWidget->observeImageRotate(),
                                    [weak](UI::ImageRotate value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setImageRotate(value);
                                        }
                                    });

                                widget->_p->imageAspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                                    viewWidget->observeImageAspectRatio(),
                                    [weak](UI::ImageAspectRatio value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setImageAspectRatio(value);
                                        }
                                    });

                                widget->_p->gridOptionsObserver = ValueObserver<GridOptions>::create(
                                    viewWidget->observeGridOptions(),
                                    [weak](const GridOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setGridOptions(value);
                                        }
                                    });

                                widget->_p->backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                    viewWidget->observeBackgroundOptions(),
                                    [weak](const ViewBackgroundOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewWidget->setBackgroundOptions(value);
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageObserver.reset();
                                widget->_p->imageOptionsObserver.reset();
                                widget->_p->imageRotateObserver.reset();
                                widget->_p->imageAspectRatioObserver.reset();
                                widget->_p->gridOptionsObserver.reset();
                                widget->_p->backgroundOptionsObserver.reset();
                            }
                        }
                    });
            }*/

            auto settings = context->getSystemT<UI::Settings::System>();
            /*if (auto viewSettings = settings->getSettingsT<ViewSettings>())
            {
                p.viewLockObserver = ValueObserver<ViewLock>::create(
                    viewSettings->observeLock(),
                    [weak](ViewLock value)
                    {
                        if (auto widget = weak.lock())
                        {
                            switch (value)
                            {
                            case ViewLock::Center:
                                widget->_p->viewWidget->imageCenter();
                                break;
                            case ViewLock::Frame:
                            default:
                                widget->_p->viewWidget->imageFrame();
                                break;
                            }
                        }
                    });
            }*/
        }

        PresentationWidget::PresentationWidget() :
            _p(new Private)
        {}

        PresentationWidget::~PresentationWidget()
        {}

        std::shared_ptr<PresentationWidget> PresentationWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PresentationWidget>(new PresentationWidget);
            out->_init(context);
            return out;
        }

        void PresentationWidget::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void PresentationWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void PresentationWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void PresentationWidget::_buttonPressEvent(Event::ButtonPress& event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto& info = event.getPointerInfo();
            p.pressedID = info.id;
        }

        void PresentationWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            DJV_PRIVATE_PTR();
            if (event.getPointerInfo().id != p.pressedID)
                return;
            event.accept();
            p.pressedID = Event::invalidID;
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void PresentationWidget::_initEvent(Event::Init & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
            }
        }

    } // namespace ViewApp
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidgetPrivate.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>

#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        PointerData::PointerData()
        {}

        PointerData::PointerData(
            PointerState state,
            const glm::vec2& pos,
            const std::map<int, bool>& buttons,
            int key,
            int keyModifiers) :
            state(state),
            pos(pos),
            buttons(buttons),
            key(key),
            keyModifiers(keyModifiers)
        {}

        bool PointerData::operator == (const PointerData& other) const
        {
            return
                state        == other.state &&
                pos          == other.pos &&
                buttons      == other.buttons &&
                key          == other.key &&
                keyModifiers == other.keyModifiers;
        }

        ScrollData::ScrollData()
        {}

        ScrollData::ScrollData(
            const glm::vec2& delta,
            int key,
            int keyModifiers) :
            delta(delta),
            key(key),
            keyModifiers(keyModifiers)
        {}

        bool ScrollData::operator == (const ScrollData& other) const
        {
            return
                delta == other.delta &&
                key == other.key &&
                keyModifiers == other.keyModifiers;
        }

        struct MediaWidget::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<Image::Image> image;
            ViewLock viewLock = ViewLock::First;
            bool frameStoreEnabled = false;
            std::shared_ptr<Image::Image> frameStore;
            std::shared_ptr<Observer::ValueSubject<PointerData> > hover;
            std::shared_ptr<Observer::ValueSubject<PointerData> > drag;
            std::shared_ptr<Observer::ValueSubject<ScrollData> > scroll;

            std::shared_ptr<PointerWidget> pointerWidget;
            std::shared_ptr<ViewWidget> viewWidget;
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Image> > > imageObserver;
            std::shared_ptr<Observer::Value<ViewLock> > viewLockObserver;
            std::shared_ptr<Observer::Value<bool> > frameStoreEnabledObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Image> > > frameStoreObserver;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.media = media;
            p.hover = Observer::ValueSubject<PointerData>::create();
            p.drag = Observer::ValueSubject<PointerData>::create();
            p.scroll = Observer::ValueSubject<ScrollData>::create();

            p.pointerWidget = PointerWidget::create(context);

            p.viewWidget = ViewWidget::create(media, context);

            p.layout = UI::StackLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.viewWidget);
            p.layout->addChild(p.pointerWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            p.pointerWidget->setHoverCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        const Math::BBox2f& g = widget->_p->viewWidget->getGeometry();
                        widget->_p->hover->setIfChanged(
                            PointerData(data.state, data.pos - g.min, data.buttons, data.key, data.keyModifiers));
                    }
                });
            p.pointerWidget->setDragCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        const Math::BBox2f& g = widget->_p->viewWidget->getGeometry();
                        widget->_p->drag->setIfChanged(
                            PointerData(data.state, data.pos - g.min, data.buttons, data.key, data.keyModifiers));
                    }
                });
            p.pointerWidget->setScrollCallback(
                [weak](const ScrollData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->moveToFront();
                        widget->_p->scroll->setAlways(value);
                    }
                });

            p.imageObserver = Observer::Value<std::shared_ptr<Image::Image> >::create(
                p.media->observeCurrentImage(),
                [weak](const std::shared_ptr<Image::Image>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->image = value;
                        widget->_imageUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
            {
                p.viewLockObserver = Observer::Value<ViewLock>::create(
                    viewSettings->observeLock(),
                    [weak](ViewLock value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->viewLock = value;
                        }
                    });
            }

            if (auto imageSystem = context->getSystemT<ImageSystem>())
            {
                p.frameStoreEnabledObserver = Observer::Value<bool>::create(
                    imageSystem->observeFrameStoreEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStoreEnabled = value;
                            widget->_imageUpdate();
                        }
                    });
                p.frameStoreObserver = Observer::Value<std::shared_ptr<Image::Image> >::create(
                    imageSystem->observeFrameStore(),
                    [weak](const std::shared_ptr<Image::Image>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStore = value;
                            widget->_imageUpdate();
                        }
                    });
            }
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(const std::shared_ptr<Media>& media, const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(media, context);
            return out;
        }

        const std::shared_ptr<Media>& MediaWidget::getMedia() const
        {
            return _p->media;
        }

        const std::shared_ptr<ViewWidget>& MediaWidget::getViewWidget() const
        {
            return _p->viewWidget;
        }

        std::shared_ptr<Observer::IValueSubject<PointerData> > MediaWidget::observeHover() const
        {
            return _p->hover;
        }

        std::shared_ptr<Observer::IValueSubject<PointerData> > MediaWidget::observeDrag() const
        {
            return _p->drag;
        }

        std::shared_ptr<Observer::IValueSubject<ScrollData> > MediaWidget::observeScroll() const
        {
            return _p->scroll;
        }

        void MediaWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const glm::vec2& minimumSize = p.layout->getMinimumSize();

            glm::vec2 imageSize = p.viewWidget->getMinimumSize();
            const float ar = p.viewWidget->getImageBBox().getAspect();
            if (ar > 1.F)
            {
                imageSize.x = std::max(imageSize.x * 2.F, minimumSize.x);
                imageSize.y = imageSize.x / ar;
            }
            else if (ar > 0.F)
            {
                imageSize.y = std::max(imageSize.y * 2.F, minimumSize.y);
                imageSize.x = imageSize.y * ar;
            }
            glm::vec2 size(ceilf(imageSize.x), ceilf(imageSize.y));
            
            _setMinimumSize(size);
        }

        void MediaWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            p.layout->setGeometry(getGeometry());
        }

        void MediaWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.viewWidget->setImage(p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

    } // namespace ViewApp
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>
#include <djvViewApp/HUDWidget.h>
#include <djvViewApp/ImageSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidgetPrivate.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/TimelineWidget.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/Border.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/LineEdit.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/MDIWidget.h>
#include <djvUI/MultiStateButton.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>
#include <djvUI/ToolButton.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/NumericValueModels.h>
#include <djvCore/Path.h>
#include <djvCore/Timer.h>

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
            Time::Units timeUnits = Time::Units::First;
            std::shared_ptr<Media> media;
            std::vector<AV::Image::Info> layers;
            int currentLayer = -1;
            std::shared_ptr<AV::Image::Image> image;
            ViewLock viewLock = ViewLock::First;
            std::shared_ptr<ValueSubject<HUDOptions> > hudOptions;
            bool frameStoreEnabled = false;
            std::shared_ptr<AV::Image::Image> frameStore;
            Math::Rational speed;
            float realSpeed = 0.F;
            Frame::Sequence sequence;
            Frame::Index currentFrame = Frame::invalidIndex;
            bool active = false;
            std::shared_ptr<ValueSubject<PointerData> > hover;
            std::shared_ptr<ValueSubject<PointerData> > drag;
            std::shared_ptr<ValueSubject<ScrollData> > scroll;

            std::shared_ptr<TitleBar> titleBar;
            std::shared_ptr<PointerWidget> pointerWidget;
            std::shared_ptr<ViewWidget> viewWidget;
            std::shared_ptr<HUDWidget> hud;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<ValueObserver<Time::Units> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::pair<std::vector<AV::Image::Info>, int> > > layersObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<ViewLock> > viewLockObserver;
            std::shared_ptr<ValueObserver<bool> > frameStoreEnabledObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > frameStoreObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > speedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver;
            std::shared_ptr<ListObserver<std::shared_ptr<AnnotatePrimitive> > > annotationsObserver;
        };

        void MediaWidget::_init(const std::shared_ptr<Media>& media, const std::shared_ptr<Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaWidget");

            p.media = media;
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.hudOptions = ValueSubject<HUDOptions>::create(viewSettings->observeHUDOptions()->get());
            p.hover = ValueSubject<PointerData>::create();
            p.drag = ValueSubject<PointerData>::create();
            p.scroll = ValueSubject<ScrollData>::create();

            p.titleBar = TitleBar::create(
                media->getFileInfo().getFileName(Frame::invalid, false),
                std::string(media->getFileInfo()),
                context);

            p.pointerWidget = PointerWidget::create(context);

            p.viewWidget = ViewWidget::create(context);

            p.hud = HUDWidget::create(context);
            p.hud->setHUDOptions(p.hudOptions->get());

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.titleBar);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(p.viewWidget);
            stackLayout->addChild(p.hud);
            stackLayout->addChild(p.pointerWidget);
            p.layout->addChild(stackLayout);
            p.layout->setStretch(stackLayout, UI::RowStretch::Expand);
            addChild(p.layout);

            _hudUpdate();

            auto weak = std::weak_ptr<MediaWidget>(std::dynamic_pointer_cast<MediaWidget>(shared_from_this()));
            p.titleBar->setMaximizeCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto canvas = std::dynamic_pointer_cast<UI::MDI::Canvas>(widget->getParent().lock()))
                        {
                            widget->moveToFront();
                            canvas->setMaximize(!canvas->isMaximized());
                        }
                    }
                });
            auto contextWeak = std::weak_ptr<Context>(context);
            p.titleBar->setCloseCallback(
                [media, contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto fileSystem = context->getSystemT<FileSystem>();
                        fileSystem->close(media);
                    }
                });

            p.pointerWidget->setHoverCallback(
                [weak](const PointerData& data)
                {
                    if (auto widget = weak.lock())
                    {
                        const BBox2f& g = widget->_p->viewWidget->getGeometry();
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
                        const BBox2f& g = widget->_p->viewWidget->getGeometry();
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

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_hudUpdate();
                    }
                });

            p.layersObserver = ValueObserver<std::pair<std::vector<AV::Image::Info>, int> >::create(
                p.media->observeLayers(),
                [weak](const std::pair<std::vector<AV::Image::Info>, int>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->layers = value.first;
                    widget->_p->currentLayer = value.second;
                    widget->_hudUpdate();
                }
            });

            p.imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                p.media->observeCurrentImage(),
                [weak](const std::shared_ptr<AV::Image::Image>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->image = value;
                        widget->_imageUpdate();
                    }
                });

            p.speedObserver = ValueObserver<Math::Rational>::create(
                p.media->observeSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_hudUpdate();
                    }
                });

            p.realSpeedObserver = ValueObserver<float>::create(
                p.media->observeRealSpeed(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->realSpeed = value;
                        widget->_hudUpdate();
                    }
                });

            p.sequenceObserver = ValueObserver<Frame::Sequence>::create(
                p.media->observeSequence(),
                [weak](const Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->sequence = value;
                        widget->_hudUpdate();
                    }
                });

            p.currentFrameObserver = ValueObserver<Frame::Index>::create(
                p.media->observeCurrentFrame(),
                [weak](Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_hudUpdate();
                    }
                });

            p.annotationsObserver = ListObserver<std::shared_ptr<AnnotatePrimitive> >::create(
                p.media->observeAnnotations(),
                [weak](const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewWidget->setAnnotations(value);
                    }
                });

            if (auto viewSettings = settingsSystem->getSettingsT<ViewSettings>())
            {
                p.viewLockObserver = ValueObserver<ViewLock>::create(
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
                p.frameStoreEnabledObserver = ValueObserver<bool>::create(
                    imageSystem->observeFrameStoreEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->frameStoreEnabled = value;
                            widget->_imageUpdate();
                        }
                    });
                p.frameStoreObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                    imageSystem->observeFrameStore(),
                    [weak](const std::shared_ptr<AV::Image::Image>& value)
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

        std::shared_ptr<MediaWidget> MediaWidget::create(const std::shared_ptr<Media>& media, const std::shared_ptr<Context>& context)
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

        void MediaWidget::fitWindow()
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const BBox2f imageBBox = p.viewWidget->getImageBBox();
            const float zoom = p.viewWidget->observeImageZoom()->get();
            const glm::vec2 imageSize = imageBBox.getSize() * zoom;
            glm::vec2 size(ceilf(imageSize.x), ceilf(imageSize.y + p.titleBar->getHeight()));
            resize(size + sh * 2.F);
        }

        std::shared_ptr<Core::IValueSubject<HUDOptions> > MediaWidget::observeHUDOptions() const
        {
            return _p->hudOptions;
        }

        void MediaWidget::setHUDOptions(const HUDOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.hudOptions->setIfChanged(value))
            {
                p.hud->setHUDOptions(value);
            }
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeHover() const
        {
            return _p->hover;
        }

        std::shared_ptr<IValueSubject<PointerData> > MediaWidget::observeDrag() const
        {
            return _p->drag;
        }

        std::shared_ptr<IValueSubject<ScrollData> > MediaWidget::observeScroll() const
        {
            return _p->scroll;
        }

        std::map<UI::MDI::Handle, std::vector<BBox2f> > MediaWidget::_getHandles() const
        {
            auto out = IWidget::_getHandles();
            out[UI::MDI::Handle::Move] = { _p->titleBar->getGeometry() };
            return out;
        }

        void MediaWidget::_setMaximize(float value)
        {
            IWidget::_setMaximize(value);
            DJV_PRIVATE_PTR();
            p.titleBar->setMaximize(value);
            _resize();
        }

        void MediaWidget::_setActiveWidget(bool value)
        {
            IWidget::_setActiveWidget(value);
            DJV_PRIVATE_PTR();
            p.active = value;
            p.titleBar->setActive(value);
            p.scroll->setAlways(ScrollData(glm::vec2(0.F, 0.F), 0, 0));
            _imageUpdate();
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
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
            
            _setMinimumSize(size + sh * 2.F);
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const BBox2f g = getGeometry().margin(-sh);
            p.layout->setGeometry(g);
        }

        void MediaWidget::_imageUpdate()
        {
            DJV_PRIVATE_PTR();
            p.viewWidget->setImage(p.active && p.frameStoreEnabled && p.frameStore ? p.frameStore : p.image);
        }

        void MediaWidget::_hudUpdate()
        {
            DJV_PRIVATE_PTR();
            HUDData data;
            data.fileName = p.media->getFileInfo().getFileName(Frame::invalid, false);
            if (p.currentLayer >= 0 && p.currentLayer < static_cast<int>(p.layers.size()))
            {
                const auto& layer = p.layers[p.currentLayer];
                data.layer = layer.name;
                data.size = layer.size;
                data.type = layer.type;
            }
            data.isSequence = p.sequence.getFrameCount() > 1;
            data.currentFrame = Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
            data.speed = p.speed;
            data.realSpeed = p.realSpeed;
            p.hud->setHUDData(data);
        }

    } // namespace ViewApp
} // namespace djv


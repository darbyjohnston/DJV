// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelinePIPWidget.h>

#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>
#include <djvUI/Window.h>

#include <djvRender2D/FontSystem.h>
#include <djvRender2D/Render.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/TimerFunc.h>

#include <djvMath/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct TimelinePIPWidget::Private
        {
            System::File::Info fileInfo;
            std::shared_ptr<AV::IO::IRead> read;
            AV::IO::Info info;
            Math::Frame::Sequence sequence;
            Math::IntRational speed;
            AV::Time::Units timeUnits = AV::Time::Units::First;
            glm::vec2 pipPos = glm::vec2(0.F, 0.F);
            Math::BBox2f timelineGeometry;
            Math::Frame::Index currentFrame = 0;
            std::shared_ptr<Image::Data> image;
            ImageData imageData;
            OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::Text::Label> timeLabel;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<System::Timer> timer;

            std::shared_ptr<Observer::Value<ImageData> > imageDataObserver;
            std::shared_ptr<Observer::Value<OCIO::Config> > ocioConfigObserver;
            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
        };

        void TimelinePIPWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::TimelinePIPWidget");

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setImageSizeRole(UI::MetricsRole::TextColumn);

            p.timeLabel = UI::Text::Label::create(context);
            p.timeLabel->setFontFamily(Render2D::Font::familyMono);
            p.timeLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.timeLabel->setBackgroundColorRole(UI::ColorRole::OverlayLight);
            p.timeLabel->setVAlign(UI::VAlign::Bottom);
            p.timeLabel->setMargin(UI::MetricsRole::Border);

            p.layout = UI::StackLayout::create(context);
            p.layout->setBackgroundColorRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.imageWidget);
            p.layout->addChild(p.timeLabel);
            addChild(p.layout);

            auto weak = std::weak_ptr<TimelinePIPWidget>(std::dynamic_pointer_cast<TimelinePIPWidget>(shared_from_this()));
            p.timer = System::Timer::create(context);
            p.timer->setRepeating(true);
            p.timer->start(
                System::getTimerDuration(System::TimerValue::VeryFast),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->read)
                        {
                            AV::IO::VideoFrame frame;
                            {
                                std::lock_guard<std::mutex> lock(widget->_p->read->getMutex());
                                const auto& videoQueue = widget->_p->read->getVideoQueue();
                                if (!videoQueue.isEmpty())
                                {
                                    frame = videoQueue.getFrame();
                                }
                            }
                            if (frame.data)
                            {
                                widget->_p->currentFrame = frame.frame;
                                widget->_p->image = frame.data;
                                widget->_p->imageWidget->setImage(frame.data);
                                widget->_textUpdate();
                            }
                        }
                        else if (widget->_p->imageWidget->getImage())
                        {
                            widget->_p->currentFrame = 0;
                            widget->_p->image.reset();
                            widget->_p->imageWidget->setImage(nullptr);
                            widget->_textUpdate();
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            p.imageDataObserver = Observer::Value<ImageData>::create(
                imageSettings->observeData(),
                [weak](const ImageData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageData = value;
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.ocioConfigObserver = Observer::Value<OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](const OCIO::Config& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                            widget->_p->ocioConfig = value;
                            widget->_p->outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                            widget->_widgetUpdate();
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = Observer::Value<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_textUpdate();
                    }
                });
        }

        TimelinePIPWidget::TimelinePIPWidget() :
            _p(new Private)
        {}

        TimelinePIPWidget::~TimelinePIPWidget()
        {}

        std::shared_ptr<TimelinePIPWidget> TimelinePIPWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<TimelinePIPWidget>(new TimelinePIPWidget);
            out->_init(context);
            return out;
        }

        void TimelinePIPWidget::setFileInfo(const System::File::Info& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (value == p.fileInfo)
                    return;
                p.fileInfo = value;
                if (!p.fileInfo.isEmpty())
                {
                    try
                    {
                        auto io = context->getSystemT<AV::IO::IOSystem>();
                        AV::IO::ReadOptions options;
                        options.videoQueueSize = 1;
                        options.audioQueueSize = 0;
                        p.read = io->read(value, options);
                        const auto info = p.read->getInfo().get();
                        p.speed = info.videoSpeed;
                        p.sequence = info.videoSequence;
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
                else
                {
                    p.read.reset();
                }
            }
        }

        void TimelinePIPWidget::setPos(const glm::vec2& value, Math::Frame::Index frame, const Math::BBox2f& timelineGeometry)
        {
            DJV_PRIVATE_PTR();
            if (value == p.pipPos && timelineGeometry == p.timelineGeometry)
                return;
            if (p.read)
            {
                p.read->seek(frame, AV::IO::Direction::Forward);
            }
            p.pipPos = value;
            p.timelineGeometry = timelineGeometry;
            _resize();
        }

        void TimelinePIPWidget::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const glm::vec2 size = p.layout->getMinimumSize();
            const glm::vec2 pos(
                Math::clamp(p.pipPos.x - floorf(size.x / 2.F), p.timelineGeometry.min.x, p.timelineGeometry.max.x - size.x),
                p.pipPos.y - size.y);
            p.layout->setGeometry(Math::BBox2f(pos.x, pos.y, size.x, size.y));
        }

        void TimelinePIPWidget::_paintEvent(System::Event::Paint& event)
        {
            UI::Widget::_paintEvent(event);
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            const auto& render = _getRender();
            render->setFillColor(style->getColor(UI::ColorRole::Shadow));
            for (const auto& i : getChildWidgets())
            {
                if (i->isVisible())
                {
                    Math::BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }
        }

        void TimelinePIPWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            Render2D::ImageOptions options;
            options.channelsDisplay = p.imageData.channelsDisplay;
            options.alphaBlend = Render2D::AlphaBlend::Straight;
            options.mirror = p.imageData.mirror;
            if (p.image)
            {
                auto i = p.ocioConfig.imageColorSpaces.find(p.image->getPluginName());
                if (i != p.ocioConfig.imageColorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.imageColorSpaces.find(std::string());
                    if (i != p.ocioConfig.imageColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
            }
            options.colorSpace.output = p.outputColorSpace;
            options.colorEnabled = p.imageData.colorEnabled;
            options.color = p.imageData.color;
            options.levelsEnabled = p.imageData.levelsEnabled;
            options.levels = p.imageData.levels;
            options.exposureEnabled = p.imageData.exposureEnabled;
            options.exposure = p.imageData.exposure;
            options.softClipEnabled = p.imageData.softClipEnabled;
            options.softClip = p.imageData.softClip;
            p.imageWidget->setImageOptions(options);
            p.imageWidget->setImageRotate(p.imageData.rotate);
            p.imageWidget->setImageAspectRatio(p.imageData.aspectRatio);
        }

        void TimelinePIPWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.timeLabel->setText(toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits));
            }
        }

    } // namespace ViewApp
} // namespace djv


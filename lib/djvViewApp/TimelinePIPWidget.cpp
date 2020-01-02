//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/TimelinePIPWidget.h>

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

#include <djvAV/AVSystem.h>
#include <djvAV/FontSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/Math.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct TimelinePIPWidget::Private
        {
            Core::FileSystem::FileInfo fileInfo;
            std::shared_ptr<AV::IO::IRead> read;
            AV::IO::Info info;
            Frame::Sequence sequence;
            Time::Speed speed;
            glm::vec2 pipPos = glm::vec2(0.F, 0.F);
            BBox2f timelineGeometry;
            Frame::Index currentFrame = 0;
            float imageAspectRatio = 0.F;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<UI::Label> timeLabel;
            std::shared_ptr<UI::StackLayout> layout;
            std::shared_ptr<Time::Timer> timer;
        };

        void TimelinePIPWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::TimelinePIPWidget");

            p.imageWidget = UI::ImageWidget::create(context);
            p.imageWidget->setSizeRole(UI::MetricsRole::TextColumn);

            p.timeLabel = UI::Label::create(context);
            p.timeLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
            p.timeLabel->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.timeLabel->setVAlign(UI::VAlign::Bottom);
            p.timeLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Border));

            p.layout = UI::StackLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            p.layout->addChild(p.imageWidget);
            p.layout->addChild(p.timeLabel);
            addChild(p.layout);

            auto weak = std::weak_ptr<TimelinePIPWidget>(std::dynamic_pointer_cast<TimelinePIPWidget>(shared_from_this()));
            p.timer = Time::Timer::create(context);
            p.timer->setRepeating(true);
            p.timer->start(
                Time::getTime(Time::TimerValue::VeryFast),
                [weak](const std::chrono::steady_clock::time_point&, const Time::Unit&)
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
                            if (frame.image)
                            {
                                widget->_p->currentFrame = frame.frame;
                                widget->_p->imageWidget->setImage(frame.image);
                                widget->_textUpdate();
                            }
                        }
                        else if (widget->_p->imageWidget->getImage())
                        {
                            widget->_p->currentFrame = 0;
                            widget->_p->imageWidget->setImage(nullptr);
                            widget->_textUpdate();
                        }
                    }
                });
        }

        TimelinePIPWidget::TimelinePIPWidget() :
            _p(new Private)
        {}

        TimelinePIPWidget::~TimelinePIPWidget()
        {}

        std::shared_ptr<TimelinePIPWidget> TimelinePIPWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimelinePIPWidget>(new TimelinePIPWidget);
            out->_init(context);
            return out;
        }

        void TimelinePIPWidget::setFileInfo(const Core::FileSystem::FileInfo& value)
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
                        auto io = context->getSystemT<AV::IO::System>();
                        AV::IO::ReadOptions options;
                        options.videoQueueSize = 1;
                        options.audioQueueSize = 0;
                        p.read = io->read(value, options);
                        const auto info = p.read->getInfo().get();
                        const auto& video = info.video;
                        if (video.size())
                        {
                            p.speed = video[0].speed;
                            p.sequence = video[0].sequence;
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _log(e.what(), LogLevel::Error);
                    }
                }
                else
                {
                    p.read.reset();
                }
            }
        }

        void TimelinePIPWidget::setPos(const glm::vec2& value, Frame::Index frame, const BBox2f& timelineGeometry)
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

        void TimelinePIPWidget::setImageAspectRatio(UI::ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            p.imageWidget->setImageAspectRatio(value);
        }

        void TimelinePIPWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const glm::vec2 size = p.layout->getMinimumSize();
            const glm::vec2 pos(
                Math::clamp(p.pipPos.x - floorf(size.x / 2.F), p.timelineGeometry.min.x, p.timelineGeometry.max.x - size.x),
                p.pipPos.y - size.y);
            p.layout->setGeometry(BBox2f(pos.x, pos.y, size.x, size.y));
        }

        void TimelinePIPWidget::_paintEvent(Event::Paint& event)
        {
            UI::Widget::_paintEvent(event);
            const auto& style = _getStyle();
            const float sh = style->getMetric(UI::MetricsRole::Shadow);
            auto render = _getRender();
            render->setFillColor(style->getColor(UI::ColorRole::Shadow));
            for (const auto& i : getChildWidgets())
            {
                if (i->isVisible())
                {
                    BBox2f g = i->getGeometry();
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

        void TimelinePIPWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto avSystem = context->getSystemT<AV::AVSystem>();
                p.timeLabel->setText(avSystem->getLabel(p.sequence.getFrame(p.currentFrame), p.speed));
            }
        }

    } // namespace ViewApp
} // namespace djv


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

#include <djvViewApp/TimelineSlider.h>

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
        namespace
        {
            class PIPWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PIPWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                PIPWidget()
                {}

            public:
                ~PIPWidget() override
                {}

                static std::shared_ptr<PIPWidget> create(const std::shared_ptr<Context>&);

                void setPIPFileInfo(const Core::FileSystem::FileInfo&);
                void setPIPPos(const glm::vec2&, Frame::Index, const BBox2f&);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                void _textUpdate();

                Core::FileSystem::FileInfo _fileInfo;
                std::shared_ptr<AV::IO::IRead> _read;
                AV::IO::Info _info;
                Frame::Sequence _sequence;
                Time::Speed _speed;
                glm::vec2 _pipPos = glm::vec2(0.F, 0.F);
                BBox2f _timelineGeometry;
                Frame::Index _currentFrame = 0;
                std::shared_ptr<UI::ImageWidget> _imageWidget;
                std::shared_ptr<UI::Label> _timeLabel;
                std::shared_ptr<UI::StackLayout> _layout;
                std::shared_ptr<Time::Timer> _timer;
            };

            void PIPWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::PIPWidget");

                _imageWidget = UI::ImageWidget::create(context);
                _imageWidget->setSizeRole(UI::MetricsRole::TextColumn);

                _timeLabel = UI::Label::create(context);
                _timeLabel->setFontSizeRole(UI::MetricsRole::FontSmall);
                _timeLabel->setBackgroundRole(UI::ColorRole::OverlayLight);
                _timeLabel->setVAlign(UI::VAlign::Bottom);
                _timeLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Border));

                _layout = UI::StackLayout::create(context);
                _layout->setBackgroundRole(UI::ColorRole::OverlayLight);
                _layout->addChild(_imageWidget);
                _layout->addChild(_timeLabel);
                addChild(_layout);

                auto weak = std::weak_ptr<PIPWidget>(std::dynamic_pointer_cast<PIPWidget>(shared_from_this()));
                _timer = Time::Timer::create(context);
                _timer->setRepeating(true);
                _timer->start(
                    Time::getMilliseconds(Time::TimerValue::Fast),
                    [weak](float)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_read)
                            {
                                AV::IO::VideoFrame frame;
                                {
                                    std::lock_guard<std::mutex> lock(widget->_read->getMutex());
                                    const auto& videoQueue = widget->_read->getVideoQueue();
                                    if (!videoQueue.isEmpty())
                                    {
                                        frame = videoQueue.getFrame();
                                    }
                                }
                                if (frame.image)
                                {
                                    widget->_currentFrame = frame.frame;
                                    widget->_imageWidget->setImage(frame.image);
                                    widget->_textUpdate();
                                }
                            }
                            else if (widget->_imageWidget->getImage())
                            {
                                widget->_currentFrame = 0;
                                widget->_imageWidget->setImage(nullptr);
                                widget->_textUpdate();
                            }
                        }
                    });
            }

            std::shared_ptr<PIPWidget> PIPWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<PIPWidget>(new PIPWidget);
                out->_init(context);
                return out;
            }

            void PIPWidget::setPIPFileInfo(const Core::FileSystem::FileInfo& value)
            {
                if (auto context = getContext().lock())
                {
                    if (value == _fileInfo)
                        return;
                    _fileInfo = value;
                    if (!_fileInfo.isEmpty())
                    {
                        try
                        {
                            auto io = context->getSystemT<AV::IO::System>();
                            AV::IO::ReadOptions options;
                            options.videoQueueSize = 1;
                            options.audioQueueSize = 0;
                            _read = io->read(value, options);
                            const auto info = _read->getInfo().get();
                            const auto& video = info.video;
                            if (video.size())
                            {
                                _speed = video[0].speed;
                                _sequence = video[0].sequence;
                            }
                        }
                        catch (const std::exception& e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                    }
                    else
                    {
                        _read.reset();
                    }
                }
            }

            void PIPWidget::setPIPPos(const glm::vec2& value, Frame::Index frame, const BBox2f& timelineGeometry)
            {
                if (value == _pipPos && timelineGeometry == _timelineGeometry)
                    return;
                if (_read)
                {
                    _read->seek(frame, AV::IO::Direction::Forward);
                }
                _pipPos = value;
                _timelineGeometry = timelineGeometry;
                _resize();
            }

            void PIPWidget::_layoutEvent(Event::Layout&)
            {
                const glm::vec2 size = _layout->getMinimumSize();
                const glm::vec2 pos(
                    Math::clamp(_pipPos.x - floorf(size.x / 2.F), _timelineGeometry.min.x, _timelineGeometry.max.x - size.x),
                    _pipPos.y - size.y);
                _layout->setGeometry(BBox2f(pos.x, pos.y, size.x, size.y));
            }

            void PIPWidget::_paintEvent(Event::Paint& event)
            {
                UI::Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(UI::MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(style->getColor(UI::ColorRole::Shadow));
                for (const auto& i : getChildrenT<UI::Widget>())
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

            void PIPWidget::_textUpdate()
            {
                if (auto context = getContext().lock())
                {
                    auto avSystem = context->getSystemT<AV::AVSystem>();
                    _timeLabel->setText(avSystem->getLabel(_sequence.getFrame(_currentFrame), _speed));
                }
            }

        } // namespace

        struct TimelineSlider::Private
        {
            std::shared_ptr<AV::Font::System> fontSystem;
            std::shared_ptr<Media> media;
            Time::Speed speed;
            Frame::Sequence sequence;
            Frame::Index currentFrame = 0;
            bool inOutPointsEnabled = false;
            Frame::Index inPoint = Frame::invalidIndex;
            Frame::Index outPoint = Frame::invalidIndex;
            bool cacheEnabled = false;
            Frame::Sequence cacheSequence;
            Frame::Sequence cachedFrames;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            std::string currentFrameText;
            float currentFrameLength = 0.F;
            std::future<glm::vec2> currentFrameSizeFuture;
            float maxFrameLength = 0.F;
            std::future<glm::vec2> maxFrameSizeFuture;
            uint32_t pressedID = Event::InvalidID;
            bool pip = true;
            AV::TimeUnits timeUnits = AV::TimeUnits::First;
            std::shared_ptr<PIPWidget> pipWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::function<void(Frame::Index)> currentFrameCallback;
            std::function<void(bool)> currentFrameDragCallback;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver;
            std::shared_ptr<ValueObserver<bool> > pipObserver;
            std::shared_ptr<ValueObserver<AV::TimeUnits> > timeUnitsObserver;
            BBox2f geometryPrev = BBox2f(0.F, 0.F, -1.F, -1.F);
            struct TimeTick
            {
                BBox2f geometry;
                std::string text;
                glm::vec2 textPos;
            };
            std::vector<TimeTick> timeTicks;
        };

        void TimelineSlider::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSlider");

            p.fontSystem = context->getSystemT<AV::Font::System>();

            p.pipWidget = PIPWidget::create(context);
            p.overlay = UI::Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setCapturePointer(false);
            p.overlay->setBackgroundRole(UI::ColorRole::None);
            p.overlay->addChild(p.pipWidget);

            auto weak = std::weak_ptr<TimelineSlider>(std::dynamic_pointer_cast<TimelineSlider>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.pipObserver = ValueObserver<bool>::create(
                    playbackSettings->observePIP(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->pip = value;
                    }
                });
            }

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = ValueObserver<AV::TimeUnits>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::TimeUnits value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_textUpdate();
                        widget->_currentFrameUpdate();
                    }
                });
        }

        TimelineSlider::TimelineSlider() :
            _p(new Private)
        {}

        TimelineSlider::~TimelineSlider()
        {}

        std::shared_ptr<TimelineSlider> TimelineSlider::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<TimelineSlider>(new TimelineSlider);
            out->_init(context);
            return out;
        }

        void TimelineSlider::setMedia(const std::shared_ptr<Media> & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.media)
                return;
            p.media = value;
            if (p.media)
            {
                auto weak = std::weak_ptr<TimelineSlider>(std::dynamic_pointer_cast<TimelineSlider>(shared_from_this()));
                p.infoObserver = ValueObserver<AV::IO::Info>::create(
                    p.media->observeInfo(),
                    [weak](const AV::IO::Info & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value.video.size() ? value.video[0].speed : Time::Speed();
                        widget->_textUpdate();
                        widget->_currentFrameUpdate();
                    }
                });

                p.speedObserver = ValueObserver<Time::Speed>::create(
                    p.media->observeDefaultSpeed(),
                    [weak](const Time::Speed& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->speed = value;
                            widget->_textUpdate();
                            widget->_currentFrameUpdate();
                        }
                    });

                p.sequenceObserver = ValueObserver<Frame::Sequence>::create(
                    p.media->observeSequence(),
                    [weak](const Frame::Sequence& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->sequence = value;
                            widget->_textUpdate();
                            widget->_currentFrameUpdate();
                        }
                    });

                p.currentFrameObserver = ValueObserver<Frame::Index>::create(
                    p.media->observeCurrentFrame(),
                    [weak](Frame::Index value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentFrame = value;
                            widget->_currentFrameUpdate();
                        }
                    });
            }
            else
            {
                p.sequence = Frame::Sequence();
                p.currentFrame = 0;
                p.speed = Time::Speed();

                p.pipWidget->setPIPFileInfo(Core::FileSystem::FileInfo());

                p.infoObserver.reset();
                p.speedObserver.reset();
                p.sequenceObserver.reset();
                p.currentFrameObserver.reset();
            }
            _textUpdate();
            _currentFrameUpdate();
        }

        void TimelineSlider::setInOutPointsEnabled(bool value)
        {
            if (value == _p->inOutPointsEnabled)
                return;
            _p->inOutPointsEnabled = value;
            _redraw();
        }

        void TimelineSlider::setInPoint(Frame::Index value)
        {
            if (value == _p->inPoint)
                return;
            _p->inPoint = value;
            _redraw();
        }

        void TimelineSlider::setOutPoint(Frame::Index value)
        {
            if (value == _p->outPoint)
                return;
            _p->outPoint = value;
            _redraw();
        }

        void TimelineSlider::setCacheEnabled(bool value)
        {
            if (value == _p->cacheEnabled)
                return;
            _p->cacheEnabled = value;
            _redraw();
        }

        void TimelineSlider::setCacheSequence(const Frame::Sequence& value)
        {
            if (value == _p->cacheSequence)
                return;
            _p->cacheSequence = value;
            _redraw();
        }

        void TimelineSlider::setCachedFrames(const Frame::Sequence& value)
        {
            if (value == _p->cachedFrames)
                return;
            _p->cachedFrames = value;
            _redraw();
        }

        void TimelineSlider::setCurrentFrameCallback(const std::function<void(Frame::Index)>& value)
        {
            _p->currentFrameCallback = value;
        }

        void TimelineSlider::setCurrentFrameDragCallback(const std::function<void(bool)>& value)
        {
            _p->currentFrameDragCallback = value;
        }

        void TimelineSlider::_styleEvent(Event::Style &)
        {
            _textUpdate();
            _currentFrameUpdate();
        }

        void TimelineSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid())
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.currentFrameSizeFuture.valid())
            {
                try
                {
                    p.currentFrameLength = p.currentFrameSizeFuture.get().x;
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            if (p.maxFrameSizeFuture.valid())
            {
                try
                {
                    p.maxFrameLength = p.maxFrameSizeFuture.get().x;
                }
                catch (const std::exception& e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const float b = style->getMetric(UI::MetricsRole::Border);
            glm::vec2 size = glm::vec2(0.F, 0.F);
            size.x = style->getMetric(UI::MetricsRole::TextColumn);
            size.y = p.fontMetrics.lineHeight * 2.F + b * 6.F + m * 2.F;
            _setMinimumSize(size);
        }

        void TimelineSlider::_layoutEvent(Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            const BBox2f& g = getGeometry();
            if (p.geometryPrev != g)
            {
                p.geometryPrev = g;
                if (auto context = getContext().lock())
                {
                    const float w = g.w();
                    const auto& style = _getStyle();
                    const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                    const float b = style->getMetric(UI::MetricsRole::Border);
                    float x = g.min.x + m;
                    float x2 = x;
                    //! \bug Why the extra subtract by one here?
                    float y = g.max.y - m - b * 6.F - p.fontMetrics.lineHeight + p.fontMetrics.ascender - 1.F;
                    const float speedF = p.speed.toFloat();
                    auto avSystem = context->getSystemT<AV::AVSystem>();
                    const std::vector<std::pair<float, std::function<Frame::Index(size_t, float)> > > units =
                    {
                        { _getHourLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * 60.F * 60.F * speed); } },
                        { _getMinuteLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * 60.F * speed); } },
                        { _getSecondLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * speed); } },
                        { _getFrameLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value); } }
                    };
                    p.timeTicks.clear();
                    for (const auto& i : units)
                    {
                        if (i.first < w)
                        {
                            size_t unit = 0;
                            const size_t sequenceSize = p.sequence.getSize();
                            while (x < g.max.x && unit < sequenceSize)
                            {
                                if (x >= x2)
                                {
                                    Private::TimeTick tick;
                                    tick.geometry = BBox2f(
                                        x,
                                        g.max.y - b * 6.F - p.fontMetrics.lineHeight,
                                        b,
                                        p.fontMetrics.lineHeight);
                                    tick.text = avSystem->getLabel(p.sequence.getFrame(i.second(unit, speedF)), p.speed);
                                    tick.textPos = glm::vec2(x + m, y);
                                    p.timeTicks.push_back(tick);
                                    x2 = x + p.maxFrameLength + m * 2.F;
                                }
                                ++unit;
                                x = _frameToPos(i.second(unit, speedF));
                            }
                            break;
                        }
                    }
                }
            }
        }

        void TimelineSlider::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const BBox2f& g = getGeometry();
                const auto& style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const float b = style->getMetric(UI::MetricsRole::Border);
                const BBox2f& hg = _getHandleGeometry();

                // Draw the time ticks.
                auto color = style->getColor(UI::ColorRole::Foreground);
                color.setF32(color.getF32(3) * .4F, 3);
                auto render = _getRender();
                render->setFillColor(color);
                const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
                render->setCurrentFont(fontInfo);
                for (const auto& tick : p.timeTicks)
                {
                    render->drawRect(tick.geometry);
                    render->drawText(tick.text, glm::vec2(floorf(tick.textPos.x), floorf(tick.textPos.y)));
                }

                // Draw the in/out points.
                if (p.inOutPointsEnabled)
                {
                    auto color = style->getColor(UI::ColorRole::Checked);
                    render->setFillColor(color);
                    const float x0 = _frameToPos(p.inPoint);
                    const float x1 = _frameToPos(p.outPoint + 1);
                    render->drawRect(BBox2f(
                        x0,
                        g.max.y - m - b * 6.F,
                        x1 - x0,
                        b * 2.F));
                }

                // Draw the cached frames.
                if (p.cacheEnabled)
                {
                    color = style->getColor(UI::ColorRole::Checked);
                    render->setFillColor(color);
                    for (const auto& i : p.cacheSequence.ranges)
                    {
                        const float x0 = _frameToPos(i.min);
                        const float x1 = _frameToPos(i.max + 1);
                        render->drawRect(BBox2f(
                            x0,
                            g.max.y - m - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                    color = style->getColor(UI::ColorRole::Cached);
                    render->setFillColor(color);
                    for (const auto& i : p.cachedFrames.ranges)
                    {
                        const float x0 = _frameToPos(i.min);
                        const float x1 = _frameToPos(i.max + 1);
                        render->drawRect(BBox2f(
                            x0,
                            g.max.y - m - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                }

                // Draw the frame ticks.
                const size_t sequenceSize = p.sequence.getSize();
                if (_getFrameLength() > b * 2.F)
                {
                    auto color = style->getColor(UI::ColorRole::Foreground);
                    color.setF32(color.getF32(3) * .4F, 3);
                    render->setFillColor(color);
                    for (Frame::Index f2 = 0; f2 < sequenceSize; ++f2)
                    {
                        const float x = _frameToPos(f2);
                        render->drawRect(BBox2f(
                            x,
                            g.max.y - m - b * 6.F,
                            b,
                            b * 6.F));
                    }
                }

                // Draw the current frame.
                color = style->getColor(UI::ColorRole::Foreground);
                color.setF32(color.getF32(3) * .5F, 3);
                render->setFillColor(color);
                render->drawRect(hg);
                color = style->getColor(UI::ColorRole::Foreground);
                render->setFillColor(color);
                float frameLeftPos = hg.min.x - m - p.currentFrameLength;
                float frameRightPos = hg.max.x + m;
                //! \bug Why the extra subtract by one here?
                const float frameY = g.min.y + m + p.fontMetrics.ascender - 1.F;
                if ((frameRightPos + p.currentFrameLength) > g.max.x)
                {
                    render->drawText(p.currentFrameText, glm::vec2(floorf(frameLeftPos), floorf(frameY)));
                }
                else
                {
                    render->drawText(p.currentFrameText, glm::vec2(floorf(frameRightPos), floorf(frameY)));
                }
            }
        }

        void TimelineSlider::_pointerEnterEvent(Event::PointerEnter & event)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!event.isRejected())
                {
                    event.accept();
                    if (p.pip && isEnabled())
                    {
                        p.pipWidget->setPIPFileInfo(p.media->getFileInfo());
                        if (auto window = getWindow())
                        {
                            window->addChild(p.overlay);
                            p.overlay->moveToFront();
                            p.overlay->show();
                        }
                    }
                }
            }
        }

        void TimelineSlider::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            p.overlay->hide();
        }

        void TimelineSlider::_pointerMoveEvent(Event::PointerMove & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto & pos = event.getPointerInfo().projectedPos;
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const Frame::Index frame = _posToFrame(static_cast<int>(pos.x - g.min.x - m));
            if (p.pipWidget)
            {
                if (auto parent = getParentRecursiveT<MediaWidget>())
                {
                    const auto& style = _getStyle();
                    const float s = style->getMetric(UI::MetricsRole::Spacing);
                    p.pipWidget->setPIPPos(glm::vec2(pos.x, g.min.y - s), frame, parent->getGeometry().margin(-s));
                }
            }
            if (p.pressedID)
            {
                p.currentFrame = frame;
                _currentFrameUpdate();
                _doCurrentFrameCallback();
            }
        }

        void TimelineSlider::_buttonPressEvent(Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            const auto id = event.getPointerInfo().id;
            const auto & pos = event.getPointerInfo().projectedPos;
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            event.accept();
            p.pressedID = id;
            p.currentFrame = _posToFrame(static_cast<int>(pos.x - g.min.x - m));
            _currentFrameUpdate();
            _doCurrentFrameDragCallback(true);
            _doCurrentFrameCallback();
        }

        void TimelineSlider::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getPointerInfo().id != p.pressedID)
                return;
            event.accept();
            p.pressedID = Event::InvalidID;
            _redraw();
            _doCurrentFrameDragCallback(false);
        }

        void TimelineSlider::_updateEvent(Event::Update & event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid())
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
        }

        Frame::Index TimelineSlider::_posToFrame(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const float v = value / (g.w() - m * 2.F);
            const size_t sequenceSize = p.sequence.getSize();
            Frame::Index out = sequenceSize ?
                Math::clamp(
                    static_cast<Frame::Index>(v * sequenceSize),
                    static_cast<Frame::Index>(0),
                    static_cast<Frame::Index>(sequenceSize - 1)) :
                0;
            return out;
        }

        float TimelineSlider::_frameToPos(Frame::Index value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f& g = getGeometry();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const size_t sequenceSize = p.sequence.getSize();
            const float v = sequenceSize ? (value / static_cast<float>(sequenceSize)) : 0.F;
            float out = g.min.x + m + v * (g.w() - m * 2.F);
            return out;
        }

        float TimelineSlider::_getFrameLength() const
        {
            const Frame::Index f = 1;
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getSecondLength() const
        {
            const Frame::Index f = static_cast<Frame::Index>(_p->speed.toFloat());
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getMinuteLength() const
        {
            const Frame::Index f = static_cast<Frame::Index>(_p->speed.toFloat() * 60.F);
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getHourLength() const
        {
            const Frame::Index f = static_cast<Frame::Index>(_p->speed.toFloat() * 60.F * 60.F);
            return _frameToPos(f) - _frameToPos(0);
        }

        BBox2f TimelineSlider::_getHandleGeometry() const
        {
            DJV_PRIVATE_PTR();
            const BBox2f & g = getGeometry();
            const auto& style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float x0 = _frameToPos(p.currentFrame);
            const float x1 = _frameToPos(p.currentFrame + 1);
            BBox2f out = BBox2f(
                floorf(x0),
                g.min.y + m,
                ceilf(std::max(x1 - x0, b)),
                g.h() - m * 2.F);
            return out;
        }

        void TimelineSlider::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
                p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
                std::string maxFrameText;
                switch (p.timeUnits)
                {
                case AV::TimeUnits::Timecode:
                    maxFrameText = "00:00:00:00";
                    break;
                case AV::TimeUnits::Frames:
                {
                    const size_t rangesSize = p.sequence.ranges.size();
                    if (rangesSize > 0)
                    {
                        maxFrameText = std::string(Math::getNumDigits(p.sequence.ranges[rangesSize - 1].max), '0');
                    }
                    break;
                }
                default: break;
                }
                p.maxFrameSizeFuture = p.fontSystem->measure(maxFrameText, fontInfo);
                p.geometryPrev = BBox2f(0.F, 0.F, -1.F, -1.F);
                _resize();
            }
        }

        void TimelineSlider::_currentFrameUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
                auto avSystem = context->getSystemT<AV::AVSystem>();
                p.currentFrameText = avSystem->getLabel(p.sequence.getFrame(p.currentFrame), p.speed);
                p.currentFrameSizeFuture = p.fontSystem->measure(p.currentFrameText, fontInfo);
                _resize();
            }
        }

        void TimelineSlider::_doCurrentFrameCallback()
        {
            DJV_PRIVATE_PTR();
            if (p.currentFrameCallback)
            {
                p.currentFrameCallback(p.currentFrame);
            }
        }

        void TimelineSlider::_doCurrentFrameDragCallback(bool value)
        {
            DJV_PRIVATE_PTR();
            if (p.currentFrameDragCallback)
            {
                p.currentFrameDragCallback(value);
            }
        }

    } // namespace ViewApp
} // namespace djv


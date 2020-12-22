// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelineSlider.h>

#include <djvViewApp/Media.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/TimelinePIPWidget.h>
#include <djvViewApp/TimelineWidget.h>

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

#include <djvSystem/Context.h>
#include <djvSystem/Timer.h>

#include <djvMath/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct TimelineSlider::Private
        {
            std::shared_ptr<Render2D::Font::FontSystem> fontSystem;
            std::shared_ptr<Media> media;
            Math::IntRational speed;
            Math::Frame::Sequence sequence;
            Math::Frame::Index currentFrame = 0;
            bool inOutPointsEnabled = false;
            Math::Frame::Index inPoint = Math::Frame::invalidIndex;
            Math::Frame::Index outPoint = Math::Frame::invalidIndex;
            bool cacheEnabled = false;
            Math::Frame::Sequence cacheSequence;
            Math::Frame::Sequence cachedFrames;
            Render2D::Font::FontInfo fontInfo;
            Render2D::Font::Metrics fontMetrics;
            std::future<Render2D::Font::Metrics> fontMetricsFuture;
            std::string currentFrameText;
            float currentFrameLength = 0.F;
            std::future<glm::vec2> currentFrameSizeFuture;
            std::vector<std::shared_ptr<Render2D::Font::Glyph> > currentFrameGlyphs;
            std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > currentFrameGlyphsFuture;
            float maxFrameLength = 0.F;
            std::future<glm::vec2> maxFrameSizeFuture;
            uint32_t pressedID = System::Event::invalidID;
            glm::vec2 pointerReleasePos = glm::vec2(0.F, 0.F);
            bool pipEnabled = true;
            AV::Time::Units timeUnits = AV::Time::Units::First;
            std::shared_ptr<TimelinePIPWidget> pipWidget;
            std::shared_ptr<UI::Layout::Overlay> pipOverlay;
            std::function<void(Math::Frame::Index)> currentFrameCallback;
            std::function<void(bool)> currentFrameDragCallback;
            std::shared_ptr<Observer::Value<AV::IO::Info> > infoObserver;
            std::shared_ptr<Observer::Value<Math::IntRational> > speedObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<Observer::Value<Math::Frame::Index> > currentFrameObserver;
            std::shared_ptr<Observer::Value<bool> > pipEnabledObserver;
            std::shared_ptr<Observer::Value<AV::Time::Units> > timeUnitsObserver;
            glm::vec2 sizePrev = glm::vec2(0.F, 0.F);
            struct TimeTick
            {
                glm::vec2 pos;
                glm::vec2 size;
                std::string text;
                glm::vec2 textPos;
                std::vector<std::shared_ptr<Render2D::Font::Glyph> > glyphs;
                std::future<std::vector<std::shared_ptr<Render2D::Font::Glyph> > > glyphsFuture;
            };
            std::vector<std::shared_ptr<TimeTick> > timeTicks;
        };

        void TimelineSlider::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::TimelineSlider");
            setBackgroundRole(UI::ColorRole::Trough);

            p.fontSystem = context->getSystemT<Render2D::Font::FontSystem>();

            p.pipWidget = TimelinePIPWidget::create(context);
            p.pipOverlay = UI::Layout::Overlay::create(context);
            p.pipOverlay->setCaptureKeyboard(false);
            p.pipOverlay->setCapturePointer(false);
            p.pipOverlay->setBackgroundRole(UI::ColorRole::None);
            p.pipOverlay->addChild(p.pipWidget);
            p.pipOverlay->hide();

            auto weak = std::weak_ptr<TimelineSlider>(std::dynamic_pointer_cast<TimelineSlider>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            if (auto playbackSettings = settingsSystem->getSettingsT<PlaybackSettings>())
            {
                p.pipEnabledObserver = Observer::Value<bool>::create(
                    playbackSettings->observePIPEnabled(),
                    [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->pipEnabled = value;
                    }
                });
            }

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.timeUnitsObserver = Observer::Value<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
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

        std::shared_ptr<TimelineSlider> TimelineSlider::create(const std::shared_ptr<System::Context>& context)
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
                p.infoObserver = Observer::Value<AV::IO::Info>::create(
                    p.media->observeInfo(),
                    [weak](const AV::IO::Info & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value.videoSpeed;
                        widget->_textUpdate();
                        widget->_currentFrameUpdate();
                    }
                });

                p.speedObserver = Observer::Value<Math::IntRational>::create(
                    p.media->observeDefaultSpeed(),
                    [weak](const Math::IntRational& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->speed = value;
                            widget->_textUpdate();
                            widget->_currentFrameUpdate();
                        }
                    });

                p.sequenceObserver = Observer::Value<Math::Frame::Sequence>::create(
                    p.media->observeSequence(),
                    [weak](const Math::Frame::Sequence& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->sequence = value;
                            widget->_textUpdate();
                            widget->_currentFrameUpdate();
                        }
                    });

                p.currentFrameObserver = Observer::Value<Math::Frame::Index>::create(
                    p.media->observeCurrentFrame(),
                    [weak](Math::Frame::Index value)
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
                p.sequence = Math::Frame::Sequence();
                p.currentFrame = 0;
                p.speed = Math::IntRational();

                p.pipWidget->setFileInfo(System::File::Info());

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

        void TimelineSlider::setInPoint(Math::Frame::Index value)
        {
            if (value == _p->inPoint)
                return;
            _p->inPoint = value;
            _redraw();
        }

        void TimelineSlider::setOutPoint(Math::Frame::Index value)
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

        void TimelineSlider::setCacheSequence(const Math::Frame::Sequence& value)
        {
            if (value == _p->cacheSequence)
                return;
            _p->cacheSequence = value;
            _redraw();
        }

        void TimelineSlider::setCachedFrames(const Math::Frame::Sequence& value)
        {
            if (value == _p->cachedFrames)
                return;
            _p->cachedFrames = value;
            _redraw();
        }

        void TimelineSlider::setCurrentFrameCallback(const std::function<void(Math::Frame::Index)>& value)
        {
            _p->currentFrameCallback = value;
        }

        void TimelineSlider::setCurrentFrameDragCallback(const std::function<void(bool)>& value)
        {
            _p->currentFrameDragCallback = value;
        }

        void TimelineSlider::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            glm::vec2 size = glm::vec2(0.F, 0.F);
            size.y = p.fontMetrics.lineHeight * 2.F + b * 6.F;
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void TimelineSlider::_layoutEvent(System::Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const glm::vec2 size = g.getSize();
            if (p.sizePrev != size)
            {
                p.sizePrev = size;
                if (auto context = getContext().lock())
                {
                    const float w = g.w();
                    const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                    const float b = style->getMetric(UI::MetricsRole::Border);
                    float x = g.min.x;
                    float x2 = x;
                    //! \bug Why the extra subtract by one here?
                    const float textY = size.y - b * 6.F - p.fontMetrics.lineHeight + p.fontMetrics.ascender - 1.F;
                    const float speedF = p.speed.toFloat();
                    auto avSystem = context->getSystemT<AV::AVSystem>();
                    const std::vector<std::pair<float, std::function<Math::Frame::Index(size_t, float)> > > units =
                    {
                        { _getHourLength(), [](size_t value, float speed) { return static_cast<Math::Frame::Index>(value * 60.F * 60.F * speed); } },
                        { _getMinuteLength(), [](size_t value, float speed) { return static_cast<Math::Frame::Index>(value * 60.F * speed); } },
                        { _getSecondLength(), [](size_t value, float speed) { return static_cast<Math::Frame::Index>(value * speed); } },
                        { _getFrameLength(), [](size_t value, float speed) { return static_cast<Math::Frame::Index>(value); } }
                    };
                    size_t timeTicksCount = 0;
                    for (const auto& i : units)
                    {
                        if (i.first < w)
                        {
                            size_t unit = 0;
                            const size_t sequenceFrameCount = p.sequence.getFrameCount();
                            while (x < g.max.x && unit < sequenceFrameCount)
                            {
                                if (x >= x2)
                                {
                                    std::shared_ptr<Private::TimeTick> tick;
                                    if (timeTicksCount < p.timeTicks.size())
                                    {
                                        tick = p.timeTicks[timeTicksCount];
                                    }
                                    else
                                    {
                                        tick = std::shared_ptr<Private::TimeTick>(new Private::TimeTick);
                                        p.timeTicks.push_back(tick);
                                    }
                                    tick->pos.x = x - g.min.x;
                                    tick->pos.y = size.y - b * 6.F - p.fontMetrics.lineHeight;
                                    tick->size.x = b;
                                    tick->size.y = p.fontMetrics.lineHeight;
                                    tick->text = AV::Time::toString(p.sequence.getFrame(i.second(unit, speedF)), p.speed, p.timeUnits);
                                    tick->glyphsFuture = p.fontSystem->getGlyphs(tick->text, p.fontInfo);
                                    tick->textPos = glm::vec2(x + tick->size.x + m - g.min.x, textY);
                                    x2 = x + p.maxFrameLength + m * 2.F;
                                    ++timeTicksCount;
                                }
                                ++unit;
                                x = _frameToPos(i.second(unit, speedF));
                            }
                            break;
                        }
                    }
                    p.timeTicks.resize(timeTicksCount);
                }
            }
        }

        void TimelineSlider::_paintEvent(System::Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const float b = style->getMetric(UI::MetricsRole::Border);
                const Math::BBox2f& hg = _getHandleGeometry();

                // Draw the time ticks.
                auto color = style->getColor(UI::ColorRole::Foreground);
                color.setF32(color.getF32(3) * .4F, 3);
                const auto& render = _getRender();
                render->setFillColor(color);
                std::vector<Math::BBox2f> rects;
                for (const auto& tick : p.timeTicks)
                {
                    rects.emplace_back(Math::BBox2f(
                        floorf(g.min.x + tick->pos.x),
                        floorf(g.min.y + tick->pos.y),
                        ceilf(tick->size.x),
                        ceilf(tick->size.y)));
                    if (tick->glyphs.size())
                    {
                        render->drawText(
                            tick->glyphs,
                            glm::vec2(floorf(g.min.x + tick->textPos.x), floorf(g.min.y + tick->textPos.y)));
                    }
                }
                render->drawRects(rects);

                // Draw the in/out points.
                if (p.inOutPointsEnabled)
                {
                    auto color = style->getColor(UI::ColorRole::Checked);
                    render->setFillColor(color);
                    const float x0 = _frameToPos(p.inPoint);
                    const float x1 = _frameToPos(p.outPoint + 1);
                    render->drawRect(Math::BBox2f(
                        x0,
                        g.max.y - b * 6.F,
                        x1 - x0,
                        b * 2.F));
                }

                // Draw the cached frames.
                if (p.cacheEnabled)
                {
                    color = style->getColor(UI::ColorRole::Checked);
                    render->setFillColor(color);
                    rects.clear();
                    for (const auto& i : p.cacheSequence.getRanges())
                    {
                        const float x0 = _frameToPos(i.getMin());
                        const float x1 = _frameToPos(i.getMax() + 1);
                        rects.emplace_back(Math::BBox2f(
                            x0,
                            g.max.y - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                    render->drawRects(rects);
                    color = style->getColor(UI::ColorRole::Cached);
                    render->setFillColor(color);
                    rects.clear();
                    for (const auto& i : p.cachedFrames.getRanges())
                    {
                        const float x0 = _frameToPos(i.getMin());
                        const float x1 = _frameToPos(i.getMax() + 1);
                        rects.emplace_back(Math::BBox2f(
                            x0,
                            g.max.y - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                    render->drawRects(rects);
                }

                // Draw the frame ticks.
                const size_t sequenceFrameCount = p.sequence.getFrameCount();
                if (_getFrameLength() > b * 2.F)
                {
                    auto color = style->getColor(UI::ColorRole::Foreground);
                    color.setF32(color.getF32(3) * .4F, 3);
                    render->setFillColor(color);
                    rects.clear();
                    for (Math::Frame::Index f2 = 0; f2 < sequenceFrameCount; ++f2)
                    {
                        const float x = _frameToPos(f2);
                        rects.emplace_back(Math::BBox2f(
                            x,
                            g.max.y - b * 6.F,
                            b,
                            b * 6.F));
                    }
                    render->drawRects(rects);
                }

                // Draw the current frame.
                if (p.currentFrameGlyphs.size())
                {
                    color = style->getColor(UI::ColorRole::Foreground);
                    color.setF32(color.getF32(3) * .5F, 3);
                    render->setFillColor(color);
                    render->drawRect(hg);
                    color = style->getColor(UI::ColorRole::Foreground);
                    render->setFillColor(color);
                    float frameLeftPos = hg.min.x - m - p.currentFrameLength;
                    float frameRightPos = hg.max.x + m;
                    //! \bug Why the extra subtract by one here?
                    const float frameY = g.min.y + p.fontMetrics.ascender - 1.F;
                    if ((frameRightPos + p.currentFrameLength) > g.max.x)
                    {
                        render->drawText(p.currentFrameGlyphs, glm::vec2(floorf(frameLeftPos), floorf(frameY)));
                    }
                    else
                    {
                        render->drawText(p.currentFrameGlyphs, glm::vec2(floorf(frameRightPos), floorf(frameY)));
                    }
                }
            }
        }

        void TimelineSlider::_pointerEnterEvent(System::Event::PointerEnter & event)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!event.isRejected())
                {
                    event.accept();
                    if (_isPIPEnabled())
                    {
                        p.pipWidget->setFileInfo(p.media->getFileInfo());
                        _showPIP(true);
                    }
                }
            }
        }

        void TimelineSlider::_pointerLeaveEvent(System::Event::PointerLeave & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            _showPIP(false);
        }

        void TimelineSlider::_pointerMoveEvent(System::Event::PointerMove & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            const auto & pos = event.getPointerInfo().projectedPos;
            const auto& style = _getStyle();
            if (_isPIPEnabled() &&
                glm::length(pos - p.pointerReleasePos) > style->getMetric(UI::MetricsRole::Handle))
            {
                _showPIP(true);
            }
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const Math::Frame::Index frame = _posToFrame(static_cast<int>(pos.x - g.min.x));
            if (auto parent = getParentRecursiveT<TimelineWidget>())
            {
                const auto& style = _getStyle();
                const float s = style->getMetric(UI::MetricsRole::Spacing);
                p.pipWidget->setPos(glm::vec2(pos.x, g.min.y - s), frame, parent->getGeometry().margin(-s));
            }
            if (p.pressedID)
            {
                p.currentFrame = frame;
                _currentFrameUpdate();
                _doCurrentFrameCallback();
            }
        }

        void TimelineSlider::_buttonPressEvent(System::Event::ButtonPress & event)
        {
            DJV_PRIVATE_PTR();
            if (p.pressedID)
                return;
            event.accept();
            const auto id = event.getPointerInfo().id;
            const auto & pos = event.getPointerInfo().projectedPos;
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            p.pressedID = id;
            p.currentFrame = _posToFrame(static_cast<int>(pos.x - g.min.x));
            _currentFrameUpdate();
            _showPIP(true);
            _doCurrentFrameDragCallback(true);
            _doCurrentFrameCallback();
        }

        void TimelineSlider::_buttonReleaseEvent(System::Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getPointerInfo().id != p.pressedID)
                return;
            event.accept();
            p.pressedID = System::Event::invalidID;
            p.pointerReleasePos = event.getPointerInfo().projectedPos;
            _redraw();
            _showPIP(false);
            _doCurrentFrameDragCallback(false);
        }

        void TimelineSlider::_initEvent(System::Event::Init & event)
        {
            if (event.getData().resize ||
                event.getData().font ||
                event.getData().text)
            {
                _textUpdate();
                _currentFrameUpdate();
            }
        }

        void TimelineSlider::_updateEvent(System::Event::Update & event)
        {
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid() &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    p.sizePrev = glm::vec2(0.F, 0.F);
                    _resize();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.currentFrameSizeFuture.valid() &&
                p.currentFrameSizeFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.currentFrameLength = p.currentFrameSizeFuture.get().x;
                    p.sizePrev = glm::vec2(0.F, 0.F);
                    _resize();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.currentFrameGlyphsFuture.valid() &&
                p.currentFrameGlyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.currentFrameGlyphs = p.currentFrameGlyphsFuture.get();
                    p.sizePrev = glm::vec2(0.F, 0.F);
                    _resize();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            if (p.maxFrameSizeFuture.valid() &&
                p.maxFrameSizeFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.maxFrameLength = p.maxFrameSizeFuture.get().x;
                    p.sizePrev = glm::vec2(0.F, 0.F);
                    _resize();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), System::LogLevel::Error);
                }
            }
            for (const auto& i : p.timeTicks)
            {
                if (i->glyphsFuture.valid() &&
                    i->glyphsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    try
                    {
                        i->glyphs = i->glyphsFuture.get();
                        p.sizePrev = glm::vec2(0.F, 0.F);
                        _resize();
                    }
                    catch (const std::exception & e)
                    {
                        _log(e.what(), System::LogLevel::Error);
                    }
                }
            }
        }

        bool TimelineSlider::_isPIPEnabled() const
        {
            DJV_PRIVATE_PTR();
            return
                p.media &&
                p.sequence.getFrameCount() > 1 &&
                p.pipEnabled &&
                isEnabled(true);
        }

        Math::Frame::Index TimelineSlider::_posToFrame(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const float v = value / g.w();
            const size_t sequenceFrameCount = p.sequence.getFrameCount();
            Math::Frame::Index out = sequenceFrameCount ?
                Math::clamp(
                    static_cast<Math::Frame::Index>(v * sequenceFrameCount),
                    static_cast<Math::Frame::Index>(0),
                    static_cast<Math::Frame::Index>(sequenceFrameCount - 1)) :
                0;
            return out;
        }

        float TimelineSlider::_frameToPos(Math::Frame::Index value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const size_t sequenceFrameCount = p.sequence.getFrameCount();
            const float v = sequenceFrameCount ? (value / static_cast<float>(sequenceFrameCount)) : 0.F;
            float out = g.min.x + v * g.w();
            return out;
        }

        float TimelineSlider::_getFrameLength() const
        {
            const Math::Frame::Index f = 1;
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getSecondLength() const
        {
            const Math::Frame::Index f = static_cast<Math::Frame::Index>(_p->speed.toFloat());
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getMinuteLength() const
        {
            const Math::Frame::Index f = static_cast<Math::Frame::Index>(_p->speed.toFloat() * 60.F);
            return _frameToPos(f) - _frameToPos(0);
        }

        float TimelineSlider::_getHourLength() const
        {
            const Math::Frame::Index f = static_cast<Math::Frame::Index>(_p->speed.toFloat() * 60.F * 60.F);
            return _frameToPos(f) - _frameToPos(0);
        }

        Math::BBox2f TimelineSlider::_getHandleGeometry() const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const Math::BBox2f g = getMargin().bbox(getGeometry(), style);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float x0 = _frameToPos(p.currentFrame);
            const float x1 = _frameToPos(p.currentFrame + 1);
            Math::BBox2f out = Math::BBox2f(
                floorf(x0),
                g.min.y,
                ceilf(std::max(x1 - x0, b)),
                g.h());
            return out;
        }

        void TimelineSlider::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                p.fontInfo = style->getFontInfo(Render2D::Font::familyMono, Render2D::Font::faceDefault, UI::MetricsRole::FontSmall);
                p.fontMetricsFuture = p.fontSystem->getMetrics(p.fontInfo);
                std::string maxFrameText;
                switch (p.timeUnits)
                {
                case AV::Time::Units::Timecode:
                    maxFrameText = "00:00:00:00";
                    break;
                case AV::Time::Units::Frames:
                {
                    const size_t rangesSize = p.sequence.getRanges().size();
                    if (rangesSize > 0)
                    {
                        maxFrameText = std::string(Math::getNumDigits(p.sequence.getRanges()[rangesSize - 1].getMax()), '0');
                    }
                    break;
                }
                default: break;
                }
                p.maxFrameSizeFuture = p.fontSystem->measure(maxFrameText, p.fontInfo);
                p.sizePrev = glm::vec2(0.F, 0.F);
                _resize();
            }
        }

        void TimelineSlider::_currentFrameUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::string text;
                if (p.sequence.getFrameCount() > 1)
                {
                    text = AV::Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
                }
                p.currentFrameText = text;
                p.currentFrameSizeFuture = p.fontSystem->measure(p.currentFrameText, p.fontInfo);
                p.currentFrameGlyphsFuture = p.fontSystem->getGlyphs(p.currentFrameText, p.fontInfo);
            }
        }

        void TimelineSlider::_showPIP(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value)
            {
                if (!p.pipOverlay->isVisible())
                {
                    if (auto window = getWindow())
                    {
                        window->addChild(p.pipOverlay);
                        p.pipOverlay->moveToFront();
                        p.pipOverlay->show();
                    }
                }
            }
            else
            {
                p.pipOverlay->hide();
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


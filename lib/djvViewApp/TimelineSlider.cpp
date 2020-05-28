// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/TimelineSlider.h>

#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/PlaybackSettings.h>
#include <djvViewApp/TimelinePIPWidget.h>

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
            AV::Font::Info fontInfo;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            std::string currentFrameText;
            float currentFrameLength = 0.F;
            std::future<glm::vec2> currentFrameSizeFuture;
            std::vector<std::shared_ptr<AV::Font::Glyph> > currentFrameGlyphs;
            std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > currentFrameGlyphsFuture;
            float maxFrameLength = 0.F;
            std::future<glm::vec2> maxFrameSizeFuture;
            uint32_t pressedID = Event::invalidID;
            bool pip = true;
            Time::Units timeUnits = Time::Units::First;
            std::shared_ptr<TimelinePIPWidget> pipWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::function<void(Frame::Index)> currentFrameCallback;
            std::function<void(bool)> currentFrameDragCallback;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Speed> > speedObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver;
            std::shared_ptr<ValueObserver<bool> > pipObserver;
            std::shared_ptr<ValueObserver<Time::Units> > timeUnitsObserver;
            glm::vec2 sizePrev = glm::vec2(0.F, 0.F);
            struct TimeTick
            {
                glm::vec2 pos;
                glm::vec2 size;
                std::string text;
                glm::vec2 textPos;
                std::vector<std::shared_ptr<AV::Font::Glyph> > glyphs;
                std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > glyphsFuture;
            };
            std::vector<std::shared_ptr<TimeTick> > timeTicks;
        };

        void TimelineSlider::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSlider");

            p.fontSystem = context->getSystemT<AV::Font::System>();

            p.pipWidget = TimelinePIPWidget::create(context);
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
            p.timeUnitsObserver = ValueObserver<Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](Time::Units value)
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

                p.pipWidget->setFileInfo(Core::FileSystem::FileInfo());

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

        void TimelineSlider::setImageOptions(const AV::Render2D::ImageOptions& value)
        {
            _p->pipWidget->setImageOptions(value);
        }

        void TimelineSlider::setImageRotate(UI::ImageRotate value)
        {
            _p->pipWidget->setImageRotate(value);
        }

        void TimelineSlider::setImageAspectRatio(UI::ImageAspectRatio value)
        {
            _p->pipWidget->setImageAspectRatio(value);
        }

        void TimelineSlider::setCurrentFrameCallback(const std::function<void(Frame::Index)>& value)
        {
            _p->currentFrameCallback = value;
        }

        void TimelineSlider::setCurrentFrameDragCallback(const std::function<void(bool)>& value)
        {
            _p->currentFrameDragCallback = value;
        }

        void TimelineSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            glm::vec2 size = glm::vec2(0.F, 0.F);
            size.y = p.fontMetrics.lineHeight * 2.F + b * 6.F;
            _setMinimumSize(size + getMargin().getSize(style));
        }

        void TimelineSlider::_layoutEvent(Event::Layout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
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
                    const std::vector<std::pair<float, std::function<Frame::Index(size_t, float)> > > units =
                    {
                        { _getHourLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * 60.F * 60.F * speed); } },
                        { _getMinuteLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * 60.F * speed); } },
                        { _getSecondLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value * speed); } },
                        { _getFrameLength(), [](size_t value, float speed) { return static_cast<Frame::Index>(value); } }
                    };
                    size_t timeTicksCount = 0;
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
                                    tick->text = Time::toString(p.sequence.getFrame(i.second(unit, speedF)), p.speed, p.timeUnits);
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

        void TimelineSlider::_paintEvent(Event::Paint& event)
        {
            Widget::_paintEvent(event);
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                const auto& style = _getStyle();
                const BBox2f g = getMargin().bbox(getGeometry(), style);
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const float b = style->getMetric(UI::MetricsRole::Border);
                const BBox2f& hg = _getHandleGeometry();

                // Draw the time ticks.
                auto color = style->getColor(UI::ColorRole::Foreground);
                color.setF32(color.getF32(3) * .4F, 3);
                const auto& render = _getRender();
                render->setFillColor(color);
                std::vector<BBox2f> boxes;
                for (const auto& tick : p.timeTicks)
                {
                    boxes.push_back(BBox2f(
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
                render->drawRects(boxes);

                // Draw the in/out points.
                if (p.inOutPointsEnabled)
                {
                    auto color = style->getColor(UI::ColorRole::Checked);
                    render->setFillColor(color);
                    const float x0 = _frameToPos(p.inPoint);
                    const float x1 = _frameToPos(p.outPoint + 1);
                    render->drawRect(BBox2f(
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
                    boxes.clear();
                    for (const auto& i : p.cacheSequence.ranges)
                    {
                        const float x0 = _frameToPos(i.min);
                        const float x1 = _frameToPos(i.max + 1);
                        boxes.push_back(BBox2f(
                            x0,
                            g.max.y - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                    render->drawRects(boxes);
                    color = style->getColor(UI::ColorRole::Cached);
                    render->setFillColor(color);
                    boxes.clear();
                    for (const auto& i : p.cachedFrames.ranges)
                    {
                        const float x0 = _frameToPos(i.min);
                        const float x1 = _frameToPos(i.max + 1);
                        boxes.push_back(BBox2f(
                            x0,
                            g.max.y - b * 2.F,
                            x1 - x0,
                            b * 2.F));
                    }
                    render->drawRects(boxes);
                }

                // Draw the frame ticks.
                const size_t sequenceSize = p.sequence.getSize();
                if (_getFrameLength() > b * 2.F)
                {
                    auto color = style->getColor(UI::ColorRole::Foreground);
                    color.setF32(color.getF32(3) * .4F, 3);
                    render->setFillColor(color);
                    boxes.clear();
                    for (Frame::Index f2 = 0; f2 < sequenceSize; ++f2)
                    {
                        const float x = _frameToPos(f2);
                        boxes.push_back(BBox2f(
                            x,
                            g.max.y - b * 6.F,
                            b,
                            b * 6.F));
                    }
                    render->drawRects(boxes);
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
                        p.pipWidget->setFileInfo(p.media->getFileInfo());
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
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            const Frame::Index frame = _posToFrame(static_cast<int>(pos.x - g.min.x));
            if (p.pipWidget)
            {
                if (auto parent = getParentRecursiveT<MediaWidget>())
                {
                    const auto& style = _getStyle();
                    const float s = style->getMetric(UI::MetricsRole::Spacing);
                    p.pipWidget->setPos(glm::vec2(pos.x, g.min.y - s), frame, parent->getGeometry().margin(-s));
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
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            event.accept();
            p.pressedID = id;
            p.currentFrame = _posToFrame(static_cast<int>(pos.x - g.min.x));
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
            p.pressedID = Event::invalidID;
            _redraw();
            _doCurrentFrameDragCallback(false);
        }

        void TimelineSlider::_initEvent(Event::Init & event)
        {
            Widget::_initEvent(event);
            _textUpdate();
            _currentFrameUpdate();
        }

        void TimelineSlider::_updateEvent(Event::Update & event)
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
                    _log(e.what(), LogLevel::Error);
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
                    _log(e.what(), LogLevel::Error);
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
                    _log(e.what(), LogLevel::Error);
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
                    _log(e.what(), LogLevel::Error);
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
                        _log(e.what(), LogLevel::Error);
                    }
                }
            }
        }

        Frame::Index TimelineSlider::_posToFrame(float value) const
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            const float v = value / g.w();
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
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            const size_t sequenceSize = p.sequence.getSize();
            const float v = sequenceSize ? (value / static_cast<float>(sequenceSize)) : 0.F;
            float out = g.min.x + v * g.w();
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
            const auto& style = _getStyle();
            const BBox2f g = getMargin().bbox(getGeometry(), style);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float x0 = _frameToPos(p.currentFrame);
            const float x1 = _frameToPos(p.currentFrame + 1);
            BBox2f out = BBox2f(
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
                p.fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
                p.fontMetricsFuture = p.fontSystem->getMetrics(p.fontInfo);
                std::string maxFrameText;
                switch (p.timeUnits)
                {
                case Time::Units::Timecode:
                    maxFrameText = "00:00:00:00";
                    break;
                case Time::Units::Frames:
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
                p.currentFrameText = Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
                p.currentFrameSizeFuture = p.fontSystem->measure(p.currentFrameText, p.fontInfo);
                p.currentFrameGlyphsFuture = p.fontSystem->getGlyphs(p.currentFrameText, p.fontInfo);
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


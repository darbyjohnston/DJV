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
#include <djvViewApp/PlaybackSettings.h>

#include <djvUI/Border.h>
#include <djvUI/EventSystem.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Window.h>

#include <djvAV/AVSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/Math.h>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/rational.h>

} // extern "C"

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
                void _init(Context*);
                PIPWidget()
                {}

            public:
                static std::shared_ptr<PIPWidget> create(Context*);

                void setPIPFileName(const std::string&);
                void setPIPPos(const glm::vec2&, Time::Timestamp);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                void _textUpdate();

                std::string _fileName;
                glm::vec2 _pipPos = glm::vec2(0.f, 0.f);
                std::shared_ptr<Media> _media;
                Time::Speed _speed;
                Time::Timestamp _currentTime = 0;
                std::shared_ptr<UI::ImageWidget> _imageWidget;
                std::shared_ptr<UI::Label> _timeLabel;
                std::shared_ptr<UI::Border> _border;
                std::shared_ptr<ValueObserver<Time::Speed> > _speedObserver;
                std::shared_ptr<ValueObserver<Time::Timestamp> > _currentTimeObserver;
                std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > _imageObserver;
            };

            void PIPWidget::_init(Context* context)
            {
                Widget::_init(context);

                setClassName("djv::ViewApp::TimelineSlider::PIPWidget");

                _imageWidget = UI::ImageWidget::create(context);
                _imageWidget->setSizeRole(UI::MetricsRole::TextColumn);

                _timeLabel = UI::Label::create(context);
                _timeLabel->setVAlign(UI::VAlign::Bottom);
                _timeLabel->setMargin(UI::MetricsRole::MarginSmall);
                _timeLabel->setBackgroundRole(UI::ColorRole::Overlay);

                auto layout = UI::StackLayout::create(context);
                layout->addChild(_imageWidget);
                layout->addChild(_timeLabel);

                _border = UI::Border::create(context);
                _border->addChild(layout);
                addChild(_border);
            }

            std::shared_ptr<PIPWidget> PIPWidget::create(Context* context)
            {
                auto out = std::shared_ptr<PIPWidget>(new PIPWidget);
                out->_init(context);
                return out;
            }

            void PIPWidget::setPIPFileName(const std::string& value)
            {
                if (value == _fileName)
                    return;
                _fileName = value;
                if (!_fileName.empty())
                {
                    _media = Media::create(_fileName, 1, 0, getContext());

                    auto weak = std::weak_ptr<PIPWidget>(std::dynamic_pointer_cast<PIPWidget>(shared_from_this()));
                    _speedObserver = ValueObserver<Time::Speed>::create(
                        _media->observeSpeed(),
                        [weak](const Time::Speed& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_speed = value;
                            widget->_textUpdate();
                        }
                    });

                    _currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                        _media->observeCurrentTime(),
                        [weak](Time::Timestamp value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_currentTime = value;
                            widget->_textUpdate();
                        }
                    });

                    _imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                        _media->observeCurrentImage(),
                        [weak](const std::shared_ptr<AV::Image::Image> & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_imageWidget->setImage(value);
                        }
                    });
                }
                else
                {
                    _media.reset();
                    _imageObserver.reset();
                }
            }

            void PIPWidget::setPIPPos(const glm::vec2& value, Time::Timestamp timestamp)
            {
                if (value == _pipPos)
                    return;
                if (_media)
                {
                    _media->setCurrentTime(timestamp);
                }
                _pipPos = value;
                _resize();
            }

            void PIPWidget::_layoutEvent(Event::Layout&)
            {
                const BBox2f& g = getGeometry();
                const glm::vec2 size = _border->getMinimumSize();
                _border->setGeometry(BBox2f(_pipPos.x - floorf(size.x / 2.f), _pipPos.y - size.y, size.x, size.y));
            }

            void PIPWidget::_paintEvent(Event::Paint& event)
            {
                UI::Widget::_paintEvent(event);
                auto style = _getStyle();
                const float sh = style->getMetric(UI::MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(_getColorWithOpacity(style->getColor(UI::ColorRole::Shadow)));
                for (const auto& i : getChildrenT<UI::Widget>())
                {
                    if (i->isVisible())
                    {
                        BBox2f g = i->getGeometry();
                        g.min.x += sh;
                        g.min.y += sh;
                        g.max.x += sh;
                        g.max.y += sh;
                        render->drawShadow(g, sh);
                    }
                }
            }

            void PIPWidget::_textUpdate()
            {
                auto avSystem = getContext()->getSystemT<AV::AVSystem>();
                _timeLabel->setText(avSystem->getLabel(_currentTime, _speed));
            }

        } // namespace

        struct TimelineSlider::Private
        {
            std::shared_ptr<Media> media;
            Time::Timestamp duration = 0;
            std::shared_ptr<ValueSubject<Time::Timestamp> > currentTime;
            Time::Speed speed;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            std::map<uint32_t, bool> hover;
            uint32_t pressedID = Event::InvalidID;
            bool pip = true;
            std::shared_ptr<PIPWidget> pipWidget;
            std::shared_ptr<UI::Layout::Overlay> overlay;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > durationObserver;
            std::shared_ptr<ValueObserver<Time::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<bool> > pipObserver;
        };

        void TimelineSlider::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::TimelineSlider");
            setPointerEnabled(true);

            p.currentTime = ValueSubject<Time::Timestamp>::create();

            p.pipWidget = PIPWidget::create(context);
            p.overlay = UI::Layout::Overlay::create(context);
            p.overlay->setCaptureKeyboard(false);
            p.overlay->setCapturePointer(false);
            p.overlay->setBackgroundRole(UI::ColorRole::None);
            p.overlay->addChild(p.pipWidget);
            addChild(p.overlay);

            auto weak = std::weak_ptr<TimelineSlider>(std::dynamic_pointer_cast<TimelineSlider>(shared_from_this()));
            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
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
            }
        }

        TimelineSlider::TimelineSlider() :
            _p(new Private)
        {}

        TimelineSlider::~TimelineSlider()
        {}

        std::shared_ptr<TimelineSlider> TimelineSlider::create(Context * context)
        {
            auto out = std::shared_ptr<TimelineSlider>(new TimelineSlider);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<Time::Timestamp> > TimelineSlider::observeCurrentTime() const
        {
            return _p->currentTime;
        }

        void TimelineSlider::setMedia(const std::shared_ptr<Media> & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.media)
                return;
            p.media = value;
            if (p.media)
            {
                p.pipWidget->setPIPFileName(p.media->getFileName());
                
                auto weak = std::weak_ptr<TimelineSlider>(std::dynamic_pointer_cast<TimelineSlider>(shared_from_this()));
                p.infoObserver = ValueObserver<AV::IO::Info>::create(
                    p.media->observeInfo(),
                    [weak](const AV::IO::Info & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value.video.size() ? value.video[0].speed : Time::Speed();
                        widget->_redraw();
                    }
                });
                p.durationObserver = ValueObserver<Time::Timestamp>::create(
                    p.media->observeDuration(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->duration = value;
                        widget->_redraw();
                    }
                });
                p.currentTimeObserver = ValueObserver<Time::Timestamp>::create(
                    p.media->observeCurrentTime(),
                    [weak](Time::Timestamp value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentTime->setIfChanged(value);
                        widget->_redraw();
                    }
                });
            }
            else
            {
                p.duration = 0;
                p.currentTime->setIfChanged(0);
                p.speed = Time::Speed();

                p.pipWidget->setPIPFileName(std::string());

                p.infoObserver.reset();
                p.durationObserver.reset();
                p.currentTimeObserver.reset();
            }
            _textUpdate();
        }

        void TimelineSlider::_styleEvent(Event::Style &)
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, UI::MetricsRole::FontMedium);
            auto fontSystem = _getFontSystem();
            p.fontMetricsFuture = fontSystem->getMetrics(fontInfo);
            _resize();
        }

        void TimelineSlider::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const float h = style->getMetric(UI::MetricsRole::Handle);
            glm::vec2 size = glm::vec2(0.f, 0.f);
            size.x = style->getMetric(UI::MetricsRole::TextColumn);
            size.y = h + p.fontMetrics.lineHeight + m * 2.f;
            _setMinimumSize(size);
        }

        void TimelineSlider::_paintEvent(Event::Paint & event)
        {
            DJV_PRIVATE_PTR();
            const BBox2f & g = getGeometry();
            auto style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const float b = style->getMetric(UI::MetricsRole::Border);
            const float h = style->getMetric(UI::MetricsRole::Handle);
            const BBox2f & hg = _getHandleGeometry();

            auto render = _getRender();
            {
                AVRational r;
                r.num = p.speed.getDuration();
                r.den = p.speed.getScale();
                const Time::Timestamp t = av_rescale_q(1, r, av_get_time_base_q());
                if (_timeToPos(t) - _timeToPos(0) > b * 2.f)
                {
                    auto color = _getColorWithOpacity(style->getColor(UI::ColorRole::Foreground));
                    color.setF32(color.getF32(3) * .2f, 3);
                    render->setFillColor(color);
                    for (Time::Timestamp t2 = 0; t2 < p.duration; t2 += t)
                    {
                        const float x = _timeToPos(t2);
                        const float h = ceilf(hg.h() * .5f);
                        render->drawRect(BBox2f(floorf(x), g.max.y - m - h, b, h));
                    }
                }
            }
            {
                AVRational r;
                r.num = p.speed.getDuration();
                r.den = p.speed.getScale();
                const Time::Timestamp t = av_rescale_q(Time::Speed::speedToFloat(p.speed), r, av_get_time_base_q());
                if (_timeToPos(t) - _timeToPos(0) > b * 2.f)
                {
                    auto color = _getColorWithOpacity(style->getColor(UI::ColorRole::Foreground));
                    color.setF32(color.getF32(3) * .2f, 3);
                    render->setFillColor(color);
                    for (Time::Timestamp t2 = 0; t2 < p.duration; t2 += t)
                    {
                        const float x = _timeToPos(t2);
                        const float h = ceilf(hg.h() * .5f);
                        render->drawRect(BBox2f(floorf(x), g.max.y - m - h, b, h));
                    }
                }
            }
            {
                AVRational r;
                r.num = p.speed.getDuration();
                r.den = p.speed.getScale();
                const Time::Timestamp t = av_rescale_q(60.f * Time::Speed::speedToFloat(p.speed), r, av_get_time_base_q());
                if (_timeToPos(t) - _timeToPos(0) > b * 2.f)
                {
                    auto color = _getColorWithOpacity(style->getColor(UI::ColorRole::Foreground));
                    color.setF32(color.getF32(3) * .2f, 3);
                    render->setFillColor(color);
                    for (Time::Timestamp t2 = 0; t2 < p.duration; t2 += t)
                    {
                        const float x = _timeToPos(t2);
                        const float h = ceilf(hg.h() * .5f);
                        render->drawRect(BBox2f(floorf(x), g.max.y - m - h, b, h));
                    }
                }
            }

            render->setFillColor(_getColorWithOpacity(style->getColor(UI::ColorRole::Foreground)));
            render->drawRect(hg);

            const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, UI::MetricsRole::FontMedium);
            render->setCurrentFont(fontInfo);
        }

        void TimelineSlider::_pointerEnterEvent(Event::PointerEnter & event)
        {
            DJV_PRIVATE_PTR();
            if (!event.isRejected())
            {
                event.accept();
                p.hover[event.getPointerInfo().id] = true;
                _redraw();
                if (p.pip && isEnabled())
                {
                    auto eventSystem = getContext()->getSystemT<UI::EventSystem>();
                    if (auto window = eventSystem->getCurrentWindow().lock())
                    {
                        window->addChild(p.overlay);
                        p.overlay->setVisible(true);
                    }
                }
            }
        }

        void TimelineSlider::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            DJV_PRIVATE_PTR();
            event.accept();
            auto i = p.hover.find(event.getPointerInfo().id);
            if (i != p.hover.end())
            {
                p.hover.erase(i);
                _redraw();
            }
            p.overlay->setVisible(false);
        }

        void TimelineSlider::_pointerMoveEvent(Event::PointerMove & event)
        {
            DJV_PRIVATE_PTR();
            const auto id = event.getPointerInfo().id;
            const auto & pos = event.getPointerInfo().projectedPos;
            const BBox2f & g = getGeometry();
            p.hover[id] = g.contains(pos);
            const Time::Timestamp timestamp = _posToTime(static_cast<int>(pos.x - g.min.x));
            if (p.hover[id] || p.pressedID)
            {
                event.accept();
                auto style = _getStyle();
                const float s = style->getMetric(UI::MetricsRole::Spacing);
                p.pipWidget->setPIPPos(glm::vec2(pos.x, g.min.y - s), timestamp);
            }
            if (p.pressedID)
            {
                if (p.currentTime->setIfChanged(timestamp))
                {
                    _textUpdate();
                    _redraw();
                }
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
            if (p.hover[id])
            {
                event.accept();
                p.pressedID = id;
                if (p.currentTime->setIfChanged(_posToTime(static_cast<int>(pos.x - g.min.x))))
                {
                    _textUpdate();
                    _redraw();
                }
            }
        }

        void TimelineSlider::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            DJV_PRIVATE_PTR();
            if (event.getPointerInfo().id != p.pressedID)
                return;
            event.accept();
            p.pressedID = Event::InvalidID;
            _redraw();
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

        Time::Timestamp TimelineSlider::_posToTime(float value) const
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const BBox2f& g = getGeometry();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const double v = (value - m) / static_cast<double>(g.w() - m * 2.f);
            AVRational r;
            r.num = p.speed.getDuration();
            r.den = p.speed.getScale();
            const Time::Timestamp t = av_rescale_q(1, r, av_get_time_base_q());
            Time::Timestamp out = p.duration ?
                Math::clamp(static_cast<Time::Timestamp>(v * (p.duration - t)), static_cast<Time::Timestamp>(0), p.duration - t) :
                0;
            return out;
        }

        float TimelineSlider::_timeToPos(Time::Timestamp value) const
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const BBox2f& g = getGeometry();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            AVRational r;
            r.num = p.speed.getDuration();
            r.den = p.speed.getScale();
            const Time::Timestamp t = av_rescale_q(1, r, av_get_time_base_q());
            const double v = value / static_cast<double>(p.duration - t);
            float out = floorf(g.min.x + m + v * (g.w() - m * 2.f));
            return out;
        }

        BBox2f TimelineSlider::_getHandleGeometry() const
        {
            DJV_PRIVATE_PTR();
            const BBox2f & g = getGeometry();
            auto style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            AVRational r;
            r.num = p.speed.getDuration();
            r.den = p.speed.getScale();
            const int64_t t = av_rescale_q(1, r, av_get_time_base_q());
            const float x = p.duration ? floorf(p.currentTime->get() / static_cast<float>(p.duration - t) * (g.w() - 1.f - m * 2.f)) : 0.f;
            BBox2f out = BBox2f(g.min.x + m + x, g.min.y + m, 1.f, g.h() - m * 2.f);
            return out;
        }

        void TimelineSlider::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            auto avSystem = getContext()->getSystemT<AV::AVSystem>();
            auto style = _getStyle();
            auto fontSystem = _getFontSystem();
            const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, UI::MetricsRole::FontMedium);
            _resize();
        }

    } // namespace ViewApp
} // namespace djv


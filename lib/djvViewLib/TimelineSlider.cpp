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

#include <djvViewLib/TimelineSlider.h>

#include <djvAV/AVSystem.h>
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
    namespace ViewLib
    {
        struct TimelineSlider::Private
        {
            Time::Timestamp duration = 0;
            std::shared_ptr<ValueSubject<Time::Timestamp> > currentTime;
            Time::Speed speed;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            std::map<uint32_t, bool> hover;
            uint32_t pressedID = Event::InvalidID;
        };

        void TimelineSlider::_init(Context * context)
        {
            Widget::_init(context);

            setClassName("djv::ViewLib::TimelineSlider");
            setPointerEnabled(true);

            DJV_PRIVATE_PTR();
            p.currentTime = ValueSubject<Time::Timestamp>::create();
        }

        TimelineSlider::TimelineSlider() :
            _p(new Private)
        {}

        TimelineSlider::~TimelineSlider()
        {}

        std::shared_ptr<TimelineSlider> TimelineSlider::create(Core::Context * context)
        {
            auto out = std::shared_ptr<TimelineSlider>(new TimelineSlider);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<Time::Timestamp> > TimelineSlider::observeCurrentTime() const
        {
            return _p->currentTime;
        }

        void TimelineSlider::setDuration(Time::Timestamp value)
        {
            if (value == _p->duration)
                return;
            _p->duration = value;
            _textUpdate();
        }

        void TimelineSlider::setCurrentTime(Time::Timestamp value)
        {
            if (_p->currentTime->setIfChanged(value))
            {
                _textUpdate();
                _redraw();
            }
        }

        void TimelineSlider::setSpeed(const Core::Time::Speed & value)
        {
            if (value == _p->speed)
                return;
            _p->speed = value;
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
            const float h = style->getMetric(UI::MetricsRole::Handle);
            const BBox2f & hg = _getHandleGeometry();

            auto render = _getRender();
            render->setFillColor(_getColorWithOpacity(style->getColor(UI::ColorRole::Foreground)));
            render->drawRect(hg);

            const auto fontInfo = style->getFontInfo(AV::Font::faceDefault, UI::MetricsRole::FontMedium);
            render->setCurrentFont(fontInfo);
        }

        void TimelineSlider::_pointerEnterEvent(Event::PointerEnter & event)
        {
            if (!event.isRejected())
            {
                event.accept();
                _p->hover[event.getPointerInfo().id] = true;
                _redraw();
            }
        }

        void TimelineSlider::_pointerLeaveEvent(Event::PointerLeave & event)
        {
            event.accept();
            auto i = _p->hover.find(event.getPointerInfo().id);
            if (i != _p->hover.end())
            {
                _p->hover.erase(i);
                _redraw();
            }
        }

        void TimelineSlider::_pointerMoveEvent(Event::PointerMove & event)
        {
            const auto id = event.getPointerInfo().id;
            const auto & pos = event.getPointerInfo().projectedPos;
            const BBox2f & g = getGeometry();
            _p->hover[id] = g.contains(pos);
            if (_p->hover[id] || _p->pressedID)
            {
                event.accept();
            }
            if (_p->pressedID)
            {
                if (_p->currentTime->setIfChanged(_posToTime(static_cast<int>(pos.x - g.min.x))))
                {
                    _textUpdate();
                    _redraw();
                }
            }
        }

        void TimelineSlider::_buttonPressEvent(Event::ButtonPress & event)
        {
            if (_p->pressedID)
                return;
            const auto id = event.getPointerInfo().id;
            const auto & pos = event.getPointerInfo().projectedPos;
            const BBox2f & g = getGeometry();
            if (_p->hover[id])
            {
                event.accept();
                _p->pressedID = id;
                if (_p->currentTime->setIfChanged(_posToTime(static_cast<int>(pos.x - g.min.x))))
                {
                    _textUpdate();
                    _redraw();
                }
            }
        }

        void TimelineSlider::_buttonReleaseEvent(Event::ButtonRelease & event)
        {
            if (event.getPointerInfo().id != _p->pressedID)
                return;
            event.accept();
            _p->pressedID = Event::InvalidID;
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

        int64_t TimelineSlider::_posToTime(int value) const
        {
            DJV_PRIVATE_PTR();
            auto style = _getStyle();
            const float m = style->getMetric(UI::MetricsRole::MarginSmall);
            const double v = value / static_cast<double>(getGeometry().w() - m * 2.f);
            AVRational r;
            r.num = p.speed.getDuration();
            r.den = p.speed.getScale();
            const int64_t f = av_rescale_q(1, r, av_get_time_base_q());
            int64_t out = _p->duration ?
                Core::Math::clamp(static_cast<int64_t>(v * (_p->duration - f)), static_cast<int64_t>(0), _p->duration - f) :
                0;
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
            const int64_t f = av_rescale_q(1, r, av_get_time_base_q());
            const float x = floorf(_p->currentTime->get() / static_cast<float>(_p->duration - f) * (g.w() - 1.f - m * 2.f));
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

    } // namespace ViewLib
} // namespace djv


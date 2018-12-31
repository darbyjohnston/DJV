//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/Render2DSystem.h>

#include <djvCore/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct TimelineSlider::Private
        {
            Time::Duration duration = 0;
            std::shared_ptr<ValueSubject<Time::Timestamp> > currentTime;
            std::map<uint32_t, bool> hover;
            uint32_t pressedId = 0;
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

        std::shared_ptr<Core::IValueSubject<Time::Timestamp> > TimelineSlider::getCurrentTime() const
        {
            return _p->currentTime;
        }

        void TimelineSlider::setDuration(Time::Duration value)
        {
            _p->duration = value;
        }

        void TimelineSlider::setCurrentTime(Time::Timestamp value)
        {
            if (_p->currentTime->setIfChanged(value))
            {
                _redraw();
            }
        }

        void TimelineSlider::_preLayoutEvent(Event::PreLayout& event)
        {}

        void TimelineSlider::_layoutEvent(Event::Layout& event)
        {}

        void TimelineSlider::_paintEvent(Event::Paint& event)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& hg = _getHandleGeometry();
                    render->setFillColor(_getColorWithOpacity(style->getColor(UI::Style::ColorRole::Foreground)));
                    render->drawRectangle(hg);
                }
            }
        }

        void TimelineSlider::_pointerEnterEvent(Event::PointerEnter& event)
        {
            if (!event.isRejected())
            {
                event.accept();
                _p->hover[event.getPointerInfo().id] = true;
                _redraw();
            }
        }

        void TimelineSlider::_pointerLeaveEvent(Event::PointerLeave& event)
        {
            event.accept();
            auto i = _p->hover.find(event.getPointerInfo().id);
            if (i != _p->hover.end())
            {
                _p->hover.erase(i);
                _redraw();
            }
        }

        void TimelineSlider::_pointerMoveEvent(Event::PointerMove& event)
        {
            const auto id = event.getPointerInfo().id;
            const auto& pos = event.getPointerInfo().projectedPos;
            const BBox2f& g = getGeometry();
            _p->hover[id] = g.contains(pos);
            if (_p->hover[id] || _p->pressedId)
            {
                event.accept();
            }
            if (_p->pressedId)
            {
                if (_p->currentTime->setIfChanged(_posToTime(static_cast<int>(pos.x - g.min.x))))
                {
                    _redraw();
                }
            }
        }

        void TimelineSlider::_buttonPressEvent(Event::ButtonPress& event)
        {
            if (_p->pressedId)
                return;
            const auto id = event.getPointerInfo().id;
            const auto& pos = event.getPointerInfo().projectedPos;
            const BBox2f& g = getGeometry();
            if (_p->hover[id])
            {
                event.accept();
                _p->pressedId = id;
                if (_p->currentTime->setIfChanged(_posToTime(static_cast<int>(pos.x - g.min.x))))
                {
                    _redraw();
                }
            }
        }

        void TimelineSlider::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            if (event.getPointerInfo().id != _p->pressedId)
                return;
            event.accept();
            _p->pressedId = 0;
            _redraw();
        }

        int64_t TimelineSlider::_posToTime(int value) const
        {
            const double v = value / static_cast<double>(getGeometry().w());
            return _p->duration ?
                Core::Math::clamp(static_cast<int64_t>(v * (_p->duration - 1)), static_cast<int64_t>(0), _p->duration - 1) :
                0;
        }

        BBox2f TimelineSlider::_getHandleGeometry() const
        {
            BBox2f out;
            const BBox2f& g = getGeometry();
            const float w = g.w();
            const float h = g.h();
            const float x = _p->currentTime->get() / static_cast<float>(_p->duration) * (w - 1);
            return BBox2f(g.min.x + x, g.min.y, 1.f, g.h());
        }

    } // namespace ViewLib
} // namespace djv


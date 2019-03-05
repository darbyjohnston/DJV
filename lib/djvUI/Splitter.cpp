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

#include <djvUI/Splitter.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Splitter::Private
            {
                Orientation orientation = Orientation::Horizontal;
                float split = .5f;
                float splitterWidth = 0.f;
                ColorRole handleColorRole = ColorRole::Button;
                size_t visibleChildCount = 0;
                std::map<Event::PointerID, bool> hover;
                Event::PointerID pressedID = Event::InvalidID;
                std::function<void(float)> splitCallback;
            };

            void Splitter::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Splitter");
                setPointerEnabled(true);
            }

            Splitter::Splitter() :
                _p(new Private)
            {}

            Splitter::~Splitter()
            {}

            std::shared_ptr<Splitter> Splitter::create(Orientation orientation, Context * context)
            {
                auto out = std::shared_ptr<Splitter>(new Splitter);
                out->_init(context);
                out->setOrientation(orientation);
                out->setPointerEnabled(true);
                return out;
            }

            Orientation Splitter::getOrientation() const
            {
                return _p->orientation;
            }

            void Splitter::setOrientation(Orientation value)
            {
                _p->orientation = value;
            }

            float Splitter::getSplit() const
            {
                return _p->split;
            }

            void Splitter::setSplit(float value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.split)
                    return;
                p.split = value;
                if (p.splitCallback)
                {
                    p.splitCallback(p.split);
                }
                _resize();
            }

            void Splitter::setSplitCallback(const std::function<void(float)>& callback)
            {
                _p->splitCallback = callback;
            }

            ColorRole Splitter::getHandleColorRole() const
            {
                return _p->handleColorRole;
            }

            void Splitter::setHandleColorRole(ColorRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.handleColorRole)
                    return;
                p.handleColorRole = value;
                _redraw();
            }

            float Splitter::getHeightForWidth(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.f;
                if (auto style = _getStyle().lock())
                {
                    // Get the child sizes.
                    const glm::vec2 m = getMargin().getSize(style);
                    for (const auto& child : getChildrenT<Widget>())
                    {
                        if (child->isVisible())
                        {
                            const float heightForWidth = child->getHeightForWidth(value - m.x);
                            switch (p.orientation)
                            {
                            case Orientation::Horizontal:
                                out = std::max(out, heightForWidth);
                                break;
                            case Orientation::Vertical:
                                out += heightForWidth;
                                break;
                            default: break;
                            }
                        }
                    }

                    // Add the splitter size.
                    switch (p.orientation)
                    {
                    case Orientation::Vertical:
                        out += p.splitterWidth;
                        break;
                    default: break;
                    }

                    out += m.y;
                }
                return out;
            }

            void Splitter::_preLayoutEvent(Event::PreLayout& event)
            {
                DJV_PRIVATE_PTR();
                if (auto style = _getStyle().lock())
                {
                    // Get the child sizes.
                    glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                    p.visibleChildCount = 0;
                    for (const auto & child : getChildrenT<Widget>())
                    {
                        if (child->isVisible())
                        {
                            ++p.visibleChildCount;
                            const glm::vec2& size = child->getMinimumSize();
                            switch (p.orientation)
                            {
                            case Orientation::Horizontal:
                                minimumSize.x += size.x;
                                minimumSize.y = std::max(size.y, minimumSize.y);
                                break;
                            case Orientation::Vertical:
                                minimumSize.x = std::max(size.x, minimumSize.x);
                                minimumSize.y += size.y;
                                break;
                            default: break;
                            }
                        }
                    }

                    // Add the splitter size.
                    if (p.visibleChildCount)
                    {
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            minimumSize.x += p.splitterWidth;
                            break;
                        case Orientation::Vertical:
                            minimumSize.y += p.splitterWidth;
                            break;
                        default: break;
                        }
                    }

                    _setMinimumSize(minimumSize + getMargin().getSize(style));
                }
            }

            void Splitter::_layoutEvent(Event::Layout& event)
            {
                DJV_PRIVATE_PTR();
                if (auto style = _getStyle().lock())
                {
                    const BBox2f & g = getMargin().bbox(getGeometry(), style);

                    const auto children = getChildrenT<Widget>();
                    if (2 == p.visibleChildCount)
                    {
                        BBox2f bbox;
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            bbox.min.x = g.min.x;
                            bbox.min.y = g.min.y;
                            bbox.max.x = _valueToPos(p.split) - ceilf(p.splitterWidth / 2.f);
                            bbox.max.y = g.max.y;
                            children[0]->setGeometry(bbox);
                            bbox.min.x = bbox.max.x + p.splitterWidth;
                            bbox.min.y = g.min.y;
                            bbox.max.x = g.max.x;
                            bbox.max.y = g.max.y;
                            children[1]->setGeometry(bbox);
                            break;
                        case Orientation::Vertical:
                            bbox.min.x = g.min.x;
                            bbox.min.y = g.min.y;
                            bbox.max.x = g.max.x;
                            bbox.max.y = _valueToPos(p.split) - ceilf(p.splitterWidth / 2.f);
                            children[0]->setGeometry(bbox);
                            bbox.min.x = g.min.x;
                            bbox.min.y = bbox.max.y + p.splitterWidth;
                            bbox.max.x = g.max.x;
                            bbox.max.y = g.max.y;
                            children[1]->setGeometry(bbox);
                            break;
                        default: break;
                        }
                    }
                    else
                    {
                        for (const auto & child : children)
                        {
                            child->setGeometry(g);
                        }
                    }
                }
            }

            void Splitter::_paintEvent(Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                if (2 == p.visibleChildCount)
                {
                    if (auto render = _getRender().lock())
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const BBox2f& sg = _getSplitterGeometry();
                            const BBox2f& hg = _getHandleGeometry();

                            // Draw the background.
                            if (p.handleColorRole != ColorRole::None)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(p.handleColorRole)));
                                render->drawRect(hg);
                            }

                            // Draw the pressed or hovered state.
                            if (p.pressedID)
                            {
                                render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Pressed)));
                                render->drawRect(hg);
                            }
                            else
                            {
                                bool hover = p.pressedID;
                                for (const auto& h : p.hover)
                                {
                                    hover |= h.second;
                                }
                                if (hover)
                                {
                                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Hovered)));
                                    render->drawRect(hg);
                                }
                            }
                        }
                    }
                }
            }

            void Splitter::_pointerEnterEvent(Event::PointerEnter& event)
            {
                DJV_PRIVATE_PTR();
                if (2 == p.visibleChildCount && !event.isRejected())
                {
                    event.accept();
                    p.hover[event.getPointerInfo().id] = _getSplitterGeometry().contains(event.getPointerInfo().projectedPos);
                    _redraw();
                }
            }

            void Splitter::_pointerLeaveEvent(Event::PointerLeave& event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                auto i = p.hover.find(event.getPointerInfo().id);
                if (i != p.hover.end())
                {
                    p.hover.erase(i);
                    _redraw();
                }
            }

            void Splitter::_pointerMoveEvent(Event::PointerMove& event)
            {
                DJV_PRIVATE_PTR();
                const auto id = event.getPointerInfo().id;
                const auto& pos = event.getPointerInfo().projectedPos;
                p.hover[id] = _getSplitterGeometry().contains(pos);

                if (p.hover[id] || p.pressedID)
                {
                    event.accept();
                    _redraw();
                }

                if (p.pressedID)
                {
                    float newPos = 0.f;
                    switch (p.orientation)
                    {
                    case Orientation::Horizontal:
                        newPos = pos.x;
                        break;
                    case Orientation::Vertical:
                        newPos = pos.y;
                        break;
                    default: break;
                    }
                    setSplit(_posToValue(newPos));
                }
            }

            void Splitter::_buttonPressEvent(Event::ButtonPress& event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID)
                    return;
                const auto id = event.getPointerInfo().id;
                if (p.hover[id])
                {
                    event.accept();
                    p.pressedID = id;
                    _redraw();
                }
            }

            void Splitter::_buttonReleaseEvent(Event::ButtonRelease& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getPointerInfo().id != p.pressedID)
                    return;
                event.accept();
                p.pressedID = Event::InvalidID;
                _redraw();
            }

            void Splitter::_updateEvent(Event::Update&)
            {
                DJV_PRIVATE_PTR();
                if (auto style = _getStyle().lock())
                {
                    const float value = style->getMetric(MetricsRole::Handle);
                    if (value != p.splitterWidth)
                    {
                        _resize();
                    }
                    p.splitterWidth = value;
                }
            }

            float Splitter::_valueToPos(float value) const
            {
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                float out = 0.f;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out = g.x() + ceilf(p.splitterWidth / 2.f + (g.w() - p.splitterWidth) * value);
                    break;
                case Orientation::Vertical:
                    out = g.y() + ceilf(p.splitterWidth / 2.f + (g.h() - p.splitterWidth) * value);
                    break;
                default: break;
                }
                return out;
            }

            float Splitter::_posToValue(float value) const
            {
                DJV_PRIVATE_PTR();
                const BBox2f& g = getGeometry();
                float out = 0.f;
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out = Math::clamp((value - g.x() - p.splitterWidth / 2.f) / (g.w() - p.splitterWidth), 0.f, 1.f);
                    break;
                case Orientation::Vertical:
                    out = Math::clamp((value - g.y() - p.splitterWidth / 2.f) / (g.h() - p.splitterWidth), 0.f, 1.f);
                    break;
                default: break;
                }
                return out;
            }

            BBox2f Splitter::_getSplitterGeometry() const
            {
                DJV_PRIVATE_PTR();
                BBox2f out;
                const auto style = _getStyle();
                const BBox2f& g = getGeometry();
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out.min.x = _valueToPos(p.split) - ceilf(p.splitterWidth / 2.f);
                    out.min.y = g.min.y;
                    out.max.x = out.min.x + p.splitterWidth;
                    out.max.y = g.max.y;
                    break;
                case Orientation::Vertical:
                    out.min.x = g.min.x;
                    out.min.y = _valueToPos(p.split) - ceilf(p.splitterWidth / 2.f);
                    out.max.x = g.max.x;
                    out.max.y = out.min.y + p.splitterWidth;
                    break;
                default: break;
                }
                return out;
            }

            BBox2f Splitter::_getHandleGeometry() const
            {
                BBox2f out;
                const BBox2f& g = _getSplitterGeometry();
                switch (_p->orientation)
                {
                case Orientation::Horizontal:
                {
                    const float w = g.w();
                    out.min.x = g.min.x + ceilf(w / 4.f);
                    out.min.y = g.min.y;
                    out.max.x = g.max.x - ceilf(w / 4.f);
                    out.max.y = g.max.y;
                    break;
                }
                case Orientation::Vertical:
                {
                    const float h = g.h();
                    out.min.x = g.min.x;
                    out.min.y = g.min.y + ceilf(h / 4.f);
                    out.max.x = g.max.x;
                    out.max.y = g.max.y - ceilf(h / 4.f);
                    break;
                }
                default: break;
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

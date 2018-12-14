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

#include <djvUI/Splitter.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct Splitter::Private
        {
            Orientation orientation = Orientation::Horizontal;
            float split = .5f;
            float splitterWidth = 0.f;
            std::map<uint32_t, bool> hover;
            uint32_t pressedId = 0;
            std::function<void(float)> splitCallback;
        };

        void Splitter::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setClassName("djv::UI::Splitter");
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
            if (value == _p->split)
                return;
            
            _p->split = value;
            
            if (_p->splitCallback)
            {
                _p->splitCallback(_p->split);
            }
        }
        
        void Splitter::setSplitCallback(const std::function<void(float)>& callback)
        {
            _p->splitCallback = callback;
        }

        float Splitter::getHeightForWidth(float value) const
        {
            float out = 0.f;

            // Get the child sizes.
            for (const auto& child : getChildrenT<Widget>())
            {
                if (child->isVisible())
                {
                    const float heightForWidth = child->getHeightForWidth(value);
                    switch (_p->orientation)
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
            switch (_p->orientation)
            {
            case Orientation::Vertical:
                out += _p->splitterWidth;
                break;
            default: break;
            }

            return out;
        }

        void Splitter::updateEvent(UpdateEvent&)
        {
            if (auto style = _getStyle().lock())
            {
                const float m = style->getMetric(MetricsRole::Margin);
                _p->splitterWidth = m * 1.5f;
            }
        }

        void Splitter::preLayoutEvent(PreLayoutEvent& event)
        {
            // Get the child sizes.
            glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
            for (const auto& child : getChildrenT<Widget>())
            {
                if (child->isVisible())
                {
                    const glm::vec2& size = child->getMinimumSize();
                    switch (_p->orientation)
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
            switch (_p->orientation)
            {
            case Orientation::Horizontal:
                minimumSize.x += _p->splitterWidth;
                break;
            case Orientation::Vertical:
                minimumSize.y += _p->splitterWidth;
                break;
            default: break;
            }

            _setMinimumSize(minimumSize);
        }

        void Splitter::layoutEvent(LayoutEvent& event)
        {
            const auto style = _getStyle();
            const BBox2f& g = getGeometry();

            const auto children = getChildrenT<Widget>();
            const size_t childrenSize = children.size();
            if (2 == childrenSize)
            {
                BBox2f bbox;
                switch (_p->orientation)
                {
                case Orientation::Horizontal:
                    bbox.min.x = g.min.x;
                    bbox.min.y = g.min.y;
                    bbox.max.x = _valueToPos(_p->split) - _p->splitterWidth / 2.f;
                    bbox.max.y = g.max.y;
                    children[0]->setGeometry(bbox);
                    bbox.min.x = bbox.max.x + _p->splitterWidth;
                    bbox.min.y = g.min.y;
                    bbox.max.x = g.max.x;
                    bbox.max.y = g.max.y;
                    children[1]->setGeometry(bbox);
                    break;
                case Orientation::Vertical:
                    bbox.min.x = g.min.x;
                    bbox.min.y = g.min.y;
                    bbox.max.x = g.max.x;
                    bbox.max.y = _valueToPos(_p->split) - _p->splitterWidth / 2.f;
                    children[0]->setGeometry(bbox);
                    bbox.min.x = g.min.x;
                    bbox.min.y = bbox.max.y + _p->splitterWidth;
                    bbox.max.x = g.max.x;
                    bbox.max.y = g.max.y;
                    children[1]->setGeometry(bbox);
                    break;
                default: break;
                }
            }
            else
            {
                for (const auto& child : children)
                {
                    child->setGeometry(g);
                }
            }
        }

        void Splitter::paintEvent(PaintEvent& event)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    const BBox2f& sg = _getSplitterGeometry();
                    const BBox2f& hg = _getHandleGeometry();

                    // Draw the background.
                    const ColorRole colorRole = getBackgroundRole();
                    if (colorRole != ColorRole::None)
                    {
                        render->setFillColor(style->getColor(colorRole));
                        render->drawRectangle(sg);
                    }

                    // Draw the pressed state.
                    if (_p->pressedId)
                    {
                        render->setFillColor(style->getColor(ColorRole::Checked));
                        render->drawRectangle(hg);
                    }

                    // Draw the hovered state.
                    bool hover = _p->pressedId;
                    for (const auto& h : _p->hover)
                    {
                        hover |= h.second;
                    }
                    if (hover)
                    {
                        render->setFillColor(style->getColor(ColorRole::Hover));
                        render->drawRectangle(hg);
                    }
                }
            }
        }

        void Splitter::pointerEnterEvent(PointerEnterEvent& event)
        {
            if (!event.isRejected())
            {
                event.accept();

                _p->hover[event.getPointerInfo().id] = _getSplitterGeometry().contains(event.getPointerInfo().projectedPos);
            }
        }

        void Splitter::pointerLeaveEvent(PointerLeaveEvent& event)
        {
            event.accept();

            auto i = _p->hover.find(event.getPointerInfo().id);
            if (i != _p->hover.end())
            {
                _p->hover.erase(i);
            }
        }

        void Splitter::pointerMoveEvent(PointerMoveEvent& event)
        {
            const auto id = event.getPointerInfo().id;
            const auto& pos = event.getPointerInfo().projectedPos;
            _p->hover[id] = _getSplitterGeometry().contains(pos);

            if (_p->hover[id] || _p->pressedId)
            {
                event.accept();
            }

            if (_p->pressedId)
            {
                float p = 0.f;
                switch (_p->orientation)
                {
                case Orientation::Horizontal:
                    p = pos.x;
                    break;
                case Orientation::Vertical:
                    p = pos.y;
                    break;
                default: break;
                }
                setSplit(_posToValue(p));
            }
        }

        void Splitter::buttonPressEvent(ButtonPressEvent& event)
        {
            if (_p->pressedId)
                return;
            const auto id = event.getPointerInfo().id;
            if (_p->hover[id])
            {
                event.accept();

                _p->pressedId = id;
            }
        }

        void Splitter::buttonReleaseEvent(ButtonReleaseEvent& event)
        {
            if (event.getPointerInfo().id != _p->pressedId)
                return;
            event.accept();
            _p->pressedId = 0;
        }

        float Splitter::_valueToPos(float value) const
        {
            const BBox2f& g = getGeometry();
            float out = 0.f;
            switch (_p->orientation)
            {
            case Orientation::Horizontal:
                out = g.x() + _p->splitterWidth / 2.f + (g.w() - _p->splitterWidth) * value;
                break;
            case Orientation::Vertical:
                out = g.y() + _p->splitterWidth / 2.f + (g.h() - _p->splitterWidth) * value;
                break;
            default: break;
            }
            return out;
        }

        float Splitter::_posToValue(float value) const
        {
            const BBox2f& g = getGeometry();
            float out = 0.f;
            switch (_p->orientation)
            {
            case Orientation::Horizontal:
                out = Math::clamp((value - g.x() - _p->splitterWidth / 2.f) / (g.w() - _p->splitterWidth), 0.f, 1.f);
                break;
            case Orientation::Vertical:
                out = Math::clamp((value - g.y() - _p->splitterWidth / 2.f) / (g.h() - _p->splitterWidth), 0.f, 1.f);
                break;
            default: break;
            }
            return out;
        }

        BBox2f Splitter::_getSplitterGeometry() const
        {
            BBox2f out;
            const auto style = _getStyle();
            const BBox2f& g = getGeometry();
            switch (_p->orientation)
            {
            case Orientation::Horizontal:
                out.min.x = _valueToPos(_p->split) - _p->splitterWidth / 2.f;
                out.min.y = g.min.y;
                out.max.x = out.min.x + _p->splitterWidth;
                out.max.y = g.max.y;
                break;
            case Orientation::Vertical:
                out.min.x = g.min.x;
                out.min.y = _valueToPos(_p->split) - _p->splitterWidth / 2.f;
                out.max.x = g.max.x;
                out.max.y = out.min.y + _p->splitterWidth;
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
                out.min.x = g.min.x + w / 4.f;
                out.min.y = g.min.y;
                out.max.x = g.max.x - w / 4.f;
                out.max.y = g.max.y;
                break;
            }
            case Orientation::Vertical:
            {
                const float h = g.h();
                out.min.x = g.min.x;
                out.min.y = g.min.y + h / 4.f;
                out.max.x = g.max.x;
                out.max.y = g.max.y - h / 4.f;
                break;
            }
            default: break;
            }
            return out;
        }

    } // namespace UI
} // namespace djv

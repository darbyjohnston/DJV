// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/Splitter.h>

#include <djvUI/Style.h>

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
                std::vector<float> split;
                float splitterWidth = 0.F;
                ColorRole handleColorRole = ColorRole::Background;
                std::map<Event::PointerID, size_t> hover;
                std::pair<Event::PointerID, size_t> pressedID;
                std::function<void(const std::vector<float> &)> splitCallback;
            };

            void Splitter::_init(const std::shared_ptr<Context>& context)
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

            std::shared_ptr<Splitter> Splitter::create(Orientation orientation, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Splitter>(new Splitter);
                out->_init(context);
                out->setOrientation(orientation);
                return out;
            }

            Orientation Splitter::getOrientation() const
            {
                return _p->orientation;
            }

            void Splitter::setOrientation(Orientation value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.orientation)
                    return;
                p.orientation = value;
                _resize();
            }

            const std::vector<float> & Splitter::getSplit() const
            {
                return _p->split;
            }

            void Splitter::setSplit(const std::vector<float> & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.split)
                    return;
                p.split = value;
                _resize();
            }

            void Splitter::setSplitCallback(const std::function<void(const std::vector<float> &)> & callback)
            {
                _p->splitCallback = callback;
            }

            void Splitter::distributeEvenly()
            {
                DJV_PRIVATE_PTR();
                const auto& children = getChildWidgets();
                p.split.resize(children.size());
                const float size = children.size() ? 1.F / children.size() : 0.F;
                float x = size;
                for (size_t i = 0; i < children.size(); ++i)
                {
                    p.split[i] = x;
                    x += size;
                }
                _resize();
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
                float out = 0.F;
                const auto& style = _getStyle();
                const glm::vec2 m = getMargin().getSize(style);
                size_t i = 0;
                for (const auto & child : getChildWidgets())
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
                    if (i > 0)
                    {
                        switch (p.orientation)
                        {
                        case Orientation::Vertical:
                            out += p.splitterWidth;
                            break;
                        default: break;
                        }
                    }
                    ++i;
                }
                out += m.y;
                return out;
            }

            void Splitter::addChild(const std::shared_ptr<IObject> & child)
            {
                Widget::addChild(child);
                distributeEvenly();
            }

            void Splitter::removeChild(const std::shared_ptr<IObject>& child)
            {
                Widget::removeChild(child);
                distributeEvenly();
            }

            void Splitter::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                size_t i = 0;
                for (const auto & child : getChildWidgets())
                {
                    if (child->isVisible())
                    {
                        const glm::vec2 & size = child->getMinimumSize();
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
                        if (i > 0)
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
                        ++i;
                    }
                }
                const auto& style = _getStyle();
                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }

            void Splitter::_layoutEvent(Event::Layout & event)
            {
                const auto g = _getChildGeometry();
                size_t i = 0;
                for (const auto & child : getChildWidgets())
                {
                    child->setGeometry(g[i]);
                    ++i;
                }
            }

            void Splitter::_paintEvent(Event::Paint & event)
            {
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                const auto hg = _getHandleGeometry();

                auto render = _getRender();
                size_t i = 0;
                for (const auto & g : hg)
                {
                    render->setFillColor(style->getColor(p.handleColorRole));
                    render->drawRect(g);

                    if (p.pressedID.first && p.pressedID.second == i)
                    {
                        render->setFillColor(style->getColor(ColorRole::Pressed));
                        render->drawRect(g);
                    }
                    else
                    {
                        for (const auto j : p.hover)
                        {
                            if (j.second == i)
                            {
                                render->setFillColor(style->getColor(ColorRole::Hovered));
                                render->drawRect(g);
                                break;
                            }
                        }
                    }

                    ++i;
                }
            }

            void Splitter::_pointerLeaveEvent(Event::PointerLeave &)
            {
                DJV_PRIVATE_PTR();
                if (p.hover.size())
                {
                    p.hover.clear();
                    _redraw();
                }
            }

            void Splitter::_pointerMoveEvent(Event::PointerMove & event)
            {
                DJV_PRIVATE_PTR();
                event.accept();
                const auto id = event.getPointerInfo().id;
                const auto & pos = event.getPointerInfo().projectedPos;
                if (p.pressedID.first)
                {
                    float newPos = 0.F;
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
                    const float v = _posToValue(newPos);
                    p.split[p.pressedID.second] = v;
                    for (size_t i = 0; i < p.pressedID.second; ++i)
                    {
                        p.split[i] = std::min(p.split[i], v);
                    }
                    for (size_t i = p.pressedID.second + 1; i < p.split.size(); ++i)
                    {
                        p.split[i] = std::max(p.split[i], v);
                    }
                    _resize();
                    if (p.splitCallback)
                    {
                        p.splitCallback(p.split);
                    }
                }
                else
                {
                    bool hover = false;
                    size_t i = 0;
                    for (const auto & g : _getHandleGeometry())
                    {
                        if (g.contains(event.getPointerInfo().projectedPos))
                        {
                            hover = true;
                            break;
                        }
                        ++i;
                    }
                    if (hover)
                    {
                        const auto j = p.hover.find(id);
                        if (j == p.hover.end())
                        {
                            p.hover[id] = i;
                            _redraw();
                        }
                    }
                    else
                    {
                        const auto j = p.hover.find(id);
                        if (j != p.hover.end())
                        {
                            p.hover.erase(j);
                            _redraw();
                        }
                    }
                }
            }

            void Splitter::_buttonPressEvent(Event::ButtonPress & event)
            {
                DJV_PRIVATE_PTR();
                if (p.pressedID.first)
                    return;
                const auto id = event.getPointerInfo().id;
                const auto i = p.hover.find(id);
                if (i != p.hover.end())
                {
                    event.accept();
                    p.pressedID.first = id;
                    p.pressedID.second = i->second;
                    _redraw();
                }
            }

            void Splitter::_buttonReleaseEvent(Event::ButtonRelease & event)
            {
                DJV_PRIVATE_PTR();
                if (event.getPointerInfo().id != p.pressedID.first)
                    return;
                event.accept();
                p.pressedID.first = Event::invalidID;
                p.pressedID.second = 0;
                _redraw();
            }

            void Splitter::_initEvent(Event::Init & event)
            {
                Widget::_initEvent(event);
                DJV_PRIVATE_PTR();
                const auto& style = _getStyle();
                p.splitterWidth = style->getMetric(MetricsRole::Handle);
                _resize();
            }

            float Splitter::_valueToPos(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out = g.min.x + ceilf(g.w() * value);
                    break;
                case Orientation::Vertical:
                    out = g.min.y + ceilf(g.h() * value);
                    break;
                default: break;
                }
                return out;
            }

            float Splitter::_posToValue(float value) const
            {
                DJV_PRIVATE_PTR();
                float out = 0.F;
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                switch (p.orientation)
                {
                case Orientation::Horizontal:
                    out = Math::clamp((value - g.min.x) / g.w(), 0.F, 1.F);
                    break;
                case Orientation::Vertical:
                    out = Math::clamp((value - g.min.y) / g.h(), 0.F, 1.F);
                    break;
                default: break;
                }
                return out;
            }

            std::vector<BBox2f> Splitter::_getChildGeometry() const
            {
                DJV_PRIVATE_PTR();
                std::vector<BBox2f> out;
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const float sw = ceilf(p.splitterWidth / 2.F);
                float x = g.min.x;
                float y = g.min.y;
                const auto& children = getChildWidgets();
                for (size_t i = 0; i < children.size(); ++i)
                {
                    BBox2f bbox;
                    const float v = _valueToPos(p.split[i]);
                    switch (p.orientation)
                    {
                    case Orientation::Horizontal:
                        bbox.min.x = x + (i > 0 ? sw : 0);
                        bbox.min.y = y;
                        bbox.max.x = v - (i < (children.size() - 1) ? sw : 0);
                        bbox.max.y = g.max.y;
                        x = v;
                        break;
                    case Orientation::Vertical:
                        bbox.min.x = x;
                        bbox.min.y = y + (i > 0 ? sw : 0);
                        bbox.max.x = g.max.x;
                        bbox.max.y = v - (i < (children.size() - 1) ? sw : 0);
                        y = v;
                        break;
                    default: break;
                    }
                    out.push_back(bbox);
                }
                return out;
            }

            std::vector<BBox2f> Splitter::_getHandleGeometry() const
            {
                DJV_PRIVATE_PTR();
                std::vector<BBox2f> out;
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const float sw = ceilf(p.splitterWidth / 2.F);
                float x = g.min.x;
                float y = g.min.y;
                const auto& children = getChildWidgets();
                for (size_t i = 0; i < children.size(); ++i)
                {
                    if (i < children.size() - 1)
                    {
                        BBox2f bbox;
                        const float v = _valueToPos(p.split[i]);
                        switch (p.orientation)
                        {
                        case Orientation::Horizontal:
                            bbox.min.x = v - sw;
                            bbox.min.y = y;
                            bbox.max.x = v + sw;
                            bbox.max.y = g.max.y;
                            x = v;
                            break;
                        case Orientation::Vertical:
                            bbox.min.x = x;
                            bbox.min.y = v - sw;
                            bbox.max.x = g.max.x;
                            bbox.max.y = v + sw;
                            y = v;
                            break;
                        default: break;
                        }
                        out.push_back(bbox);
                    }
                }
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

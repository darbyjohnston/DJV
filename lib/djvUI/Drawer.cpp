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

#include <djvUI/Drawer.h>

#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Drawer::Private
            {
                bool open = false;
                Side side = Side::First;
                std::shared_ptr<Stack> layout;
            };

            void Drawer::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Drawer");

                _p->layout = Stack::create(context);
                _p->layout->setBackgroundRole(ColorRole::Background);
                _p->layout->setPointerEnabled(true);
                Widget::addChild(_p->layout);
            }

            Drawer::Drawer() :
                _p(new Private)
            {}

            Drawer::~Drawer()
            {}

            std::shared_ptr<Drawer> Drawer::create(Context * context)
            {
                auto out = std::shared_ptr<Drawer>(new Drawer);
                out->_init(context);
                return out;
            }

            bool Drawer::isOpen() const
            {
                return _p->open;
            }

            void Drawer::setOpen(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.open)
                    return;
                p.open = value;
                _resize();
            }

            void Drawer::open()
            {
                setOpen(true);
            }

            void Drawer::close()
            {
                setOpen(false);
            }

            Side Drawer::getSide() const
            {
                return _p->side;
            }

            void Drawer::setSide(Side value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.side)
                    return;
                p.side = value;
                _resize();
            }

            void Drawer::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->layout->addChild(value);
            }

            void Drawer::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->layout->removeChild(value);
            }

            void Drawer::clearChildren()
            {
                _p->layout->clearChildren();
            }

            void Drawer::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                glm::vec2 size = _p->layout->getMinimumSize();
                switch (p.side)
                {
                case Side::Left:   size.x += sh; break;
                case Side::Top:    size.y += sh; break;
                case Side::Right:  size.x += sh; break;
                case Side::Bottom: size.y += sh; break;
                default: break;
                }
                _setMinimumSize(size);
            }

            void Drawer::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                auto style = _getStyle();
                const BBox2f & g = getGeometry();
                const glm::vec2 & minimumSize = p.layout->getMinimumSize();
                const float sh = style->getMetric(MetricsRole::Shadow);
                BBox2f childGeometry = g;
                switch (p.side)
                {
                case Side::Left:   childGeometry.max.x = (p.open ? (g.min.x + minimumSize.x) : g.min.x) + sh; break;
                case Side::Top:    childGeometry.max.y = (p.open ? (g.min.y + minimumSize.y) : g.min.y) + sh; break;
                case Side::Right:  childGeometry.min.x = (p.open ? (g.max.x - minimumSize.x) : g.max.x) + sh; break;
                case Side::Bottom: childGeometry.min.y = (p.open ? (g.max.y - minimumSize.y) : g.max.y) + sh; break;
                default: break;
                }
                _p->layout->setGeometry(childGeometry);
            }

            void Drawer::_paintEvent(Event::Paint & event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                if (p.open)
                {
                    const BBox2f & g = p.layout->getGeometry();
                    auto style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    auto render = _getRender();
                    render->setFillColor(_getColorWithOpacity(style->getColor(ColorRole::Shadow)));
                    switch (p.side)
                    {
                    case Side::Left:   render->drawShadow(BBox2f(g.max.x + sh, g.min.y, sh, g.h()), AV::Side::Right); break;
                    case Side::Top:    render->drawShadow(BBox2f(g.min.x, g.max.y, g.w(), sh), AV::Side::Bottom);     break;
                    case Side::Right:  render->drawShadow(BBox2f(g.min.x - sh, g.min.y, sh, g.h()), AV::Side::Left);  break;
                    case Side::Bottom: render->drawShadow(BBox2f(g.min.x, g.min.y - sh, g.w(), sh), AV::Side::Top);   break;
                    default: break;
                    }
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvUI/Spacer.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Timer.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            namespace
            {
                //! \todo Should this be configurable?
                const size_t animationTime = 100;

            } // namespace

            struct Drawer::Private
            {
                Side side = Side::First;
                bool open = false;
                float openAmount = 0.F;
                std::shared_ptr<Animation::Animation> openAnimation;
                std::shared_ptr<Stack> childLayout;
            };

            void Drawer::_init(Side side, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();

                setClassName("djv::UI::Layout::Drawer");

                p.side = side;

                p.openAnimation = Animation::Animation::create(context);

                p.childLayout = Stack::create(context);
                Widget::addChild(p.childLayout);
            }

            Drawer::Drawer() :
                _p(new Private)
            {}

            Drawer::~Drawer()
            {}

            std::shared_ptr<Drawer> Drawer::create(Side side, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Drawer>(new Drawer);
                out->_init(side, context);
                return out;
            }

            Side Drawer::getSide() const
            {
                return _p->side;
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
                auto weak = std::weak_ptr<Drawer>(std::dynamic_pointer_cast<Drawer>(shared_from_this()));
                if (p.open)
                {
                    _openStart();
                    p.openAnimation->start(
                        0.F,
                        1.F,
                        std::chrono::milliseconds(animationTime),
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                widget->_resize();
                            }
                        },
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                widget->_openEnd();
                                widget->_resize();
                            }
                        });
                }
                else
                {
                    _closeStart();
                    p.openAnimation->start(
                        1.F,
                        0.F,
                        std::chrono::milliseconds(animationTime),
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                widget->_resize();
                            }
                        },
                        [weak](float value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->openAmount = value;
                                widget->_closeEnd();
                                widget->_resize();
                            }
                        });
                }
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

            void Drawer::addChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->addChild(value);
            }

            void Drawer::removeChild(const std::shared_ptr<IObject> & value)
            {
                _p->childLayout->removeChild(value);
            }

            void Drawer::clearChildren()
            {
                _p->childLayout->clearChildren();
            }

            void Drawer::_preLayoutEvent(Event::PreLayout & event)
            {
                DJV_PRIVATE_PTR();
                const glm::vec2& minimumSize = p.childLayout->getMinimumSize();
                glm::vec2 size(0.F, 0.F);
                if (p.open)
                {
                    switch (p.side)
                    {
                    case Side::Left:
                    case Side::Right:
                        size.x = minimumSize.x * p.openAmount;
                        size.y = minimumSize.y;
                        break;
                    case Side::Top:
                    case Side::Bottom:
                        size.x = minimumSize.x;
                        size.y = minimumSize.y * p.openAmount;
                        break;
                    default: break;
                    }
                }
                _setMinimumSize(size);
            }

            void Drawer::_layoutEvent(Event::Layout & event)
            {
                DJV_PRIVATE_PTR();
                const BBox2f & g = getGeometry();
                const glm::vec2 & minimumSize = p.childLayout->getMinimumSize();
                BBox2f childGeometry(0.F, 0.F, 0.F, 0.F);
                switch (p.side)
                {
                case Side::Left:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.min.x + (minimumSize.x * p.openAmount);
                    childGeometry.max.y = g.max.y;
                    break;
                case Side::Top:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.min.y + (minimumSize.y * p.openAmount);
                    break;
                case Side::Right:
                    childGeometry.min.x = g.max.x - (minimumSize.x * p.openAmount);
                    childGeometry.min.y = g.min.y;
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.max.y;
                    break;
                case Side::Bottom:
                    childGeometry.min.x = g.min.x;
                    childGeometry.min.y = g.max.y - (minimumSize.x * p.openAmount);
                    childGeometry.max.x = g.max.x;
                    childGeometry.max.y = g.max.y;
                    break;
                default: break;
                }
                p.childLayout->setGeometry(childGeometry);
            }

            void Drawer::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                DJV_PRIVATE_PTR();
                if (p.open)
                {
                    const BBox2f& g = p.childLayout->getGeometry();
                    const auto& style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    auto render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Shadow));
                    switch (p.side)
                    {
                    case Side::Left:   render->drawShadow(BBox2f(g.max.x, g.min.y, sh, g.h()), AV::Side::Right);     break;
                    case Side::Top:    render->drawShadow(BBox2f(g.min.x, g.max.y, g.w(), sh), AV::Side::Bottom);    break;
                    case Side::Right:  render->drawShadow(BBox2f(g.min.x - sh, g.min.y, sh, g.h()), AV::Side::Left); break;
                    case Side::Bottom: render->drawShadow(BBox2f(g.min.x, g.min.y - sh, g.w(), sh), AV::Side::Top);  break;
                    default: break;
                    }
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/MDIWidget.h>

#include <djvUI/Style.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            struct IWidget::Private
            {
                bool active = false;
                float maximize = 0.F;
                Handle hovered = Handle::None;
                Handle pressed = Handle::None;
            };

            void IWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setPointerEnabled(true);
            }

            IWidget::IWidget() :
                _p(new Private)
            {}

            IWidget::~IWidget()
            {}

            std::map<Handle, std::vector<BBox2f> > IWidget::_getHandles() const
            {
                std::map<Handle, std::vector<BBox2f> > out;
                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                const float corner = style->getMetric(MetricsRole::Handle) * 2.F;
                out =
                {
                    {
                        Handle::Move,
                        {
                            g.margin(-corner)
                        }
                    },
                    {
                        Handle::ResizeE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y + corner),
                                glm::vec2(g.min.x + corner, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeN,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.min.y),
                                glm::vec2(g.max.x - corner, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y + corner),
                                glm::vec2(g.max.x, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeS,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.max.y - corner),
                                glm::vec2(g.max.x - corner, g.max.y))
                        }
                    },
                    {
                        Handle::ResizeNE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.min.x + corner, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeNW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y),
                                glm::vec2(g.max.x, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeSW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.max.y - corner),
                                glm::vec2(g.max.x, g.max.y))
                        }
                    },
                    {
                        Handle::ResizeSE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.max.y - corner),
                                glm::vec2(g.min.x + corner, g.max.y))
                        }
                    }
                };
                return out;
            }

            std::map<Handle, std::vector<BBox2f> > IWidget::_getHandlesDraw() const
            {
                std::map<Handle, std::vector<BBox2f> > out;
                const BBox2f & g = getGeometry();
                const auto& style = _getStyle();
                const float edge = ceilf(style->getMetric(MetricsRole::Handle) * .75F);
                const float corner = style->getMetric(MetricsRole::Handle) * 2.F;
                out =
                {
                    {
                        Handle::ResizeE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y + corner),
                                glm::vec2(g.min.x + edge, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeN,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.min.y),
                                glm::vec2(g.max.x - corner, g.min.y + edge))
                        }
                    },
                    {
                        Handle::ResizeW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - edge, g.min.y + corner),
                                glm::vec2(g.max.x, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeS,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.max.y - edge),
                                glm::vec2(g.max.x - corner, g.max.y))
                            }
                    },
                    {
                        Handle::ResizeNE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.min.x + corner, g.min.y + edge)),
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y + edge),
                                glm::vec2(g.min.x + edge, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeNW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y),
                                glm::vec2(g.max.x, g.min.y + edge)),
                            BBox2f(
                                glm::vec2(g.max.x - edge, g.min.y + edge),
                                glm::vec2(g.max.x, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeSW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.max.y - edge),
                                glm::vec2(g.max.x, g.max.y)),
                            BBox2f(
                                glm::vec2(g.max.x - edge, g.max.y - corner),
                                glm::vec2(g.max.x, g.max.y - edge))
                        }
                    },
                    {
                        Handle::ResizeSE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.max.y - edge),
                                glm::vec2(g.min.x + corner, g.max.y)),
                            BBox2f(
                                glm::vec2(g.min.x, g.max.y - corner),
                                glm::vec2(g.min.x + edge, g.max.y - edge))
                        }
                    }
                };
                return out;
            }

            float IWidget::_getMaximize() const
            {
                return _p->maximize;
            }

            void IWidget::_setMaximize(float value)
            {
                _p->maximize = value;
            }

            void IWidget::_setHandleHovered(Handle value)
            {
                if (value == _p->hovered)
                    return;
                _p->hovered = value;
                _redraw();
            }

            void IWidget::_setHandlePressed(Handle value)
            {
                if (value == _p->pressed)
                    return;
                _p->pressed = value;
                _redraw();
            }

            void IWidget::_setActiveWidget(bool value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.active)
                    return;
                p.active = value;
                _redraw();
            }

            void IWidget::_paintEvent(Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                if (p.maximize < 1.F)
                {
                    const auto& style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    auto render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Shadow));
                    const BBox2f& g = getGeometry().margin(0, -sh, 0, 0);
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void IWidget::_paintOverlayEvent(Event::PaintOverlay&)
            {
                DJV_PRIVATE_PTR();
                if (p.maximize < 1.F)
                {
                    const auto& style = _getStyle();
                    auto render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Handle));
                    const auto& handles = _getHandlesDraw();
                    const auto i = handles.find(p.pressed);
                    if (i != handles.end())
                    {
                        for (const auto& j : i->second)
                        {
                            render->drawRect(j);
                        }
                    }
                    if (p.hovered != p.pressed)
                    {
                        const auto i = handles.find(p.hovered);
                        if (i != handles.end())
                        {
                            for (const auto& j : i->second)
                            {
                                render->drawRect(j);
                            }
                        }
                    }
                }
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv

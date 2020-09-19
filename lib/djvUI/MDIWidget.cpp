// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/MDIWidget.h>

#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

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

            void IWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setPointerEnabled(true);
            }

            IWidget::IWidget() :
                _p(new Private)
            {}

            IWidget::~IWidget()
            {}

            std::map<Handle, std::vector<Math::BBox2f> > IWidget::_getHandles() const
            {
                std::map<Handle, std::vector<Math::BBox2f> > out;
                const Math::BBox2f& g = getGeometry();
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
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.min.y + corner),
                                glm::vec2(g.min.x + corner, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeN,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x + corner, g.min.y),
                                glm::vec2(g.max.x - corner, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y + corner),
                                glm::vec2(g.max.x, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeS,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x + corner, g.max.y - corner),
                                glm::vec2(g.max.x - corner, g.max.y))
                        }
                    },
                    {
                        Handle::ResizeNE,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.min.x + corner, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeNW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y),
                                glm::vec2(g.max.x, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeSW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - corner, g.max.y - corner),
                                glm::vec2(g.max.x, g.max.y))
                        }
                    },
                    {
                        Handle::ResizeSE,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.max.y - corner),
                                glm::vec2(g.min.x + corner, g.max.y))
                        }
                    }
                };
                return out;
            }

            std::map<Handle, std::vector<Math::BBox2f> > IWidget::_getHandlesDraw() const
            {
                std::map<Handle, std::vector<Math::BBox2f> > out;
                const Math::BBox2f& g = getGeometry();
                const auto& style = _getStyle();
                const float edge = ceilf(style->getMetric(MetricsRole::Handle) * .75F);
                const float corner = style->getMetric(MetricsRole::Handle) * 2.F;
                out =
                {
                    {
                        Handle::ResizeE,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.min.y + corner),
                                glm::vec2(g.min.x + edge, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeN,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x + corner, g.min.y),
                                glm::vec2(g.max.x - corner, g.min.y + edge))
                        }
                    },
                    {
                        Handle::ResizeW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - edge, g.min.y + corner),
                                glm::vec2(g.max.x, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeS,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x + corner, g.max.y - edge),
                                glm::vec2(g.max.x - corner, g.max.y))
                            }
                    },
                    {
                        Handle::ResizeNE,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.min.x + corner, g.min.y + edge)),
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.min.y + edge),
                                glm::vec2(g.min.x + edge, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeNW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y),
                                glm::vec2(g.max.x, g.min.y + edge)),
                            Math::BBox2f(
                                glm::vec2(g.max.x - edge, g.min.y + edge),
                                glm::vec2(g.max.x, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeSW,
                        {
                            Math::BBox2f(
                                glm::vec2(g.max.x - corner, g.max.y - edge),
                                glm::vec2(g.max.x, g.max.y)),
                            Math::BBox2f(
                                glm::vec2(g.max.x - edge, g.max.y - corner),
                                glm::vec2(g.max.x, g.max.y - edge))
                        }
                    },
                    {
                        Handle::ResizeSE,
                        {
                            Math::BBox2f(
                                glm::vec2(g.min.x, g.max.y - edge),
                                glm::vec2(g.min.x + corner, g.max.y)),
                            Math::BBox2f(
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

            void IWidget::_paintEvent(System::Event::Paint& event)
            {
                DJV_PRIVATE_PTR();
                if (p.maximize < 1.F)
                {
                    const auto& style = _getStyle();
                    const float sh = style->getMetric(MetricsRole::Shadow);
                    const auto& render = _getRender();
                    render->setFillColor(style->getColor(ColorRole::Shadow));
                    const Math::BBox2f& g = getGeometry().margin(0, -sh, 0, 0);
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void IWidget::_paintOverlayEvent(System::Event::PaintOverlay&)
            {
                DJV_PRIVATE_PTR();
                if (p.maximize < 1.F)
                {
                    const auto& style = _getStyle();
                    const auto& render = _getRender();
                    const auto& handles = _getHandlesDraw();
                    const auto i = handles.find(p.pressed);
                    std::vector<Math::BBox2f> rects;
                    if (i != handles.end())
                    {
                        for (const auto& j : i->second)
                        {
                            rects.push_back(j);
                        }
                    }
                    if (p.hovered != p.pressed)
                    {
                        const auto i = handles.find(p.hovered);
                        if (i != handles.end())
                        {
                            for (const auto& j : i->second)
                            {
                                rects.push_back(j);
                            }
                        }
                    }
                    render->setFillColor(style->getColor(ColorRole::Handle));
                    render->drawRects(rects);
                }
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv

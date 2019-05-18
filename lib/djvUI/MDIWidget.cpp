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

#include <djvUI/MDIWidget.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            void IWidget::_init(Context* context)
            {
                Widget::_init(context);
            }

            IWidget::IWidget()
            {}

            IWidget::~IWidget()
            {}

            std::map<Handle, std::vector<BBox2f> > IWidget::getHandles() const
            {
                std::map<Handle, std::vector<BBox2f> > out;
                const BBox2f & g = getGeometry();
                auto style = _getStyle();
                const float edge = style->getMetric(MetricsRole::Handle);
                const float corner = style->getMetric(MetricsRole::Handle) * 2.f;
                out =
                {
                    {
                        Handle::Move,
                        {
                            g.margin(-edge)
                        }
                    },
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

            std::map<Handle, std::vector<BBox2f> > IWidget::getHandlesDraw() const
            {
                std::map<Handle, std::vector<BBox2f> > out;
                const BBox2f & g = getGeometry();
                auto style = _getStyle();
                const float edge = style->getMetric(MetricsRole::Handle);
                const float corner = style->getMetric(MetricsRole::Handle) * 2.f;
                out =
                {
                    {
                        Handle::Move,
                        {
                            g.margin(-edge)
                        }
                    },
                    {
                        Handle::ResizeE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y + corner),
                                glm::vec2(g.min.x + edge / 2.f, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeN,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.min.y),
                                glm::vec2(g.max.x - corner, g.min.y + edge / 2.f))
                        }
                    },
                    {
                        Handle::ResizeW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - edge / 2.f, g.min.y + corner),
                                glm::vec2(g.max.x, g.max.y - corner))
                        }
                    },
                    {
                        Handle::ResizeS,
                        {
                            BBox2f(
                                glm::vec2(g.min.x + corner, g.max.y - edge / 2.f),
                                glm::vec2(g.max.x - corner, g.max.y))
                            }
                    },
                    {
                        Handle::ResizeNE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y),
                                glm::vec2(g.min.x + corner, g.min.y + edge / 2.f)),
                            BBox2f(
                                glm::vec2(g.min.x, g.min.y + edge / 2.f),
                                glm::vec2(g.min.x + edge / 2.f, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeNW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.min.y),
                                glm::vec2(g.max.x, g.min.y + edge / 2.f)),
                            BBox2f(
                                glm::vec2(g.max.x - edge / 2.f, g.min.y + edge / 2.f),
                                glm::vec2(g.max.x, g.min.y + corner))
                        }
                    },
                    {
                        Handle::ResizeSW,
                        {
                            BBox2f(
                                glm::vec2(g.max.x - corner, g.max.y - edge / 2.f),
                                glm::vec2(g.max.x, g.max.y)),
                            BBox2f(
                                glm::vec2(g.max.x - edge / 2.f, g.max.y - corner),
                                glm::vec2(g.max.x, g.max.y - edge / 2.f))
                        }
                    },
                    {
                        Handle::ResizeSE,
                        {
                            BBox2f(
                                glm::vec2(g.min.x, g.max.y - edge / 2.f),
                                glm::vec2(g.min.x + corner, g.max.y)),
                            BBox2f(
                                glm::vec2(g.min.x, g.max.y - corner),
                                glm::vec2(g.min.x + edge / 2.f, g.max.y - edge / 2.f))
                        }
                    }
                };
                return out;
            }

            void IWidget::setMaximized(float)
            {}

        } // namespace MDI
    } // namespace UI
} // namespace djdv

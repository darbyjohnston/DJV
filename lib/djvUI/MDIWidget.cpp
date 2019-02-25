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
            IWidget::IWidget()
            {}

            IWidget::~IWidget()
            {}

            namespace
            {
                std::map<Handle, std::vector<BBox2f> > _getHandleBBox(const BBox2f & bbox, float edge, float corner)
                {
                    return
                    {
                        {
                            Handle::Move,
                            {
                                bbox.margin(-edge)
                            }
                        },
                        {
                            Handle::ResizeE,
                            {
                                BBox2f(
                                    glm::vec2(bbox.min.x, bbox.min.y + corner),
                                    glm::vec2(bbox.min.x + edge, bbox.max.y - corner))
                            }
                        },
                        {
                            Handle::ResizeN,
                            {
                                BBox2f(
                                    glm::vec2(bbox.min.x + corner, bbox.min.y),
                                    glm::vec2(bbox.max.x - corner, bbox.min.y + edge))
                            }
                        },
                        {
                            Handle::ResizeW,
                            {
                                BBox2f(
                                    glm::vec2(bbox.max.x - edge, bbox.min.y + corner),
                                    glm::vec2(bbox.max.x, bbox.max.y - corner))
                            }
                        },
                        {
                            Handle::ResizeS,
                            {
                                BBox2f(
                                    glm::vec2(bbox.min.x + corner, bbox.max.y - edge),
                                    glm::vec2(bbox.max.x - corner, bbox.max.y))
                                }
                        },
                        {
                            Handle::ResizeNE,
                            {
                                BBox2f(
                                    glm::vec2(bbox.min.x, bbox.min.y),
                                    glm::vec2(bbox.min.x + corner, bbox.min.y + edge)),
                                BBox2f(
                                    glm::vec2(bbox.min.x, bbox.min.y + edge),
                                    glm::vec2(bbox.min.x + edge, bbox.min.y + corner))
                            }
                        },
                        {
                            Handle::ResizeNW,
                            {
                                BBox2f(
                                    glm::vec2(bbox.max.x - corner, bbox.min.y),
                                    glm::vec2(bbox.max.x, bbox.min.y + edge)),
                                BBox2f(
                                    glm::vec2(bbox.max.x - edge, bbox.min.y + edge),
                                    glm::vec2(bbox.max.x, bbox.min.y + corner))
                            }
                        },
                        {
                            Handle::ResizeSW,
                            {
                                BBox2f(
                                    glm::vec2(bbox.max.x - corner, bbox.max.y - edge),
                                    glm::vec2(bbox.max.x, bbox.max.y)),
                                BBox2f(
                                    glm::vec2(bbox.max.x - edge, bbox.max.y - corner),
                                    glm::vec2(bbox.max.x, bbox.max.y - edge))
                            }
                        },
                        {
                            Handle::ResizeSE,
                            {
                                BBox2f(
                                    glm::vec2(bbox.min.x, bbox.max.y - edge),
                                    glm::vec2(bbox.min.x + corner, bbox.max.y)),
                                BBox2f(
                                    glm::vec2(bbox.min.x, bbox.max.y - corner),
                                    glm::vec2(bbox.min.x + edge, bbox.max.y - edge))
                            }
                        }
                    };
                }
            
            } // namespace

            Handle IWidget::getHandle(const glm::vec2 & pos) const
            {
                Handle out = Handle::None;
                if (auto style = _getStyle().lock())
                {
                    const float m = style->getMetric(MetricsRole::Handle);
                    for (const auto & i : _getHandleBBox(getGeometry(), m, m * 2.f))
                    {
                        for (const auto & j : i.second)
                        {
                            if (j.contains(pos))
                            {
                                out = i.first;
                                break;
                            }
                        }
                    }
                }
                return out;
            }
            
            std::vector<BBox2f> IWidget::getHandleBBox(Handle value) const
            {
                std::vector<BBox2f> out;
                if (auto style = _getStyle().lock())
                {
                    const float m = style->getMetric(MetricsRole::Handle);
                    const auto & bbox = _getHandleBBox(getGeometry(), m, m * 2.f);
                    const auto i = bbox.find(value);
                    if (i != bbox.end())
                    {
                        out = i->second;
                    }
                }
                return out;
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv

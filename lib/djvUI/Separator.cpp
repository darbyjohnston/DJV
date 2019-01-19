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

#include <djvUI/Separator.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Separator::Private
            {
                float width = 0.f;
                AV::Image::Color color;
            };

            void Separator::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Separator");
            }

            Separator::Separator() :
                _p(new Private)
            {}

            Separator::~Separator()
            {}

            std::shared_ptr<Separator> Separator::create(Context * context)
            {
                auto out = std::shared_ptr<Separator>(new Separator);
                out->_init(context);
                return out;
            }

            void Separator::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    DJV_PRIVATE_PTR();
                    p.width = style->getMetric(Style::MetricsRole::Border);
                    p.color = style->getColor(Style::ColorRole::Border);

                    glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                    minimumSize += p.width;
                    _setMinimumSize(minimumSize);
                }
            }

            void Separator::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRender().lock())
                {
                    const BBox2f& g = getGeometry();
                    render->setFillColor(_getColorWithOpacity(_p->color));
                    render->drawRectangle(g);
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

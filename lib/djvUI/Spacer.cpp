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

#include <djvUI/Spacer.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Spacer::Private
            {
                Orientation orientation = Orientation::First;
				MetricsRole spacerSize = MetricsRole::Spacing;
            };

            void Spacer::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Spacer");
            }

            Spacer::Spacer() :
                _p(new Private)
            {}

            Spacer::~Spacer()
            {}

            std::shared_ptr<Spacer> Spacer::create(Orientation orientation, Context * context)
            {
                auto out = std::shared_ptr<Spacer>(new Spacer);
                out->_init(context);
                out->setOrientation(orientation);
                return out;
            }

            Orientation Spacer::getOrientation() const
            {
                return _p->orientation;
            }

            void Spacer::setOrientation(Orientation value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.orientation)
                    return;
                p.orientation = value;
                _resize();
            }

			MetricsRole Spacer::getSpacerSize() const
            {
                return _p->spacerSize;
            }

            void Spacer::setSpacerSize(MetricsRole value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacerSize)
                    return;
                p.spacerSize = value;
                _resize();
            }

            void Spacer::_preLayoutEvent(Event::PreLayout& event)
            {
                if (auto style = _getStyle().lock())
                {
                    glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                    DJV_PRIVATE_PTR();
                    switch (p.orientation)
                    {
                    case Orientation::Horizontal: minimumSize.x = style->getMetric(p.spacerSize); break;
                    case Orientation::Vertical:   minimumSize.y = style->getMetric(p.spacerSize); break;
                    default: break;
                    }
                    _setMinimumSize(minimumSize);
                }
            }

            HorizontalSpacer::HorizontalSpacer()
            {}

            std::shared_ptr<HorizontalSpacer> HorizontalSpacer::create(Context * context)
            {
                auto out = std::shared_ptr<HorizontalSpacer>(new HorizontalSpacer);
                out->_init(context);
                out->setOrientation(Orientation::Horizontal);
                return out;
            }

            VerticalSpacer::VerticalSpacer()
            {}

            std::shared_ptr<VerticalSpacer> VerticalSpacer::create(Context * context)
            {
                auto out = std::shared_ptr<VerticalSpacer>(new VerticalSpacer);
                out->_init(context);
                out->setOrientation(Orientation::Vertical);
                return out;
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

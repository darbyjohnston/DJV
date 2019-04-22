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

#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Stack::Private
            {};

            void Stack::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::Layout::Stack");
            }

            Stack::Stack() :
                _p(new Private)
            {}

            Stack::~Stack()
            {}

            std::shared_ptr<Stack> Stack::create(Context * context)
            {
                auto out = std::shared_ptr<Stack>(new Stack);
                out->_init(context);
                return out;
            }

            float Stack::getHeightForWidth(float value) const
            {
                return heightForWidth(value, getChildrenT<Widget>(), getMargin(), _getStyle());
            }

            float Stack::heightForWidth(float value, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                float out = 0.f;
                const glm::vec2 m = margin.getSize(style);
                for (const auto& child : children)
                {
                    if (child->isVisible())
                    {
                        out = glm::max(out, child->getHeightForWidth(value - m.x));
                    }
                }
                out += m.y;
                return out;
            }

            glm::vec2 Stack::minimumSize(const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                for (const auto& child : children)
                {
                    if (child->isVisible())
                    {
                        minimumSize = glm::max(minimumSize, child->getMinimumSize());
                    }
                }
                return minimumSize + margin.getSize(style);
            }

            void Stack::layout(const Core::BBox2f& geometry, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                const BBox2f& g = margin.bbox(geometry, style);
                for (const auto& child : children)
                {
                    const BBox2f childGeometry = Widget::getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                    child->setGeometry(childGeometry);
                }
            }

            void Stack::_preLayoutEvent(Event::PreLayout &)
            {
                _setMinimumSize(minimumSize(getChildrenT<Widget>(), getMargin(), _getStyle()));
            }

            void Stack::_layoutEvent(Event::Layout &)
            {
                layout(getGeometry(), getChildrenT<Widget>(), getMargin(), _getStyle());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

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

#include <djvUI/StackLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct StackLayout::Private
        {};

        void StackLayout::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setName("djv::UI::StackLayout");
        }
        
        StackLayout::StackLayout() :
            _p(new Private)
        {}

        StackLayout::~StackLayout()
        {}

        std::shared_ptr<StackLayout> StackLayout::create(Context * context)
        {
            auto out = std::shared_ptr<StackLayout>(new StackLayout);
            out->_init(context);
            return out;
        }

        float StackLayout::getHeightForWidth(float value) const
        {
            float out = 0.f;
            if (auto style = _getStyle().lock())
            {
                for (const auto& child : getChildrenT<Widget>())
                {
                    if (child->isVisible())
                    {
                        out = glm::max(out, child->getHeightForWidth(value));
                    }
                }
                out += getMargin().getHeight(style);
            }
            return out;
        }

        void StackLayout::_preLayoutEvent(PreLayoutEvent&)
        {
            if (auto style = _getStyle().lock())
            {
                glm::vec2 minimumSize = glm::vec2(0.f, 0.f);
                for (const auto& child : getChildrenT<Widget>())
                {
                    if (child->isVisible())
                    {
                        minimumSize = glm::max(minimumSize, child->getMinimumSize());
                    }
                }
                _setMinimumSize(minimumSize + getMargin().getSize(style));
            }
        }

        void StackLayout::_layoutEvent(LayoutEvent&)
        {
            if (auto style = _getStyle().lock())
            {
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                for (const auto& child : getChildrenT<Widget>())
                {
                    child->setGeometry(Widget::getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign()));
                }
            }
        }

    } // namespace UI
} // namespace djv

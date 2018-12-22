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

#include <djvUI/SoloLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct SoloLayout::Private
        {
            int currentIndex = -1;
        };

        void SoloLayout::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setClassName("djv::UI::SoloLayout");
        }
        
        SoloLayout::SoloLayout() :
            _p(new Private)
        {}

        SoloLayout::~SoloLayout()
        {}

        std::shared_ptr<SoloLayout> SoloLayout::create(Context * context)
        {
            auto out = std::shared_ptr<SoloLayout>(new SoloLayout);
            out->_init(context);
            return out;
        }

        int SoloLayout::getCurrentIndex() const
        {
            return _p->currentIndex;
        }

        void SoloLayout::setCurrentIndex(int value)
        {
            _p->currentIndex = value;
        }

        void SoloLayout::addWidget(const std::shared_ptr<Widget>& value)
        {
            IContainerWidget::addWidget(value);
            if (-1 == _p->currentIndex)
            {
                _p->currentIndex = 0;
            }
        }

        void SoloLayout::removeWidget(const std::shared_ptr<Widget>& value)
        {
            IContainerWidget::removeWidget(value);
            if (0 == getChildren().size())
            {
                _p->currentIndex = -1;
            }
        }

        float SoloLayout::getHeightForWidth(float value) const
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

        void SoloLayout::preLayoutEvent(Event::PreLayout&)
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

        void SoloLayout::layoutEvent(Event::Layout&)
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

        void SoloLayout::updateEvent(Event::Update& event)
        {
            int index = 0;
            for (const auto& child : getChildrenT<Widget>())
            {
                child->setVisible(_p->currentIndex == index);
                ++index;
            }
        }

    } // namespace UI
} // namespace djv

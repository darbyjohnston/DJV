// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/StackLayout.h>

#include <djvUI/LayoutUtil.h>

#include <glm/glm.hpp>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Stack::Private
            {};

            void Stack::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Stack");
            }

            Stack::Stack() :
                _p(new Private)
            {}

            Stack::~Stack()
            {}

            std::shared_ptr<Stack> Stack::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Stack>(new Stack);
                out->_init(context);
                return out;
            }

            float Stack::getHeightForWidth(float value) const
            {
                return heightForWidth(value, getChildWidgets(), getMargin(), _getStyle());
            }

            float Stack::heightForWidth(float value, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                float out = 0.F;
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
                glm::vec2 minimumSize = glm::vec2(0.F, 0.F);
                for (const auto& child : children)
                {
                    if (child->isVisible())
                    {
                        minimumSize = glm::max(minimumSize, child->getMinimumSize());
                    }
                }
                return minimumSize + margin.getSize(style);
            }

            void Stack::layout(const Math::BBox2f& geometry, const std::vector<std::shared_ptr<Widget> >& children, const Layout::Margin& margin, const std::shared_ptr<Style::Style>& style)
            {
                const Math::BBox2f& g = margin.bbox(geometry, style);
                for (const auto& child : children)
                {
                    const Math::BBox2f childGeometry = getAlign(g, child->getMinimumSize(), child->getHAlign(), child->getVAlign());
                    child->setGeometry(childGeometry);
                }
            }

            void Stack::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(minimumSize(getChildWidgets(), getMargin(), _getStyle()));
            }

            void Stack::_layoutEvent(System::Event::Layout&)
            {
                layout(getGeometry(), getChildWidgets(), getMargin(), _getStyle());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <vector>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides a layout that arranges it's children into a stack.
            class Stack : public Widget
            {
                DJV_NON_COPYABLE(Stack);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Stack();

            public:
                ~Stack() override;

                static std::shared_ptr<Stack> create(const std::shared_ptr<System::Context>&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };
            
            //! \name Stack Layout
            ///@{

            float stackHeightForWidth(
                    float,
                    const std::vector<std::shared_ptr<Widget> >&,
                    const Margin&,
                    const std::shared_ptr<Style::Style>&);

            glm::vec2 stackMinimumSize(
                const std::vector<std::shared_ptr<Widget> >&,
                const Margin&,
                const std::shared_ptr<Style::Style>&);

            void stackLayout(
                const Math::BBox2f&,
                const std::vector<std::shared_ptr<Widget> >&,
                const Margin&,
                const std::shared_ptr<Style::Style>&);

            ///@}

        } // namespace Layout

        typedef Layout::Stack StackLayout;

    } // namespace UI
} // namespace djv

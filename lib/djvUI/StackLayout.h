// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

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
                void _init(const std::shared_ptr<Core::Context>&);
                Stack();

            public:
                ~Stack() override;

                static std::shared_ptr<Stack> create(const std::shared_ptr<Core::Context>&);

                float getHeightForWidth(float) const override;

                static float heightForWidth(
                    float,
                    const std::vector<std::shared_ptr<Widget> >&,
                    const Layout::Margin&,
                    const std::shared_ptr<Style::Style>&);

                static glm::vec2 minimumSize(
                    const std::vector<std::shared_ptr<Widget> >&,
                    const Layout::Margin&,
                    const std::shared_ptr<Style::Style>&);

                static void layout(
                    const Core::BBox2f&,
                    const std::vector<std::shared_ptr<Widget> >&,
                    const Layout::Margin&,
                    const std::shared_ptr<Style::Style>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Stack StackLayout;

    } // namespace UI
} // namespace djv

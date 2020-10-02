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
            //! This class provides a widget that draws a border.
            class Border : public Widget
            {
                DJV_NON_COPYABLE(Border);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Border();

            public:
                ~Border() override;

                static std::shared_ptr<Border> create(const std::shared_ptr<System::Context>&);

                MetricsRole getBorderSize() const;
                ColorRole getBorderColorRole() const;

                void setBorderSize(MetricsRole);
                void setBorderColorRole(ColorRole);

                const Margin& getInsideMargin() const;

                void setInsideMargin(const Margin&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Border Border;

    } // namespace UI
} // namespace djv


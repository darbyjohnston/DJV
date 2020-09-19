// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Margin.h>
#include <djvUI/Spacing.h>
#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides a layout that arranges it's children similar to a text paragraph.
            class Flow : public Widget
            {
                DJV_NON_COPYABLE(Flow);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Flow();

            public:
                ~Flow() override;

                static std::shared_ptr<Flow> create(const std::shared_ptr<System::Context>&);

                const Spacing& getSpacing() const;
                void setSpacing(const Spacing&);

                float getHeightForWidth(float) const override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Flow FlowLayout;

    } // namespace UI
} // namespace djv

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
            //! This enumeration provides options for how the Solo layout
            //! calculates a minimum size from it's child widgets.
            enum class SoloMinimumSize
            {
                None,
                Horizontal,
                Vertical,
                Both
            };

            //! This class provides a layout that shows a single child at a time.
            class Solo : public Widget
            {
                DJV_NON_COPYABLE(Solo);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Solo();

            public:
                ~Solo() override;
                static std::shared_ptr<Solo> create(const std::shared_ptr<Core::Context>&);

                const std::shared_ptr<Widget>& getCurrentWidget() const;
                void setCurrentWidget(const std::shared_ptr<Widget>&);
                void setCurrentWidget(const std::shared_ptr<Widget>&, Side, bool animated = true);

                SoloMinimumSize getSoloMinimumSize() const;
                void setSoloMinimumSize(SoloMinimumSize);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Solo SoloLayout;

    } // namespace UI
} // namespace djv

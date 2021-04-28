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
            //! Solo layout minimum size.
            enum class SoloMinimumSize
            {
                None,
                Horizontal,
                Vertical,
                Both
            };

            //! Soo layout widget.
            //!
            //! This layout shows a single child at a time.
            class Solo : public Widget
            {
                DJV_NON_COPYABLE(Solo);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Solo();

            public:
                ~Solo() override;
                static std::shared_ptr<Solo> create(const std::shared_ptr<System::Context>&);

                const std::shared_ptr<Widget>& getCurrentWidget() const;

                void setCurrentWidget(const std::shared_ptr<Widget>&);
                void setCurrentWidget(const std::shared_ptr<Widget>&, Side);

                SoloMinimumSize getSoloMinimumSize() const;

                void setSoloMinimumSize(SoloMinimumSize);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                void _widgetUpdate();

                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::Solo SoloLayout;

    } // namespace UI
} // namespace djv

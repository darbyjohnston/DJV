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
            //! Group box widget.
            class GroupBox : public Widget
            {
                DJV_NON_COPYABLE(GroupBox);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                GroupBox();

            public:
                ~GroupBox() override;

                static std::shared_ptr<GroupBox> create(const std::shared_ptr<System::Context>&);

                const std::string& getText() const;

                void setText(const std::string&);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        typedef Layout::GroupBox GroupBox;

    } // namespace UI
} // namespace djv


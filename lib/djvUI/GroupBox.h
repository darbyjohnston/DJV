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
            //! This class provides a group box widget.
            class GroupBox : public Widget
            {
                DJV_NON_COPYABLE(GroupBox);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                GroupBox();

            public:
                virtual ~GroupBox();

                static std::shared_ptr<GroupBox> create(const std::shared_ptr<Core::Context>&);

                const std::string& getText() const;
                void setText(const std::string&);

                float getHeightForWidth(float) const override;

                void addChild(const std::shared_ptr<IObject>&) override;
                void removeChild(const std::shared_ptr<IObject>&) override;
                void clearChildren() override;

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout

        using Layout::GroupBox;

    } // namespace UI
} // namespace djv


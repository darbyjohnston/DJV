// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            //! This class provides a popup layout.
            class Popup : public Widget
            {
                DJV_NON_COPYABLE(Popup);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Popup();

            public:
                ~Popup() override;

                static std::shared_ptr<Popup> create(const std::shared_ptr<System::Context>&);

                void setButton(const std::weak_ptr<Widget>&);
                void setPopupDefault(UI::Popup);
                void clearPopups();

            protected:
                void _layoutEvent(System::Event::Layout&) override;
                void _paintEvent(System::Event::Paint&) override;

                void _childRemovedEvent(System::Event::ChildRemoved&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv


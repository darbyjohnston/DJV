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
            //! Separator widget.
            class Separator : public Widget
            {
                DJV_NON_COPYABLE(Separator);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Separator();

            public:
                ~Separator() override;

                static std::shared_ptr<Separator> create(const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

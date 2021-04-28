// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! Base class for tooltip widgets.
        class ITooltipWidget : public Widget
        {
            DJV_NON_COPYABLE(ITooltipWidget);

        protected:
            ITooltipWidget();

        public:
            ~ITooltipWidget() override = 0;

            //! Set the tooltip text.
            virtual void setTooltip(const std::string&) = 0;
        };

    } // namespace UI
} // namespace djv


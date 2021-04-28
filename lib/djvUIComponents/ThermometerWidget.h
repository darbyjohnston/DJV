// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UIComponents
    {
        //! Thermometer widget.
        class ThermometerWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(ThermometerWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ThermometerWidget();

        public:
            ~ThermometerWidget() override;

            static std::shared_ptr<ThermometerWidget> create(const std::shared_ptr<System::Context>&);

            //! \name Percentage
            ///@{

            float getPercentage() const;

            void setPercentage(float);

            ///@}

            //! \name Options
            ///@{

            UI::Orientation getOrientation() const;
            UI::ColorRole getColorRole() const;
            UI::MetricsRole getSizeRole() const;

            void setOrientation(UI::Orientation);
            void setColorRole(UI::ColorRole);
            void setSizeRole(UI::MetricsRole);

            ///@}

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv


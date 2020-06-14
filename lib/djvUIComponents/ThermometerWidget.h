// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvCore/Range.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a widget for displaying a value in a
        //! "thermometer" style.
        class ThermometerWidget : public Widget
        {
            DJV_NON_COPYABLE(ThermometerWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ThermometerWidget();

        public:
            ~ThermometerWidget() override;

            static std::shared_ptr<ThermometerWidget> create(const std::shared_ptr<Core::Context>&);

            Orientation getOrientation() const;
            void setOrientation(Orientation);

            float getPercentage() const;
            void setPercentage(float);

            ColorRole getColorRole() const;
            void setColorRole(ColorRole);

            MetricsRole getSizeRole() const;
            void setSizeRole(MetricsRole);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv


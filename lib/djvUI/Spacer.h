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
            //! Spacer widget.
            class Spacer : public Widget
            {
                DJV_NON_COPYABLE(Spacer);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                Spacer();

            public:
                ~Spacer() override;

                static std::shared_ptr<Spacer> create(Orientation, const std::shared_ptr<System::Context>&);

                Orientation getOrientation() const;

                void setOrientation(Orientation);

                MetricsRole getSpacerSize() const;
                MetricsRole getSpacerOppositeSize() const;

                void setSpacerSize(MetricsRole);
                void setSpacerOppositeSize(MetricsRole);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;

            private:
                DJV_PRIVATE();
            };

            //! Horizontal spacer widget.
            class HorizontalSpacer : public Spacer
            {
                DJV_NON_COPYABLE(HorizontalSpacer);

            protected:
                HorizontalSpacer();

            public:
                static std::shared_ptr<HorizontalSpacer> create(const std::shared_ptr<System::Context>&);
            };

            //! Vertical spacer widget.
            class VerticalSpacer : public Spacer
            {
                DJV_NON_COPYABLE(VerticalSpacer);

            protected:
                VerticalSpacer();

            public:
                static std::shared_ptr<VerticalSpacer> create(const std::shared_ptr<System::Context>&);
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

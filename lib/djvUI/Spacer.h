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
            //! This class provides a spacer for use in layouts.
            class Spacer : public Widget
            {
                DJV_NON_COPYABLE(Spacer);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                Spacer();

            public:
                ~Spacer() override;

                static std::shared_ptr<Spacer> create(Orientation, const std::shared_ptr<Core::Context>&);

                Orientation getOrientation() const;
                void setOrientation(Orientation);

                MetricsRole getSpacerSize() const;
                void setSpacerSize(MetricsRole);

                MetricsRole getSpacerOppositeSize() const;
                void setSpacerOppositeSize(MetricsRole);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides a horizontal spacer for use in layouts.
            class HorizontalSpacer : public Spacer
            {
                DJV_NON_COPYABLE(HorizontalSpacer);

            protected:
                HorizontalSpacer();

            public:
                static std::shared_ptr<HorizontalSpacer> create(const std::shared_ptr<Core::Context>&);
            };

            //! This class provides a vertical spacer for use in layouts.
            class VerticalSpacer : public Spacer
            {
                DJV_NON_COPYABLE(VerticalSpacer);

            protected:
                VerticalSpacer();

            public:
                static std::shared_ptr<VerticalSpacer> create(const std::shared_ptr<Core::Context>&);
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv

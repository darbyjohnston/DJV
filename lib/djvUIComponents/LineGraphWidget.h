// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvMath/Range.h>

namespace djv
{
    namespace UIComponents
    {
        //! Animated line graph widget.
        class LineGraphWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(LineGraphWidget);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            LineGraphWidget();

        public:
            ~LineGraphWidget() override;

            static std::shared_ptr<LineGraphWidget> create(const std::shared_ptr<System::Context>&);

            const Math::FloatRange& getSampleRange() const;

            void addSample(float);
            void resetSamples();
            void setPrecision(size_t);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;
            void _paintEvent(System::Event::Paint&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UIComponents
} // namespace djv


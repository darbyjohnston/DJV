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
        //! This class provides a widget for displaying an animated line graph.
        class LineGraphWidget : public Widget
        {
            DJV_NON_COPYABLE(LineGraphWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            LineGraphWidget();

        public:
            ~LineGraphWidget() override;

            static std::shared_ptr<LineGraphWidget> create(const std::shared_ptr<Core::Context>&);

            const Core::FloatRange& getSampleRange() const;
            void addSample(float);
            void resetSamples();
            void setPrecision(size_t);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout&) override;
            void _paintEvent(Core::Event::Paint&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv


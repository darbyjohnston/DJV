// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace Image
    {
        class Color;
        
    } // namespace Image
    
    namespace ViewApp
    {
        //! This class provides the annotation widget.
        class AnnotateWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(AnnotateWidget);

        protected:
            void _init(
                std::map<std::string, std::shared_ptr<UI::Action> >&,
                const std::shared_ptr<System::Context>&);
            AnnotateWidget();

        public:
            ~AnnotateWidget() override;

            static std::shared_ptr<AnnotateWidget> create(
                std::map<std::string, std::shared_ptr<UI::Action> >&,
                const std::shared_ptr<System::Context>&);

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


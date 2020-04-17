// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides a memory cache widget.
        class MemoryCacheWidget : public UI::Widget
        {
            DJV_NON_COPYABLE(MemoryCacheWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MemoryCacheWidget();

        public:
            ~MemoryCacheWidget() override;

            static std::shared_ptr<MemoryCacheWidget> create(const std::shared_ptr<Core::Context>&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;
            
            void _initEvent(Core::Event::Init &) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


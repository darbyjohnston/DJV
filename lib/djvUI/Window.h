// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

namespace djv
{
    namespace UI
    {
        //! This class provides a top-level window.
        class Window : public Widget
        {
            DJV_NON_COPYABLE(Window);
            
        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Window();

        public:
            ~Window() override;

            static std::shared_ptr<Window> create(const std::shared_ptr<System::Context>&);

            bool isClosed() const;

            //! Call this before destroying the window.
            void close();

        protected:
            void _preLayoutEvent(System::Event::PreLayout&) override;
            void _layoutEvent(System::Event::Layout&) override;

            void _initEvent(System::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv


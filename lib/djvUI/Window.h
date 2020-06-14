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
            void _init(const std::shared_ptr<Core::Context>&);
            Window();

        public:
            virtual ~Window();

            static std::shared_ptr<Window> create(const std::shared_ptr<Core::Context>&);

            bool isClosed() const;

            //! Call this before destroying the window.
            void close();

        protected:
            void _preLayoutEvent(Core::Event::PreLayout&) override;
            void _layoutEvent(Core::Event::Layout&) override;

            void _initEvent(Core::Event::Init&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv


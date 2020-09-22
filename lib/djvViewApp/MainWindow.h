// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the main window.
        class MainWindow : public UI::Window
        {
            DJV_NON_COPYABLE(MainWindow);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MainWindow();

        public:
            ~MainWindow() override;

            static std::shared_ptr<MainWindow> create(const std::shared_ptr<System::Context>&);

        protected:
            void _dropEvent(System::Event::Drop&) override;

            void _initEvent(System::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


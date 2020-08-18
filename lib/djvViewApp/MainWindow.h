// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;
    
        } // namespace FileLSystem
    } // namespce Core

    namespace ViewApp
    {
        class MediaCanvas;

        //! This class provides the main window.
        class MainWindow : public UI::Window
        {
            DJV_NON_COPYABLE(MainWindow);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            MainWindow();

        public:
            ~MainWindow() override;

            static std::shared_ptr<MainWindow> create(const std::shared_ptr<Core::Context>&);

            const std::shared_ptr<MediaCanvas>& getMediaCanvas() const;

        protected:
            void _dropEvent(Core::Event::Drop&) override;

            void _initEvent(Core::Event::Init &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


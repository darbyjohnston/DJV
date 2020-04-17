// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

#include <djvUI/Window.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the new user experience widget.
        class NUXWidget : public UI::Window
        {
            DJV_NON_COPYABLE(NUXWidget);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            NUXWidget();

        public:
            static std::shared_ptr<NUXWidget> create(const std::shared_ptr<Core::Context>&);

            void setFinishCallback(const std::function<void(void)>&);

        protected:
            void _initEvent(Core::Event::Init&) override;

        private:
            void _widgetUpdate();

            DJV_PRIVATE();
        };

        //! This class provides the new user experience system.
        class NUXSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(NUXSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            NUXSystem();

        public:
            static std::shared_ptr<NUXSystem> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<NUXWidget> createNUXWidget();

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


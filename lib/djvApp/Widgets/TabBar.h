// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/FilesModel.h>

#include <feather-tk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Tab bar.
        class TabBar : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(TabBar);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            TabBar();

        public:
            ~TabBar();

            static std::shared_ptr<TabBar> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/FilesModel.h>

#include <dtk/ui/ToolBar.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Tab bar.
        class TabBar : public dtk::IWidget
        {
            DTK_NON_COPYABLE(TabBar);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            TabBar();

        public:
            ~TabBar();

            static std::shared_ptr<TabBar> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        private:
            DTK_PRIVATE();
        };
    }
}

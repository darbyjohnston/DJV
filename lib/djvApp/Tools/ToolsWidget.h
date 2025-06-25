// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <feather-tk/ui/IWidget.h>

namespace djv
{
    namespace app
    {
        class App;
        class MainWindow;

        //! Tools widget.
        class ToolsWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(ToolsWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&,
                const std::shared_ptr<IWidget>& parent);

            ToolsWidget();

        public:
            virtual ~ToolsWidget();

            //! Create a new widget.
            static std::shared_ptr<ToolsWidget> create(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}

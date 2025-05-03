// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

#include <tlTimeline/Player.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Information tool.
        class InfoTool : public IToolWidget
        {
            DTK_NON_COPYABLE(InfoTool);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            InfoTool();

        public:
            virtual ~InfoTool();

            static std::shared_ptr<InfoTool> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            void _widgetUpdate();

            DTK_PRIVATE();
        };
    }
}

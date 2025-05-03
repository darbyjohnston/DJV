// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Tools/IToolWidget.h>

namespace djv
{
    namespace app
    {
        struct ExportSettings;

        class App;

        //! Export tool.
        class ExportTool : public IToolWidget
        {
            DTK_NON_COPYABLE(ExportTool);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent);

            ExportTool();

        public:
            virtual ~ExportTool();

            static std::shared_ptr<ExportTool> create(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

        private:
            void _widgetUpdate(const ExportSettings&);
            void _export();
            void _exportFrame();

            DTK_PRIVATE();
        };
    }
}

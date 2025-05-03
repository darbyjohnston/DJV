// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/ToolsModel.h>

#include <dtk/ui/Bellows.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Base class for tool widgets.
        class IToolWidget : public dtk::IWidget
        {
            DTK_NON_COPYABLE(IToolWidget);

        protected:
            void _init(
                const std::shared_ptr<dtk::Context>&,
                const std::shared_ptr<App>&,
                Tool,
                const std::string& objectName,
                const std::shared_ptr<IWidget>& parent);

            IToolWidget();

        public:
            virtual ~IToolWidget() = 0;

            void setGeometry(const dtk::Box2I&) override;
            void sizeHintEvent(const dtk::SizeHintEvent&) override;

        protected:
            void _loadSettings(const std::map<std::string, std::shared_ptr<dtk::Bellows> >&);
            void _saveSettings(const std::map<std::string, std::shared_ptr<dtk::Bellows> >&);

            void _setWidget(const std::shared_ptr<dtk::IWidget>&);

            std::weak_ptr<App> _app;

        private:
            DTK_PRIVATE();
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#pragma once

#include <djvApp/Models/ToolsModel.h>

#include <feather-tk/ui/Bellows.h>

namespace djv
{
    namespace app
    {
        class App;

        //! Base class for tool widgets.
        class IToolWidget : public feather_tk::IWidget
        {
            FEATHER_TK_NON_COPYABLE(IToolWidget);

        protected:
            void _init(
                const std::shared_ptr<feather_tk::Context>&,
                const std::shared_ptr<App>&,
                Tool,
                const std::string& objectName,
                const std::shared_ptr<IWidget>& parent);

            IToolWidget();

        public:
            virtual ~IToolWidget() = 0;

            void setGeometry(const feather_tk::Box2I&) override;
            void sizeHintEvent(const feather_tk::SizeHintEvent&) override;

        protected:
            void _loadSettings(const std::map<std::string, std::shared_ptr<feather_tk::Bellows> >&);
            void _saveSettings(const std::map<std::string, std::shared_ptr<feather_tk::Bellows> >&);

            void _setWidget(const std::shared_ptr<feather_tk::IWidget>&);

            std::weak_ptr<App> _app;

        private:
            FEATHER_TK_PRIVATE();
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/IViewAppSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the settings system.
        class SettingsSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(SettingsSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            SettingsSystem();

        public:
            ~SettingsSystem() override;

            static std::shared_ptr<SettingsSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<ActionData> getToolActionData() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > createSettingsWidgets() const override;

        protected:
            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the metadata information system.
        class InfoSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(InfoSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            InfoSystem();

        public:
            ~InfoSystem() override;

            static std::shared_ptr<InfoSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<ActionData> getToolActionData() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


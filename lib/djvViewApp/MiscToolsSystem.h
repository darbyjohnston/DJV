// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the miscellaneous tools system.
        class MiscToolsSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(MiscToolsSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MiscToolsSystem();

        public:
            ~MiscToolsSystem() override;

            static std::shared_ptr<MiscToolsSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<ActionData> getToolActionData() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            void deleteToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            void _messagesPopup();
            std::string _getMessagesString() const;

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


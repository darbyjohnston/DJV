// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the tool system.
        class ToolSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(ToolSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ToolSystem();

        public:
            ~ToolSystem() override;

            static std::shared_ptr<ToolSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _closeWidget(const std::string&) override;

            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            std::string _getMessagesString() const;
            void _messagesPopup();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


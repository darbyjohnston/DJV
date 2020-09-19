// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace Core
    {
        class ICommand;
    
    } // namespace Core

    namespace ViewApp
    {
        //! This class provides the edit system.
        class EditSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(EditSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            EditSystem();

        public:
            ~EditSystem() override;

            static std::shared_ptr<EditSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


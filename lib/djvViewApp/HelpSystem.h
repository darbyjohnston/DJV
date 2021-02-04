// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the help system.
        class HelpSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(HelpSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            HelpSystem();

        public:
            ~HelpSystem() override;

            static std::shared_ptr<HelpSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenuData() const override;

        protected:
            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


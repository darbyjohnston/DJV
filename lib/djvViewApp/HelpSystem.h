// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the help system.
        class HelpSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(HelpSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            HelpSystem();

        public:
            ~HelpSystem() override;

            static std::shared_ptr<HelpSystem> create(const std::shared_ptr<System::Context>&);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _closeWidget(const std::string&) override;

            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


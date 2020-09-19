// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IToolSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the magnify system.
        class MagnifySystem : public IToolSystem
        {
            DJV_NON_COPYABLE(MagnifySystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            MagnifySystem();

        public:
            ~MagnifySystem() override;

            static std::shared_ptr<MagnifySystem> create(const std::shared_ptr<System::Context>&);

            ToolActionData getToolAction() const override;
            void setCurrentTool(bool, int) override;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;

        protected:
            void _closeWidget(const std::string&) override;
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


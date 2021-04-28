// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewApp
    {
        //! Current tool.
        struct CurrentTool
        {
            std::shared_ptr<UI::Action> action;
            std::shared_ptr<IViewAppSystem> system;

            bool operator == (const CurrentTool&) const;
        };

        //! Tool system.
        class ToolSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(ToolSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ToolSystem();

        public:
            ~ToolSystem() override;

            static std::shared_ptr<ToolSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::Observer::IValueSubject<CurrentTool> > observeCurrentTool() const;

            int getToolIndex(const std::shared_ptr<UI::Action>&) const;

            std::shared_ptr<UI::Widget> createToolDrawer();

            int getSortKey() const override;
            std::vector<std::shared_ptr<UI::Menu> > getMenus() const override;
            std::vector<std::shared_ptr<UI::ToolBar> > createToolBars() const override;

        protected:
            void _textUpdate() override;

        private:
            void _setCurrentTool(int);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


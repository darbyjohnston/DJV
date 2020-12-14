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
        //! This structure provides the current tool data.
        struct CurrentTool
        {
            std::shared_ptr<IViewAppSystem> system;
            std::shared_ptr<UI::Action> action;

            bool operator == (const CurrentTool&) const;
        };

        //! This class provides the tool system.
        class ToolSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(ToolSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ToolSystem();

        public:
            ~ToolSystem() override;

            static std::shared_ptr<ToolSystem> create(const std::shared_ptr<System::Context>&);

            const std::vector<std::shared_ptr<IViewAppSystem> >& getToolSystems() const;
            const std::vector<std::shared_ptr<UI::Action> >& getToolActions() const;

            std::shared_ptr<Core::Observer::IValueSubject<CurrentTool> > observeCurrentTool() const;

            std::shared_ptr<UI::Widget> createToolDrawer();

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<MenuData> getMenuData() const override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            void _setCurrentTool(int);

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


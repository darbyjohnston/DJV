// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

#include <djvUI/Enum.h>

#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the view system.
        class ViewSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(ViewSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewSystem();

        public:
            ~ViewSystem() override;

            static std::shared_ptr<ViewSystem> create(const std::shared_ptr<System::Context>&);

            int getSortKey() const override;
            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<std::shared_ptr<UI::Menu> > getMenus() const override;
            std::vector<std::shared_ptr<UI::ToolBar> > createToolBars() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetActions() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetToolBarActions() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;
            void deleteToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            static float _getScrollWheelSpeed(UI::ScrollWheelSpeed);

            void _panImage(const glm::vec2&);
            void _zoomImage(float);
            void _zoomAction(float);

            void _actionsUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


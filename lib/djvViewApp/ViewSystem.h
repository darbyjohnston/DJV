// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/IToolSystem.h>

#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace ViewApp
    {
        //! This class provides the view system.
        class ViewSystem : public IToolSystem
        {
            DJV_NON_COPYABLE(ViewSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            ViewSystem();

        public:
            ~ViewSystem() override;

            static std::shared_ptr<ViewSystem> create(const std::shared_ptr<System::Context>&);

            ToolActionData getToolAction() const override;
            void setCurrentTool(bool, int) override;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _closeWidget(const std::string&) override;

            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            static float _getScrollWheelSpeed(ScrollWheelSpeed);

            void _panImage(const glm::vec2&);
            void _zoomImage(float);
            void _zoomAction(float);

            void _actionsUpdate();
            void _widgetUpdate();

            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


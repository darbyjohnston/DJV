// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>

#include <djvMath/BBox.h>

#include <functional>
#include <map>

namespace djv
{
    namespace UI
    {
        class Action;
        class Menu;
        class Widget;

        struct ShortcutDataPair;

    } // namespace UI

    namespace UIComponents
    {
        namespace Settings
        {
            class IWidget;

        } // namespace Settings
    } // namespace UI

    namespace ViewApp
    {
        //! This struct provides action data.
        struct ActionData
        {
            std::shared_ptr<UI::Action> action;
            std::string sortKey;
        };

        //! This struct provides menu data.
        struct MenuData
        {
            std::shared_ptr<UI::Menu> menu;
            std::string sortKey;
        };

        //! This struct provides tool widget data.
        struct ToolWidgetData
        {
            std::shared_ptr<UI::Widget> widget;
            std::shared_ptr<UI::Widget> footer;
        };

        //! This class provides the base functionality for systems.
        class IViewAppSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(IViewAppSystem);

        protected:
            void _init(const std::string& name, const std::shared_ptr<System::Context>&);
            IViewAppSystem();

        public:
            ~IViewAppSystem() override;

            virtual std::map<std::string, std::shared_ptr<UI::Action> > getActions() const;
            virtual std::vector<MenuData> getMenuData() const;

            virtual std::vector<ActionData> getToolActionData() const;
            virtual ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&);
            virtual void deleteToolWidget(const std::shared_ptr<UI::Action>&);

            virtual std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > createSettingsWidgets() const;

        protected:
            UI::ShortcutDataPair _getShortcuts(const std::string&) const;
            void _addShortcut(const std::string&, const UI::ShortcutDataPair&);
            void _addShortcut(const std::string&, int key);
            void _addShortcut(const std::string&, int key, int keyModifiers);

            virtual void _textUpdate();
            virtual void _shortcutsUpdate();

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


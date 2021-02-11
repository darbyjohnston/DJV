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
        class ToolBar;
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
        //! This struct provides tool widget data.
        class ToolWidgetData
        {
        public:
            ToolWidgetData();
            ToolWidgetData(
                const std::shared_ptr<UI::Widget>& titleBar,
                const std::shared_ptr<UI::Widget>& toolBar  = nullptr,
                const std::shared_ptr<UI::Widget>& widget   = nullptr,
                const std::shared_ptr<UI::Widget>& footer   = nullptr);

            std::shared_ptr<UI::Widget> titleBar;
            std::shared_ptr<UI::Widget> toolBar;
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

            //! Get the sort key.
            virtual int getSortKey() const = 0;

            //! Get the actions.
            virtual std::map<std::string, std::shared_ptr<UI::Action> > getActions() const;

            //! Get the menus.
            virtual std::vector<std::shared_ptr<UI::Menu> > getMenus() const;

            //! Create tool bars.
            virtual std::vector<std::shared_ptr<UI::ToolBar> > createToolBars() const;

            //! Get the tool widget actions.
            virtual std::vector<std::shared_ptr<UI::Action> > getToolWidgetActions() const;

            //! Get the tool widget tool bar actions.
            virtual std::vector<std::shared_ptr<UI::Action> > getToolWidgetToolBarActions() const;

            //! Create a tool widget.
            virtual ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&);

            //! Delete a tool widget.
            virtual void deleteToolWidget(const std::shared_ptr<UI::Action>&);

            //! Create settings widgets.
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


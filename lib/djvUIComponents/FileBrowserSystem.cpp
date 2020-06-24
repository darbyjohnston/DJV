// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserSystem.h>

#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutsSettings.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FileBrowserSystem::Private
        {};

        void FileBrowserSystem::_init(const std::shared_ptr<Context>& context)
        {
            ISystem::_init("djv::UI::FileBrowserSystem", context);

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto shortcutsSettings = settingsSystem->getSettingsT<UI::Settings::Shortcuts>();
            shortcutsSettings->addShortcut("file_browser_shortcut_back", GLFW_KEY_LEFT, Shortcut::getSystemModifier());
            shortcutsSettings->addShortcut("file_browser_shortcut_forward", GLFW_KEY_RIGHT, Shortcut::getSystemModifier());
            shortcutsSettings->addShortcut("file_browser_shortcut_up", GLFW_KEY_UP, Shortcut::getSystemModifier());
            shortcutsSettings->addShortcut("file_browser_shortcut_tiles", GLFW_KEY_T);
            shortcutsSettings->addShortcut("file_browser_shortcut_list", GLFW_KEY_L);
            shortcutsSettings->addShortcut("file_browser_shortcut_increase_thumbnail_size", GLFW_KEY_EQUAL);
            shortcutsSettings->addShortcut("file_browser_shortcut_decrease_thumbnail_size", GLFW_KEY_MINUS);
            shortcutsSettings->addShortcut("file_browser_shortcut_file_sequences", GLFW_KEY_S);
            shortcutsSettings->addShortcut("file_browser_shortcut_show_hidden", GLFW_KEY_N);
            shortcutsSettings->addShortcut("file_browser_shortcut_sort_by_name", GLFW_KEY_3);
            shortcutsSettings->addShortcut("file_browser_shortcut_sort_by_size", GLFW_KEY_4);
            shortcutsSettings->addShortcut("file_browser_shortcut_sort_by_time", GLFW_KEY_5);
            shortcutsSettings->addShortcut("file_browser_shortcut_reverse_sort", GLFW_KEY_R);
            shortcutsSettings->addShortcut("file_browser_shortcut_sort_directories_first", GLFW_KEY_D);
        }

        FileBrowserSystem::FileBrowserSystem() :
            _p(new Private)
        {}

        FileBrowserSystem::~FileBrowserSystem()
        {}

        std::shared_ptr<FileBrowserSystem> FileBrowserSystem::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FileBrowserSystem>(new FileBrowserSystem);
            out->_init(context);
            return out;
        }

    } // namespace UI
} // namespace djv


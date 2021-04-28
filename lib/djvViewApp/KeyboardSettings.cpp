// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/KeyboardSettings.h>

#include <djvUI/ShortcutData.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct KeyboardSettings::Private
        {
            std::shared_ptr<Observer::MapSubject<std::string, UI::ShortcutDataPair> > shortcuts;
        };

        void KeyboardSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::KeyboardSettings", context);

            DJV_PRIVATE_PTR();
            p.shortcuts = Observer::MapSubject<std::string, UI::ShortcutDataPair>::create();
            _load();
        }

        KeyboardSettings::KeyboardSettings() :
            _p(new Private)
        {}

        std::shared_ptr<KeyboardSettings> KeyboardSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<KeyboardSettings>(new KeyboardSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::MapSubject<std::string, UI::ShortcutDataPair > > KeyboardSettings::observeShortcuts() const
        {
            return _p->shortcuts;
        }

        void KeyboardSettings::setShortcuts(const UI::ShortcutDataMap& value)
        {
            _p->shortcuts->setIfChanged(value);
        }

        void KeyboardSettings::addShortcut(const std::string& name, const UI::ShortcutDataPair& shortcuts, bool overwrite)
        {
            DJV_PRIVATE_PTR();
            if (p.shortcuts->hasKey(name))
            {
                if (overwrite)
                {
                    p.shortcuts->setItem(name, shortcuts);
                }
            }
            else
            {
                p.shortcuts->setItem(name, shortcuts);
            }
        }

        void KeyboardSettings::addShortcut(const std::string& name, int key, bool overwrite)
        {
            addShortcut(name, { UI::ShortcutData(key) }, overwrite);
        }

        void KeyboardSettings::addShortcut(const std::string& name, int key, int keyModifiers, bool overwrite)
        {
            addShortcut(name, { UI::ShortcutData(key, keyModifiers) }, overwrite);
        }

        void KeyboardSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Shortcuts", value, p.shortcuts);
            }
        }

        rapidjson::Value KeyboardSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Shortcuts", p.shortcuts->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


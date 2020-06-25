// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSettings.h>

#include <djvUI/ShortcutData.h>

#include <djvCore/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct InputSettings::Private
        {
            std::shared_ptr<MapSubject<std::string, std::vector<UI::ShortcutData> > > shortcuts;
            std::shared_ptr<ValueSubject<ScrollWheelSpeed> > scrollWheelSpeed;
        };

        void InputSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::InputSettings", context);

            DJV_PRIVATE_PTR();
            p.shortcuts = MapSubject<std::string, std::vector<UI::ShortcutData> >::create();
            p.scrollWheelSpeed = ValueSubject<ScrollWheelSpeed>::create(ScrollWheelSpeed::Slow);
            _load();
        }

        InputSettings::InputSettings() :
            _p(new Private)
        {}

        std::shared_ptr<InputSettings> InputSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<InputSettings>(new InputSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<MapSubject<std::string, std::vector<UI::ShortcutData> > > InputSettings::observeShortcuts() const
        {
            return _p->shortcuts;
        }

        void InputSettings::setShortcuts(const UI::ShortcutDataMap& value)
        {
            _p->shortcuts->setIfChanged(value);
        }

        void InputSettings::addShortcut(const std::string& name, const std::vector<UI::ShortcutData>& shortcuts, bool overwrite)
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

        void InputSettings::addShortcut(const std::string& name, int key, bool overwrite)
        {
            addShortcut(name, { UI::ShortcutData(key) }, overwrite);
        }

        void InputSettings::addShortcut(const std::string& name, int key, int keyModifiers, bool overwrite)
        {
            addShortcut(name, { UI::ShortcutData(key, keyModifiers) }, overwrite);
        }

        std::shared_ptr<IValueSubject<ScrollWheelSpeed> > InputSettings::observeScrollWheelSpeed() const
        {
            return _p->scrollWheelSpeed;
        }

        void InputSettings::setScrollWheelSpeed(ScrollWheelSpeed value)
        {
            _p->scrollWheelSpeed->setIfChanged(value);
        }

        void InputSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Shortcuts", value, p.shortcuts);
                UI::Settings::read("ScrollWheelSpeed", value, p.scrollWheelSpeed);
            }
        }

        rapidjson::Value InputSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Shortcuts", p.shortcuts->get(), out, allocator);
            UI::Settings::write("ScrollWheelSpeed", p.scrollWheelSpeed->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


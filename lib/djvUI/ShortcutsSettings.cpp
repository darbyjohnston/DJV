// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/ShortcutsSettings.h>

#include <djvUI/Shortcut.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct Shortcuts::Private
            {
                std::shared_ptr<MapSubject<std::string, std::vector<ShortcutData> > > shortcuts;
            };

            void Shortcuts::_init(const std::shared_ptr<Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Shortcuts", context);
                DJV_PRIVATE_PTR();
                p.shortcuts = MapSubject<std::string, std::vector<ShortcutData> >::create();
                _load();
            }

            Shortcuts::Shortcuts() :
                _p(new Private)
            {}

            Shortcuts::~Shortcuts()
            {}

            std::shared_ptr<Shortcuts> Shortcuts::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Shortcuts>(new Shortcuts);
                out->_init(context);
                return out;
            }

            std::shared_ptr<MapSubject<std::string, std::vector<ShortcutData> > > Shortcuts::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void Shortcuts::setShortcuts(const std::map<std::string, std::vector<ShortcutData> >& value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            void Shortcuts::addShortcut(const std::string& name, const std::vector<ShortcutData>& shortcuts, bool overwrite)
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

            void Shortcuts::addShortcut(const std::string& name, int key, bool overwrite)
            {
                addShortcut(name, { ShortcutData(key) }, overwrite);
            }

            void Shortcuts::addShortcut(const std::string& name, int key, int keyModifiers, bool overwrite)
            {
                addShortcut(name, { ShortcutData(key, keyModifiers) }, overwrite);
            }

            void Shortcuts::load(const rapidjson::Value& value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    Settings::read("Shortcuts", value, p.shortcuts);
                }
            }

            rapidjson::Value Shortcuts::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                Settings::write("Shortcuts", p.shortcuts->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/UISettings.h>

#include <djvUI/Widget.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct UI::Private
            {
                std::shared_ptr<ValueSubject<bool> > tooltips;
            };

            void UI::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::UI", context);
                DJV_PRIVATE_PTR();
                p.tooltips = ValueSubject<bool>::create(true);
                _load();
            }

            UI::UI() :
                _p(new Private)
            {}

            UI::~UI()
            {}

            std::shared_ptr<UI> UI::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<UI>(new UI);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IValueSubject<bool> > UI::observeTooltips() const
            {
                return _p->tooltips;
            }

            void UI::setTooltips(bool value)
            {
                if (_p->tooltips->setIfChanged(value))
                {
                    Widget::setTooltipsEnabled(value);
                }
            }

            void UI::load(const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    read("Tooltips", object, p.tooltips);
                    Widget::setTooltipsEnabled(p.tooltips->get());
                }
            }

            picojson::value UI::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("Tooltips", p.tooltips->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


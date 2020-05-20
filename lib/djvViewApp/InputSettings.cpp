// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct InputSettings::Private
        {
            std::shared_ptr<ValueSubject<ScrollWheelSpeed> > scrollWheelSpeed;
        };

        void InputSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::InputSettings", context);

            DJV_PRIVATE_PTR();
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

        std::shared_ptr<IValueSubject<ScrollWheelSpeed> > InputSettings::observeScrollWheelSpeed() const
        {
            return _p->scrollWheelSpeed;
        }

        void InputSettings::setScrollWheelSpeed(ScrollWheelSpeed value)
        {
            _p->scrollWheelSpeed->setIfChanged(value);
        }

        void InputSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("ScrollWheelSpeed", object, p.scrollWheelSpeed);
            }
        }

        picojson::value InputSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("ScrollWheelSpeed", p.scrollWheelSpeed->get(), object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


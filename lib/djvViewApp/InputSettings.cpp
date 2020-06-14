// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/InputSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/RapidJSONTemplates.h>
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

        void InputSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("ScrollWheelSpeed", value, p.scrollWheelSpeed);
            }
        }

        rapidjson::Value InputSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("ScrollWheelSpeed", p.scrollWheelSpeed->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


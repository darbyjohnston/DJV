// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MouseSettings.h>

#include <djvViewApp/EnumFunc.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MouseSettings::Private
        {
            std::shared_ptr<ValueSubject<ScrollWheelSpeed> > scrollWheelSpeed;
        };

        void MouseSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MouseSettings", context);

            DJV_PRIVATE_PTR();
            p.scrollWheelSpeed = ValueSubject<ScrollWheelSpeed>::create(ScrollWheelSpeed::Slow);
            _load();
        }

        MouseSettings::MouseSettings() :
            _p(new Private)
        {}

        std::shared_ptr<MouseSettings> MouseSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MouseSettings>(new MouseSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<ScrollWheelSpeed> > MouseSettings::observeScrollWheelSpeed() const
        {
            return _p->scrollWheelSpeed;
        }

        void MouseSettings::setScrollWheelSpeed(ScrollWheelSpeed value)
        {
            _p->scrollWheelSpeed->setIfChanged(value);
        }

        void MouseSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("ScrollWheelSpeed", value, p.scrollWheelSpeed);
            }
        }

        rapidjson::Value MouseSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("ScrollWheelSpeed", p.scrollWheelSpeed->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


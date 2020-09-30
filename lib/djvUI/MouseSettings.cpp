// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/MouseSettings.h>

#include <djvUI/EnumFunc.h>

#include <djvSystem/Context.h>

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
            struct Mouse::Private
            {
                std::shared_ptr<Observer::ValueSubject<float> > doubleClickTime;
                std::shared_ptr<Observer::ValueSubject<ScrollWheelSpeed> > scrollWheelSpeed;
                std::shared_ptr<Observer::ValueSubject<bool> > reverseScrolling;
            };

            void Mouse::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Mouse", context);

                DJV_PRIVATE_PTR();
                p.doubleClickTime = Observer::ValueSubject<float>::create(.3F);
                p.scrollWheelSpeed = Observer::ValueSubject<ScrollWheelSpeed>::create(ScrollWheelSpeed::Slow);
                p.reverseScrolling = Observer::ValueSubject<bool>::create(false);
                _load();
            }

            Mouse::Mouse() :
                _p(new Private)
            {}

            std::shared_ptr<Mouse> Mouse::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Mouse>(new Mouse);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Observer::IValueSubject<float> > Mouse::observeDoubleClickTime() const
            {
                return _p->doubleClickTime;
            }

            void Mouse::setDoubleClickTime(float value)
            {
                _p->doubleClickTime->setIfChanged(value);
            }

            std::shared_ptr<Observer::IValueSubject<ScrollWheelSpeed> > Mouse::observeScrollWheelSpeed() const
            {
                return _p->scrollWheelSpeed;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > Mouse::observeReverseScrolling() const
            {
                return _p->reverseScrolling;
            }

            void Mouse::setScrollWheelSpeed(ScrollWheelSpeed value)
            {
                _p->scrollWheelSpeed->setIfChanged(value);
            }

            void Mouse::setReverseScrolling(bool value)
            {
                _p->reverseScrolling->setIfChanged(value);
            }

            void Mouse::load(const rapidjson::Value& value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    read("DoubleClickTime", value, p.doubleClickTime);
                    read("ScrollWheelSpeed", value, p.scrollWheelSpeed);
                    read("ReverseScrolling", value, p.reverseScrolling);
                }
            }

            rapidjson::Value Mouse::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("DoubleClickTime", p.doubleClickTime->get(), out, allocator);
                write("ScrollWheelSpeed", p.scrollWheelSpeed->get(), out, allocator);
                write("ReverseScrolling", p.reverseScrolling->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


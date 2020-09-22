// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/UISettings.h>

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
            struct UI::Private
            {
                std::shared_ptr<ValueSubject<bool> > tooltips;
                std::shared_ptr<ValueSubject<bool> > reverseScrolling;
            };

            void UI::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::UI", context);
                DJV_PRIVATE_PTR();
                p.tooltips = ValueSubject<bool>::create(true);
                p.reverseScrolling = ValueSubject<bool>::create(false);
                _load();
            }

            UI::UI() :
                _p(new Private)
            {}

            UI::~UI()
            {}

            std::shared_ptr<UI> UI::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<UI>(new UI);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IValueSubject<bool> > UI::observeTooltips() const
            {
                return _p->tooltips;
            }

            std::shared_ptr<IValueSubject<bool> > UI::observeReverseScrolling() const
            {
                return _p->reverseScrolling;
            }

            void UI::setTooltips(bool value)
            {
                _p->tooltips->setIfChanged(value);
            }

            void UI::setReverseScrolling(bool value)
            {
                _p->reverseScrolling->setIfChanged(value);
            }

            void UI::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    read("Tooltips", value, p.tooltips);
                    read("ReverseScrolling", value, p.reverseScrolling);
                }
            }

            rapidjson::Value UI::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("Tooltips", p.tooltips->get(), out, allocator);
                write("ReverseScrolling", p.reverseScrolling->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/IOSettings.h>

#include <djvSystem/Context.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct IO::Private
            {
                std::shared_ptr<Observer::ValueSubject<size_t> > threadCount;
            };

            void IO::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::IO", context);
                
                DJV_PRIVATE_PTR();
                p.threadCount = Observer::ValueSubject<size_t>::create(4);

                _load();
            }

            IO::IO() :
                _p(new Private)
            {}

            IO::~IO()
            {}

            std::shared_ptr<IO> IO::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<IO>(new IO);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Observer::IValueSubject<size_t> > IO::observeThreadCount() const
            {
                return _p->threadCount;
            }

            void IO::setThreadCount(size_t value)
            {
                DJV_PRIVATE_PTR();
                p.threadCount->setIfChanged(value);
            }

            void IO::load(const rapidjson::Value & value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    UI::Settings::read("ThreadCount", value, p.threadCount);
                }
            }

            rapidjson::Value IO::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                UI::Settings::write("ThreadCount", p.threadCount->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/IOSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct IO::Private
            {
                std::shared_ptr<ValueSubject<size_t> > threadCount;
            };

            void IO::_init(const std::shared_ptr<Context>& context)
            {
                ISettings::_init("djv::UI::Settings::IO", context);
                
                DJV_PRIVATE_PTR();
                p.threadCount = ValueSubject<size_t>::create(4);

                _load();
            }

            IO::IO() :
                _p(new Private)
            {}

            IO::~IO()
            {}

            std::shared_ptr<IO> IO::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<IO>(new IO);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IValueSubject<size_t> > IO::observeThreadCount() const
            {
                return _p->threadCount;
            }

            void IO::setThreadCount(size_t value)
            {
                DJV_PRIVATE_PTR();
                p.threadCount->setIfChanged(value);
            }

            void IO::load(const picojson::value & value)
            {
                if (value.is<picojson::object>())
                {
                    DJV_PRIVATE_PTR();
                    const auto & object = value.get<picojson::object>();
                    read("ThreadCount", object, p.threadCount);
                }
            }

            picojson::value IO::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("ThreadCount", p.threadCount->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


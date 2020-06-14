// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/NUXSettings.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct NUXSettings::Private
        {
            std::shared_ptr<ValueSubject<bool> > nux;
        };

        void NUXSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::NUXSettings", context);

            DJV_PRIVATE_PTR();
            p.nux = ValueSubject<bool>::create(true);
            _load();
        }

        NUXSettings::NUXSettings() :
            _p(new Private)
        {}

        std::shared_ptr<NUXSettings> NUXSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<NUXSettings>(new NUXSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<bool> > NUXSettings::observeNUX() const
        {
            return _p->nux;
        }

        void NUXSettings::setNUX(bool value)
        {
            _p->nux->setIfChanged(value);
        }

        void NUXSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("NUX", value, p.nux);
            }
        }

        rapidjson::Value NUXSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("NUX", p.nux->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


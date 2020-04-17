// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FontSettings.h>

#include <djvAV/FontSystem.h>

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
            struct Font::Private
            {
                std::shared_ptr<MapSubject<std::string, std::string> > localeFonts;
            };

            void Font::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::Font", context);

                DJV_PRIVATE_PTR();
                p.localeFonts = MapSubject<std::string, std::string>::create();
                p.localeFonts->setItem("Default", AV::Font::familyDefault);
                p.localeFonts->setItem("ja", "Noto Sans CJK JP");
                p.localeFonts->setItem("ko", "Noto Sans CJK KR");
                p.localeFonts->setItem("zh", "Noto Sans CJK SC");

                _load();
            }

            Font::Font() :
                _p(new Private)
            {}

            Font::~Font()
            {}

            std::shared_ptr<Font> Font::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<Font>(new Font);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<MapSubject<std::string, std::string> > & Font::observeLocaleFonts() const
            {
                return _p->localeFonts;
            }

            void Font::load(const picojson::value & value)
            {
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    read("LocaleFonts", object, _p->localeFonts);
                }
            }

            picojson::value Font::save()
            {
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("LocaleFonts", _p->localeFonts->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


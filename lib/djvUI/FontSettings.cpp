//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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

            void Font::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::Font", context);

                DJV_PRIVATE_PTR();
                p.localeFonts = MapSubject<std::string, std::string>::create();
                p.localeFonts->setItem("Default", AV::Font::Info::familyDefault);
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

            std::shared_ptr<Font> Font::create(Context * context)
            {
                auto out = std::shared_ptr<Font>(new Font);
                out->_init(context);
                return out;
            }

            const std::shared_ptr<MapSubject<std::string, std::string> >& Font::observeLocaleFonts() const
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


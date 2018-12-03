//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUICore/FontSettings.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UICore
    {
        struct FontSettings::Private
        {
            std::shared_ptr<MapSubject<std::string, FontMap> > fonts;
        };

        void FontSettings::_init(const std::shared_ptr<Context>& context)
        {
            ISettings::_init("djv::UICore::FontSettings", context);

            _p->fonts = MapSubject<std::string, FontMap>::create();
            _p->fonts->setItem(
                "Default",
                {
                    { FontFace::Regular, "Noto Sans" },
                    { FontFace::Bold, "Noto Sans" }
                });
            _p->fonts->setItem(
                "ja",
                {
                    { FontFace::Regular, "Noto Sans CJKjp" },
                    { FontFace::Bold, "Noto Sans CJKjp" }
                });
            _p->fonts->setItem(
                "ko",
                {
                    { FontFace::Regular, "Noto SansCJKkr" },
                    { FontFace::Bold, "Noto SansCJKkr" }
                });
            _p->fonts->setItem(
                "zh",
                {
                    { FontFace::Regular, "Noto Sans CJKsc" },
                    { FontFace::Bold, "Noto Sans CJKsc" }
                });

            _load();
        }

        FontSettings::FontSettings() :
            _p(new Private)
        {}

        FontSettings::~FontSettings()
        {}

        std::shared_ptr<FontSettings> FontSettings::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FontSettings>(new FontSettings);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<MapSubject<std::string, FontMap> >& FontSettings::getFonts() const
        {
            return _p->fonts;
        }

        void FontSettings::load(const picojson::value& value)
        {
            if (value.is<picojson::object>())
            {
                std::map<std::string, FontMap> fonts;
                const auto& object = value.get<picojson::object>();
                for (const auto& i : object)
                {
                    FontMap v;
                    _read(i.first, object, v);
                    fonts[i.first] = v;
                }
                _p->fonts->setIfChanged(fonts);
            }
        }

        picojson::value FontSettings::save()
        {
            picojson::value out(picojson::object_type, true);
            auto& object = out.get<picojson::object>();
            for (const auto& i : _p->fonts->get())
            {
                _write(i.first, i.second, object);
            }
            return out;
        }

    } // namespace UICore
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        UICore,
        FontFace,
        DJV_TEXT("Regular"),
        DJV_TEXT("Bold"));

} // namespace djv


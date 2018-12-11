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

#include <djvUI/GeneralSettings.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct GeneralSettings::Private
        {
            std::weak_ptr<TextSystem> textSystem;
            std::shared_ptr<ValueSubject<std::string> > currentLocale;
        };

        void GeneralSettings::_init(const std::shared_ptr<Context>& context)
        {
            ISettings::_init("Gp::UI::GeneralSettings", context);

            auto textSystem = context->getSystemT<Core::TextSystem>();
            _p->textSystem = textSystem;
            _p->currentLocale = ValueSubject<std::string>::create(textSystem->getCurrentLocale());

            _load();
        }

        GeneralSettings::GeneralSettings() :
            _p(new Private)
        {}

        GeneralSettings::~GeneralSettings()
        {}

        std::shared_ptr<GeneralSettings> GeneralSettings::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<GeneralSettings>(new GeneralSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<std::string> > GeneralSettings::getCurrentLocale() const
        {
            return _p->currentLocale;
        }

        void GeneralSettings::setCurrentLocale(const std::string& value)
        {
            if (_p->currentLocale->setIfChanged(value))
            {
                if (auto textSystem = _p->textSystem.lock())
                {
                    textSystem->setCurrentLocale(value);
                }
            }
        }

        void GeneralSettings::load(const picojson::value& value)
        {
            if (value.is<picojson::object>())
            {
                const auto& object = value.get<picojson::object>();
                _read("CurrentLocale", object, _p->currentLocale);
            }
        }

        picojson::value GeneralSettings::save()
        {
            picojson::value out(picojson::object_type, true);
            auto& object = out.get<picojson::object>();
            _write("CurrentLocale", _p->currentLocale->get(), object);
            return out;
        }

    } // namespace UI
} // namespace djv


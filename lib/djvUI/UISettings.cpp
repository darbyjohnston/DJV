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

#include <djvUI/UISettings.h>

#include <djvUI/Widget.h>

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
            struct UI::Private
            {
                std::shared_ptr<ValueSubject<bool> > tooltips;
            };

            void UI::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::UI", context);
                DJV_PRIVATE_PTR();
                p.tooltips = ValueSubject<bool>::create(true);
                _load();
            }

            UI::UI() :
                _p(new Private)
            {}

            UI::~UI()
            {}

            std::shared_ptr<UI> UI::create(Context * context)
            {
                auto out = std::shared_ptr<UI>(new UI);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IValueSubject<bool> > UI::observeTooltips() const
            {
                return _p->tooltips;
            }

            void UI::setTooltips(bool value)
            {
                if (_p->tooltips->setIfChanged(value))
                {
                    Widget::setTooltipsEnabled(value);
                }
            }

            void UI::load(const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    read("Tooltips", object, p.tooltips);
                    Widget::setTooltipsEnabled(p.tooltips->get());
                }
            }

            picojson::value UI::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("Tooltips", p.tooltips->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


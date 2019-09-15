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

#include <djvViewApp/HelpSettings.h>

#include <djvCore/Vector.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct HelpSettings::Private
        {
            std::shared_ptr<ValueSubject<bool> > errorsPopup;
            std::map<std::string, bool> debugBellowsState;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void HelpSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::HelpSettings", context);
            DJV_PRIVATE_PTR();
            p.errorsPopup = ValueSubject<bool>::create(true);
            _load();
        }

        HelpSettings::HelpSettings() :
            _p(new Private)
        {}

        HelpSettings::~HelpSettings()
        {}

        std::shared_ptr<HelpSettings> HelpSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<HelpSettings>(new HelpSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<IValueSubject<bool> > HelpSettings::observeErrorsPopup() const
        {
            return _p->errorsPopup;
        }

        void HelpSettings::setErrorsPopup(bool value)
        {
            _p->errorsPopup->setIfChanged(value);
        }

        std::map<std::string, bool> HelpSettings::getDebugBellowsState() const
        {
            return _p->debugBellowsState;
        }

        void HelpSettings::setDebugBellowsState(const std::map<std::string, bool>& value)
        {
            _p->debugBellowsState = value;
        }

        const std::map<std::string, BBox2f>& HelpSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void HelpSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void HelpSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("ErrorsPopup", object, p.errorsPopup);
                UI::Settings::read("DebugBellowsState", object, p.debugBellowsState);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value HelpSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("ErrorsPopup", p.errorsPopup->get(), object);
            UI::Settings::write("DebugBellowsState", p.debugBellowsState, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


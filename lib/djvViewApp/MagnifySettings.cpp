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

#include <djvViewApp/MagnifySettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MagnifySettings::Private
        {
            int magnify = 1;
            glm::vec2 magnifyPos = glm::vec2(0.F, 0.F);
            std::map<std::string, BBox2f> widgetGeom;
        };

        void MagnifySettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::MagnifySettings", context);
            DJV_PRIVATE_PTR();
            _load();
        }

        MagnifySettings::MagnifySettings() :
            _p(new Private)
        {}

        MagnifySettings::~MagnifySettings()
        {}

        int MagnifySettings::getMagnify() const
        {
            return _p->magnify;
        }

        void MagnifySettings::setMagnify(int value)
        {
            _p->magnify = value;
        }

        const glm::vec2& MagnifySettings::getMagnifyPos() const
        {
            return _p->magnifyPos;
        }

        void MagnifySettings::setMagnifyPos(const glm::vec2& value)
        {
            _p->magnifyPos = value;
        }

        std::shared_ptr<MagnifySettings> MagnifySettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MagnifySettings>(new MagnifySettings);
            out->_init(context);
            return out;
        }

        const std::map<std::string, BBox2f>& MagnifySettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void MagnifySettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void MagnifySettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("Magnify", object, p.magnify);
                UI::Settings::read("MagnifyPos", object, p.magnifyPos);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value MagnifySettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("Magnify", p.magnify, object);
            UI::Settings::write("MagnifyPos", p.magnifyPos, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


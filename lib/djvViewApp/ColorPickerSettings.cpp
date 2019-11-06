//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvViewApp/ColorPickerSettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSettings::Private
        {
            int sampleSize = 1;
            AV::Image::Type typeLock = AV::Image::Type::None;
            glm::vec2 pickerPos = glm::vec2(0.F, 0.F);
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ColorPickerSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ColorPickerSettings", context);
            _load();
        }

        ColorPickerSettings::ColorPickerSettings() :
            _p(new Private)
        {}

        ColorPickerSettings::~ColorPickerSettings()
        {}

        std::shared_ptr<ColorPickerSettings> ColorPickerSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSettings>(new ColorPickerSettings);
            out->_init(context);
            return out;
        }

        int ColorPickerSettings::getSampleSize() const
        {
            return _p->sampleSize;
        }

        AV::Image::Type ColorPickerSettings::getTypeLock() const
        {
            return _p->typeLock;
        }

        const glm::vec2& ColorPickerSettings::getPickerPos() const
        {
            return _p->pickerPos;
        }

        void ColorPickerSettings::setSampleSize(int value)
        {
            _p->sampleSize = value;
        }

        void ColorPickerSettings::setTypeLock(AV::Image::Type value)
        {
            _p->typeLock = value;
        }

        void ColorPickerSettings::setPickerPos(const glm::vec2& value)
        {
            _p->pickerPos = value;
        }

        const std::map<std::string, BBox2f>& ColorPickerSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ColorPickerSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ColorPickerSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("sampleSize", object, p.sampleSize);
                UI::Settings::read("typeLock", object, p.typeLock);
                UI::Settings::read("pickerPos", object, p.pickerPos);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ColorPickerSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("sampleSize", p.sampleSize, object);
            UI::Settings::write("typeLock", p.typeLock, object);
            UI::Settings::write("pickerPos", p.pickerPos, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


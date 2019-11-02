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

#include <djvViewApp/ToolSettings.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolSettings::Private
        {
            int colorPickerSampleSize = 1;
            AV::Image::Type colorPickerTypeLock = AV::Image::Type::None;
            glm::vec2 colorPickerPos = glm::vec2(0.F, 0.F);

            int magnify = 1;
            glm::vec2 magnifyPos = glm::vec2(0.F, 0.F);

            std::shared_ptr<ValueSubject<bool> > errorsPopup;

            std::map<std::string, bool> debugBellowsState;

            std::map<std::string, BBox2f> widgetGeom;
        };

        void ToolSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ToolSettings", context);
            DJV_PRIVATE_PTR();
            p.errorsPopup = ValueSubject<bool>::create(true);
            _load();
        }

        ToolSettings::ToolSettings() :
            _p(new Private)
        {}

        ToolSettings::~ToolSettings()
        {}

        std::shared_ptr<ToolSettings> ToolSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ToolSettings>(new ToolSettings);
            out->_init(context);
            return out;
        }

        int ToolSettings::getColorPickerSampleSize() const
        {
            return _p->colorPickerSampleSize;
        }

        AV::Image::Type ToolSettings::getColorPickerTypeLock() const
        {
            return _p->colorPickerTypeLock;
        }

        const glm::vec2& ToolSettings::getColorPickerPos() const
        {
            return _p->colorPickerPos;
        }

        void ToolSettings::setColorPickerSampleSize(int value)
        {
            _p->colorPickerSampleSize = value;
        }

        void ToolSettings::setColorPickerTypeLock(AV::Image::Type value)
        {
            _p->colorPickerTypeLock = value;
        }

        void ToolSettings::setColorPickerPos(const glm::vec2& value)
        {
            _p->colorPickerPos = value;
        }

        int ToolSettings::getMagnify() const
        {
            return _p->magnify;
        }

        void ToolSettings::setMagnify(int value)
        {
            _p->magnify = value;
        }

        const glm::vec2& ToolSettings::getMagnifyPos() const
        {
            return _p->magnifyPos;
        }

        void ToolSettings::setMagnifyPos(const glm::vec2& value)
        {
            _p->magnifyPos = value;
        }

        std::shared_ptr<IValueSubject<bool> > ToolSettings::observeErrorsPopup() const
        {
            return _p->errorsPopup;
        }

        void ToolSettings::setErrorsPopup(bool value)
        {
            _p->errorsPopup->setIfChanged(value);
        }

        std::map<std::string, bool> ToolSettings::getDebugBellowsState() const
        {
            return _p->debugBellowsState;
        }

        void ToolSettings::setDebugBellowsState(const std::map<std::string, bool>& value)
        {
            _p->debugBellowsState = value;
        }

        const std::map<std::string, BBox2f>& ToolSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ToolSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ToolSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("ColorPickerSampleSize", object, p.colorPickerSampleSize);
                UI::Settings::read("ColorPickerTypeLock", object, p.colorPickerTypeLock);
                UI::Settings::read("ColorPickerPos", object, p.colorPickerPos);
                UI::Settings::read("Magnify", object, p.magnify);
                UI::Settings::read("MagnifyPos", object, p.magnifyPos);
                UI::Settings::read("ErrorsPopup", object, p.errorsPopup);
                UI::Settings::read("DebugBellowsState", object, p.debugBellowsState);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ToolSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("ColorPickerSampleSize", p.colorPickerSampleSize, object);
            UI::Settings::write("ColorPickerTypeLock", p.colorPickerTypeLock, object);
            UI::Settings::write("ColorPickerPos", p.colorPickerPos, object);
            UI::Settings::write("Magnify", p.magnify, object);
            UI::Settings::write("MagnifyPos", p.magnifyPos, object);
            UI::Settings::write("ErrorsPopup", p.errorsPopup->get(), object);
            UI::Settings::write("DebugBellowsState", p.debugBellowsState, object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


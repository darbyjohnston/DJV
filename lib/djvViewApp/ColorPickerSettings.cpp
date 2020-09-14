// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerSettings.h>

#include <djvCore/BBoxFunc.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSettings::Private
        {
            size_t sampleSize = 1;
            AV::Image::Type lockType = AV::Image::Type::None;
            bool applyColorOperations = true;
            bool applyColorSpace = true;
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

        size_t ColorPickerSettings::getSampleSize() const
        {
            return _p->sampleSize;
        }

        void ColorPickerSettings::setSampleSize(size_t value)
        {
            _p->sampleSize = value;
        }

        AV::Image::Type ColorPickerSettings::getLockType() const
        {
            return _p->lockType;
        }

        void ColorPickerSettings::setLockType(AV::Image::Type value)
        {
            _p->lockType = value;
        }

        bool ColorPickerSettings::getApplyColorOperations() const
        {
            return _p->applyColorOperations;
        }

        void ColorPickerSettings::setApplyColorOperations(bool value)
        {
            _p->applyColorOperations = value;
        }

        bool ColorPickerSettings::getApplyColorSpace() const
        {
            return _p->applyColorSpace;
        }

        void ColorPickerSettings::setApplyColorSpace(bool value)
        {
            _p->applyColorSpace = value;
        }

        const glm::vec2& ColorPickerSettings::getPickerPos() const
        {
            return _p->pickerPos;
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

        void ColorPickerSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("sampleSize", value, p.sampleSize);
                UI::Settings::read("lockType", value, p.lockType);
                UI::Settings::read("applyColorOperations", value, p.applyColorOperations);
                UI::Settings::read("applyColorSpace", value, p.applyColorSpace);
                UI::Settings::read("pickerPos", value, p.pickerPos);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value ColorPickerSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("sampleSize", p.sampleSize, out, allocator);
            UI::Settings::write("lockType", p.lockType, out, allocator);
            UI::Settings::write("applyColorOperations", p.applyColorOperations, out, allocator);
            UI::Settings::write("applyColorSpace", p.applyColorSpace, out, allocator);
            UI::Settings::write("pickerPos", p.pickerPos, out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


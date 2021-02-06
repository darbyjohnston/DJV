// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSettings.h>

#include <djvViewApp/AnnotateFunc.h>

#include <djvUI/EnumFunc.h>

#include <djvImage/ColorFunc.h>

#include <djvMath/BBoxFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSettings::Private
        {
            std::shared_ptr<Observer::ValueSubject<AnnotateTool> > tool;
            std::shared_ptr<Observer::ValueSubject<AnnotateLineSize> > lineSize;
            std::shared_ptr<Observer::ValueSubject<Image::Color> > color;
        };

        void AnnotateSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::AnnotateSettings", context);
            DJV_PRIVATE_PTR();
            p.tool = Observer::ValueSubject<AnnotateTool>::create(AnnotateTool::Freehand);
            p.lineSize = Observer::ValueSubject<AnnotateLineSize>::create(AnnotateLineSize::Medium);
            p.color = Observer::ValueSubject<Image::Color>::create(Image::Color(1.F, 0.F, 0.f));
            _load();
        }

        AnnotateSettings::AnnotateSettings() :
            _p(new Private)
        {}

        AnnotateSettings::~AnnotateSettings()
        {}

        std::shared_ptr<AnnotateSettings> AnnotateSettings::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSettings>(new AnnotateSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<AnnotateTool> > AnnotateSettings::observeTool() const
        {
            return _p->tool;
        }

        void AnnotateSettings::setTool(AnnotateTool value)
        {
            _p->tool->setIfChanged(value);
        }


        std::shared_ptr<Observer::IValueSubject<AnnotateLineSize> > AnnotateSettings::observeLineSize() const
        {
            return _p->lineSize;
        }

        void AnnotateSettings::setLineSize(AnnotateLineSize value)
        {
            _p->lineSize->setIfChanged(value);
        }

        std::shared_ptr<Observer::IValueSubject<Image::Color> > AnnotateSettings::observeColor() const
        {
            return _p->color;
        }

        void AnnotateSettings::setColor(const Image::Color& value)
        {
            _p->color->setIfChanged(value);
        }

        void AnnotateSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Tool", value, p.tool);
                UI::Settings::read("LineSize", value, p.lineSize);
                UI::Settings::read("Color", value, p.color);
            }
        }

        rapidjson::Value AnnotateSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Tool", p.tool->get(), out, allocator);
            UI::Settings::write("LineSize", p.lineSize->get(), out, allocator);
            UI::Settings::write("Color", p.color->get(), out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


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
            std::shared_ptr<Observer::ListSubject<Image::Color> > colors;
            std::shared_ptr<Observer::ValueSubject<int> > currentColor;
            std::map<std::string, Math::BBox2f> widgetGeom;
        };

        void AnnotateSettings::_init(const std::shared_ptr<System::Context>& context)
        {
            ISettings::_init("djv::ViewApp::AnnotateSettings", context);
            DJV_PRIVATE_PTR();
            p.tool = Observer::ValueSubject<AnnotateTool>::create(AnnotateTool::Polyline);
            p.lineSize = Observer::ValueSubject<AnnotateLineSize>::create(AnnotateLineSize::Medium);
            p.colors = Observer::ListSubject<Image::Color>::create(
                {
                    Image::Color(1.F, 0.F, 0.F),
                    Image::Color(0.F, 1.F, 0.F),
                    Image::Color(0.F, 0.F, 1.F),
                    Image::Color(1.F, 1.F, 1.F),
                    Image::Color(0.F, 0.F, 0.F)
                });
            p.currentColor = Observer::ValueSubject<int>::create(0);
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

        std::shared_ptr<Core::Observer::IValueSubject<AnnotateTool> > AnnotateSettings::observeTool() const
        {
            return _p->tool;
        }

        void AnnotateSettings::setTool(AnnotateTool value)
        {
            _p->tool->setIfChanged(value);
        }


        std::shared_ptr<Core::Observer::IValueSubject<AnnotateLineSize> > AnnotateSettings::observeLineSize() const
        {
            return _p->lineSize;
        }

        void AnnotateSettings::setLineSize(AnnotateLineSize value)
        {
            _p->lineSize->setIfChanged(value);
        }

        std::shared_ptr<Core::Observer::IListSubject<Image::Color> > AnnotateSettings::observeColors() const
        {
            return _p->colors;
        }

        void AnnotateSettings::setColors(const std::vector<Image::Color>& value)
        {
            _p->colors->setIfChanged(value);
        }

        std::shared_ptr<Core::Observer::IValueSubject<int> > AnnotateSettings::observeCurrentColor() const
        {
            return _p->currentColor;
        }

        void AnnotateSettings::setCurrentColor(int value)
        {
            _p->currentColor->setIfChanged(value);
        }

        const std::map<std::string, Math::BBox2f>& AnnotateSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void AnnotateSettings::setWidgetGeom(const std::map<std::string, Math::BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void AnnotateSettings::load(const rapidjson::Value & value)
        {
            if (value.IsObject())
            {
                DJV_PRIVATE_PTR();
                UI::Settings::read("Tool", value, p.tool);
                UI::Settings::read("LineSize", value, p.lineSize);
                UI::Settings::read("Colors", value, p.colors);
                UI::Settings::read("CurrentColor", value, p.currentColor);
                UI::Settings::read("WidgetGeom", value, p.widgetGeom);
            }
        }

        rapidjson::Value AnnotateSettings::save(rapidjson::Document::AllocatorType& allocator)
        {
            DJV_PRIVATE_PTR();
            rapidjson::Value out(rapidjson::kObjectType);
            UI::Settings::write("Tool", p.tool->get(), out, allocator);
            UI::Settings::write("LineSize", p.lineSize->get(), out, allocator);
            UI::Settings::write("Colors", p.colors->get(), out, allocator);
            UI::Settings::write("CurrentColor", p.currentColor->get(), out, allocator);
            UI::Settings::write("WidgetGeom", p.widgetGeom, out, allocator);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


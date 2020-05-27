// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/AnnotateSettings.h>

#include <djvAV/Color.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct AnnotateSettings::Private
        {
            std::shared_ptr<ValueSubject<AnnotateTool> > tool;
            std::shared_ptr<ValueSubject<AnnotateLineSize> > lineSize;
            std::shared_ptr<ListSubject<AV::Image::Color> > colors;
            std::shared_ptr<ValueSubject<int> > currentColor;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void AnnotateSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::AnnotateSettings", context);
            DJV_PRIVATE_PTR();
            p.tool = ValueSubject<AnnotateTool>::create(AnnotateTool::Polyline);
            p.lineSize = ValueSubject<AnnotateLineSize>::create(AnnotateLineSize::Medium);
            p.colors = ListSubject<AV::Image::Color>::create(
                {
                    AV::Image::Color(1.F, 0.F, 0.F),
                    AV::Image::Color(0.F, 1.F, 0.F),
                    AV::Image::Color(0.F, 0.F, 1.F),
                    AV::Image::Color(1.F, 1.F, 1.F),
                    AV::Image::Color(0.F, 0.F, 0.F)
                });
            p.currentColor = ValueSubject<int>::create(0);
            _load();
        }

        AnnotateSettings::AnnotateSettings() :
            _p(new Private)
        {}

        AnnotateSettings::~AnnotateSettings()
        {}

        std::shared_ptr<AnnotateSettings> AnnotateSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<AnnotateSettings>(new AnnotateSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<AnnotateTool> > AnnotateSettings::observeTool() const
        {
            return _p->tool;
        }

        void AnnotateSettings::setTool(AnnotateTool value)
        {
            _p->tool->setIfChanged(value);
        }


        std::shared_ptr<Core::IValueSubject<AnnotateLineSize> > AnnotateSettings::observeLineSize() const
        {
            return _p->lineSize;
        }

        void AnnotateSettings::setLineSize(AnnotateLineSize value)
        {
            _p->lineSize->setIfChanged(value);
        }

        std::shared_ptr<Core::IListSubject<AV::Image::Color> > AnnotateSettings::observeColors() const
        {
            return _p->colors;
        }

        void AnnotateSettings::setColors(const std::vector<AV::Image::Color>& value)
        {
            _p->colors->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<int> > AnnotateSettings::observeCurrentColor() const
        {
            return _p->currentColor;
        }

        void AnnotateSettings::setCurrentColor(int value)
        {
            _p->currentColor->setIfChanged(value);
        }

        const std::map<std::string, BBox2f>& AnnotateSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void AnnotateSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void AnnotateSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("Tool", object, p.tool);
                UI::Settings::read("LineSize", object, p.lineSize);
                UI::Settings::read("Colors", object, p.colors);
                UI::Settings::read("CurrentColor", object, p.currentColor);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value AnnotateSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("Tool", p.tool->get(), object);
            UI::Settings::write("LineSize", p.lineSize->get(), object);
            UI::Settings::write("Colors", p.colors->get(), object);
            UI::Settings::write("CurrentColor", p.currentColor->get(), object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


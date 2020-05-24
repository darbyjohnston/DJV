// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewSettings.h>

#include <djvAV/Color.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewSettings::Private
        {
            std::map<std::string, bool> bellowsState;
            std::shared_ptr<ValueSubject<ImageViewLock> > lock;
            std::shared_ptr<ValueSubject<GridOptions> > gridOptions;
            std::shared_ptr<ValueSubject<HUDOptions> > hudOptions;
            std::shared_ptr<ValueSubject<AV::Image::Color> > backgroundColor;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ViewSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ViewSettings", context);

            DJV_PRIVATE_PTR();
            p.lock = ValueSubject<ImageViewLock>::create(ImageViewLock::Fill);
            p.gridOptions = ValueSubject<GridOptions>::create();
            p.hudOptions = ValueSubject<HUDOptions>::create();
            p.backgroundColor = ValueSubject<AV::Image::Color>::create(AV::Image::Color(0.F, 0.F, 0.F));
            _load();
        }

        ViewSettings::ViewSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ViewSettings> ViewSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ViewSettings>(new ViewSettings);
            out->_init(context);
            return out;
        }

        const std::map<std::string, bool> ViewSettings::getBellowsState() const
        {
            return _p->bellowsState;
        }

        void ViewSettings::setBellowsState(const std::map<std::string, bool>& value)
        {
            _p->bellowsState = value;
        }

        std::shared_ptr<IValueSubject<ImageViewLock> > ViewSettings::observeLock() const
        {
            return _p->lock;
        }

        void ViewSettings::setLock(ImageViewLock value)
        {
            _p->lock->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<GridOptions> > ViewSettings::observeGridOptions() const
        {
            return _p->gridOptions;
        }

        void ViewSettings::setGridOptions(const GridOptions& value)
        {
            _p->gridOptions->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<HUDOptions> > ViewSettings::observeHUDOptions() const
        {
            return _p->hudOptions;
        }

        void ViewSettings::setHUDOptions(const HUDOptions& value)
        {
            _p->hudOptions->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<AV::Image::Color> > ViewSettings::observeBackgroundColor() const
        {
            return _p->backgroundColor;
        }

        void ViewSettings::setBackgroundColor(const AV::Image::Color& value)
        {
            _p->backgroundColor->setIfChanged(value);
        }

        const std::map<std::string, BBox2f>& ViewSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ViewSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ViewSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("BellowsState", object, p.bellowsState);
                UI::Settings::read("Lock", object, p.lock);
                UI::Settings::read("GridOptions", object, p.gridOptions);
                UI::Settings::read("HUDOptions", object, p.hudOptions);
                UI::Settings::read("BackgroundColor", object, p.backgroundColor);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ViewSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("BellowsState", p.bellowsState, object);
            UI::Settings::write("Lock", p.lock->get(), object);
            UI::Settings::write("GridOptions", p.gridOptions->get(), object);
            UI::Settings::write("HUDOptions", p.hudOptions->get(), object);
            UI::Settings::write("BackgroundColor", p.backgroundColor->get(), object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


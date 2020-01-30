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
            int widgetCurrentTab = 0;
            std::shared_ptr<ValueSubject<ImageViewLock> > lock;
            std::shared_ptr<ValueSubject<GridOptions> > gridOptions;
            std::shared_ptr<ValueSubject<AV::Image::Color> > backgroundColor;
            std::shared_ptr<ValueSubject<ScrollWheelZoomSpeed> > scrollWheelZoomSpeed;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ViewSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ViewSettings", context);

            DJV_PRIVATE_PTR();
            p.lock = ValueSubject<ImageViewLock>::create(ImageViewLock::Fill);
            p.gridOptions = ValueSubject<GridOptions>::create();
            p.backgroundColor = ValueSubject<AV::Image::Color>::create(AV::Image::Color(0.F, 0.F, 0.F));
            p.scrollWheelZoomSpeed = ValueSubject<ScrollWheelZoomSpeed>::create(ScrollWheelZoomSpeed::Slow);
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

        int ViewSettings::getWidgetCurrentTab() const
        {
            return _p->widgetCurrentTab;
        }

        void ViewSettings::setWidgetCurrentTab(int value)
        {
            _p->widgetCurrentTab = value;
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

        std::shared_ptr<IValueSubject<AV::Image::Color> > ViewSettings::observeBackgroundColor() const
        {
            return _p->backgroundColor;
        }

        void ViewSettings::setBackgroundColor(const AV::Image::Color& value)
        {
            _p->backgroundColor->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<ScrollWheelZoomSpeed> > ViewSettings::observeScrollWheelZoomSpeed() const
        {
            return _p->scrollWheelZoomSpeed;
        }

        void ViewSettings::setScrollWheelZoomSpeed(ScrollWheelZoomSpeed value)
        {
            _p->scrollWheelZoomSpeed->setIfChanged(value);
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
                UI::Settings::read("WidgetCurrentTab", object, p.widgetCurrentTab);
                UI::Settings::read("Lock", object, p.lock);
                UI::Settings::read("GridOptions", object, p.gridOptions);
                UI::Settings::read("BackgroundColor", object, p.backgroundColor);
                UI::Settings::read("ScrollWheelZoomSpeed", object, p.scrollWheelZoomSpeed);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ViewSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("WidgetCurrentTab", p.widgetCurrentTab, object);
            UI::Settings::write("Lock", p.lock->get(), object);
            UI::Settings::write("GridOptions", p.gridOptions->get(), object);
            UI::Settings::write("BackgroundColor", p.backgroundColor->get(), object);
            UI::Settings::write("ScrollWheelZoomSpeed", p.scrollWheelZoomSpeed->get(), object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


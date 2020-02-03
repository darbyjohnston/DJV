//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvViewApp/ImageSettings.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSettings::Private
        {
            int colorSpaceCurrentTab = 0;
            int colorCurrentTab = 0;
            std::shared_ptr<ValueSubject<ImageRotate> > rotate;
            std::shared_ptr<ValueSubject<UI::ImageAspectRatio> > aspectRatio;
            std::map<std::string, BBox2f> widgetGeom;
        };

        void ImageSettings::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISettings::_init("djv::ViewApp::ImageSettings", context);

            DJV_PRIVATE_PTR();
            p.rotate = ValueSubject<ImageRotate>::create(ImageRotate::_0);
            p.aspectRatio = ValueSubject<UI::ImageAspectRatio>::create(UI::ImageAspectRatio::Default);
            _load();
        }

        ImageSettings::ImageSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ImageSettings> ImageSettings::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ImageSettings>(new ImageSettings);
            out->_init(context);
            return out;
        }

        int ImageSettings::getColorSpaceCurrentTab() const
        {
            return _p->colorSpaceCurrentTab;
        }

        int ImageSettings::getColorCurrentTab() const
        {
            return _p->colorCurrentTab;
        }

        void ImageSettings::setColorSpaceCurrentTab(int value)
        {
            _p->colorSpaceCurrentTab = value;
        }

        void ImageSettings::setColorCurrentTab(int value)
        {
            _p->colorCurrentTab = value;
        }

        std::shared_ptr<IValueSubject<ImageRotate> > ImageSettings::observeRotate() const
        {
            return _p->rotate;
        }

        std::shared_ptr<IValueSubject<UI::ImageAspectRatio> > ImageSettings::observeAspectRatio() const
        {
            return _p->aspectRatio;
        }

        void ImageSettings::setRotate(ImageRotate value)
        {
            _p->rotate->setIfChanged(value);
        }

        void ImageSettings::setAspectRatio(UI::ImageAspectRatio value)
        {
            _p->aspectRatio->setIfChanged(value);
        }

        const std::map<std::string, BBox2f>& ImageSettings::getWidgetGeom() const
        {
            return _p->widgetGeom;
        }

        void ImageSettings::setWidgetGeom(const std::map<std::string, BBox2f>& value)
        {
            _p->widgetGeom = value;
        }

        void ImageSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("ColorSpaceCurrentTab", object, p.colorSpaceCurrentTab);
                UI::Settings::read("ColorCurrentTab", object, p.colorCurrentTab);
                UI::Settings::read("Rotate", object, p.rotate);
                UI::Settings::read("AspectRatio", object, p.aspectRatio);
                UI::Settings::read("WidgetGeom", object, p.widgetGeom);
            }
        }

        picojson::value ImageSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("ColorSpaceCurrentTab", p.colorSpaceCurrentTab, object);
            UI::Settings::write("ColorCurrentTab", p.colorCurrentTab, object);
            UI::Settings::write("Rotate", p.rotate->get(), object);
            UI::Settings::write("AspectRatio", p.aspectRatio->get(), object);
            UI::Settings::write("WidgetGeom", p.widgetGeom, object);
            return out;
        }

    } // namespace ViewApp
} // namespace djv


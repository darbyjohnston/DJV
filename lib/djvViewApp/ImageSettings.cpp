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
            std::shared_ptr<ValueSubject<std::string> > inputColorSpace;
            std::shared_ptr<ValueSubject<std::string> > outputColorSpace;
            std::shared_ptr<ValueSubject<std::string> > colorDisplay;
            std::shared_ptr<ValueSubject<std::string> > colorView;
            std::shared_ptr<ValueSubject<ImageRotate> > rotate;
            std::shared_ptr<ValueSubject<ImageAspectRatio> > aspectRatio;
        };

        void ImageSettings::_init(Context * context)
        {
            ISettings::_init("djv::ViewApp::ImageSettings", context);

            DJV_PRIVATE_PTR();
            p.inputColorSpace = ValueSubject<std::string>::create();
            p.outputColorSpace = ValueSubject<std::string>::create();
            auto ocioSystem = context->getSystemT<AV::OCIOSystem>();
            p.colorDisplay = ValueSubject<std::string>::create(ocioSystem->getDefaultDisplay());
            p.colorView = ValueSubject<std::string>::create(ocioSystem->getDefaultView());

            p.rotate = ValueSubject<ImageRotate>::create(ImageRotate::_0);
            p.aspectRatio = ValueSubject<ImageAspectRatio>::create(ImageAspectRatio::Default);
            _load();
        }

        ImageSettings::ImageSettings() :
            _p(new Private)
        {}

        std::shared_ptr<ImageSettings> ImageSettings::create(Context * context)
        {
            auto out = std::shared_ptr<ImageSettings>(new ImageSettings);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageSettings::observeInputColorSpace() const
        {
            return _p->inputColorSpace;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageSettings::observeOutputColorSpace() const
        {
            return _p->outputColorSpace;
        }

        void ImageSettings::setInputColorSpace(const std::string& value)
        {
            _p->inputColorSpace->setIfChanged(value);
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageSettings::observeColorDisplay() const
        {
            return _p->colorDisplay;
        }

        void ImageSettings::setColorDisplay(const std::string& value)
        {
            if (_p->colorDisplay->setIfChanged(value))
            {
                _p->outputColorSpace->setIfChanged(_getOutputColorSpace());
            }
        }

        std::shared_ptr<Core::IValueSubject<std::string> > ImageSettings::observeColorView() const
        {
            return _p->colorView;
        }

        void ImageSettings::setColorView(const std::string& value)
        {
            if (_p->colorView->setIfChanged(value))
            {
                _p->outputColorSpace->setIfChanged(_getOutputColorSpace());
            }
        }

        std::shared_ptr<IValueSubject<ImageRotate> > ImageSettings::observeImageRotate() const
        {
            return _p->rotate;
        }

        void ImageSettings::setImageRotate(ImageRotate value)
        {
            _p->rotate->setIfChanged(value);
        }

        std::shared_ptr<IValueSubject<ImageAspectRatio> > ImageSettings::observeImageAspectRatio() const
        {
            return _p->aspectRatio;
        }

        void ImageSettings::setImageAspectRatio(ImageAspectRatio value)
        {
            _p->aspectRatio->setIfChanged(value);
        }

        void ImageSettings::load(const picojson::value & value)
        {
            if (value.is<picojson::object>())
            {
                DJV_PRIVATE_PTR();
                const auto & object = value.get<picojson::object>();
                UI::Settings::read("InputColorSpace", object, p.inputColorSpace);
                UI::Settings::read("ColorDisplay", object, p.colorDisplay);
                UI::Settings::read("ColorView", object, p.colorView);
                UI::Settings::read("Rotate", object, p.rotate);
                UI::Settings::read("AspectRatio", object, p.aspectRatio);
            }
        }

        picojson::value ImageSettings::save()
        {
            DJV_PRIVATE_PTR();
            picojson::value out(picojson::object_type, true);
            auto & object = out.get<picojson::object>();
            UI::Settings::write("InputColorSpace", p.inputColorSpace->get(), object);
            UI::Settings::write("ColorDisplay", p.colorDisplay->get(), object);
            UI::Settings::write("ColorView", p.colorView->get(), object);
            UI::Settings::write("Rotate", p.rotate->get(), object);
            UI::Settings::write("AspectRatio", p.aspectRatio->get(), object);
            return out;
        }

        std::string ImageSettings::_getOutputColorSpace() const
        {
            DJV_PRIVATE_PTR();
            std::string out;
            auto ocioSystem = getContext()->getSystemT<AV::OCIOSystem>();
            for (const auto& i : ocioSystem->observeDisplays()->get())
            {
                if (p.colorDisplay->get() == i.name)
                {
                    for (const auto& j : i.views)
                    {
                        if (p.colorView->get() == j.name)
                        {
                            out = j.colorSpace;
                            break;
                        }
                    }
                }
            }
            return out;
        }

    } // namespace ViewApp
} // namespace djv


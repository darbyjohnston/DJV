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

#include <djvUI/ColorSpaceSettings.h>

#include <djvUI/Widget.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct ColorSpace::Private
            {
                std::shared_ptr<ValueSubject<std::string> > colorSpace;
                std::shared_ptr<ValueSubject<std::string> > colorDisplay;
                std::shared_ptr<ValueSubject<std::string> > colorView;
                std::shared_ptr<ValueSubject<std::string> > outputColorSpace;
            };

            void ColorSpace::_init(Context * context)
            {
                ISettings::_init("djv::UI::Settings::ColorSpace", context);
                DJV_PRIVATE_PTR();
                p.colorSpace = ValueSubject<std::string>::create();
                auto ocioSystem = context->getSystemT<AV::OCIOSystem>();
                p.colorDisplay = ValueSubject<std::string>::create(ocioSystem->getDefaultDisplay());
                p.colorView = ValueSubject<std::string>::create(ocioSystem->getDefaultView());
                p.outputColorSpace = ValueSubject<std::string>::create(_getOutputColorSpace());
                _load();
            }

            ColorSpace::ColorSpace() :
                _p(new Private)
            {}

            ColorSpace::~ColorSpace()
            {}

            std::shared_ptr<ColorSpace> ColorSpace::create(Context * context)
            {
                auto out = std::shared_ptr<ColorSpace>(new ColorSpace);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Core::IValueSubject<std::string> > ColorSpace::observeColorSpace() const
            {
                return _p->colorSpace;
            }

            std::shared_ptr<Core::IValueSubject<std::string> > ColorSpace::observeColorDisplay() const
            {
                return _p->colorDisplay;
            }

            std::shared_ptr<Core::IValueSubject<std::string> > ColorSpace::observeColorView() const
            {
                return _p->colorView;
            }

            std::shared_ptr<Core::IValueSubject<std::string> > ColorSpace::observeOutputColorSpace() const
            {
                return _p->outputColorSpace;
            }

            void ColorSpace::setColorSpace(const std::string& value)
            {
                _p->colorSpace->setIfChanged(value);
            }

            void ColorSpace::setColorDisplay(const std::string& value)
            {
                if (_p->colorDisplay->setIfChanged(value))
                {
                    _p->outputColorSpace->setIfChanged(_getOutputColorSpace());
                }
            }

            void ColorSpace::setColorView(const std::string& value)
            {
                if (_p->colorView->setIfChanged(value))
                {
                    _p->outputColorSpace->setIfChanged(_getOutputColorSpace());
                }
            }

            void ColorSpace::load(const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto & object = value.get<picojson::object>();
                    UI::Settings::read("ColorSpace", object, p.colorSpace);
                    UI::Settings::read("ColorDisplay", object, p.colorDisplay);
                    UI::Settings::read("ColorView", object, p.colorView);
                }
            }

            picojson::value ColorSpace::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                UI::Settings::write("ColorSpace", p.colorSpace->get(), object);
                UI::Settings::write("ColorDisplay", p.colorDisplay->get(), object);
                UI::Settings::write("ColorView", p.colorView->get(), object);
                return out;
            }

            std::string ColorSpace::_getOutputColorSpace() const
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

        } // namespace Settings
    } // namespace UI
} // namespace djv


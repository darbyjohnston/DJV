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
                std::vector<AV::OCIO::Config> configs;
                int currentIndex = 0;
                std::shared_ptr<ListObserver<AV::OCIO::Config> > configsObserver;
                std::shared_ptr<ValueObserver<int> > currentIndexObserver;
            };

            void ColorSpace::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::ColorSpace", context);
                DJV_PRIVATE_PTR();
                {
                    AV::OCIO::Config config;
                    config.fileName = "nuke-default";
                    config.name = "nuke-default";
                    config.display = "default";
                    config.view = "sRGB";
                    config.fileColorSpaces =
                    {
                        { "",        "sRGB" },
                        { "Cineon",  "Cineon" },
                        { "DPX",     "Cineon" },
                        { "OpenEXR", "linear" }
                    };
                    p.configs.push_back(config);
                }
                {
                    AV::OCIO::Config config;
                    config.fileName = "spi-anim";
                    config.name = "spi-anim";
                    config.display = "sRGB";
                    config.view = "Film";
                    config.fileColorSpaces =
                    {
                        { "",        "" },
                        { "OpenEXR", "lnh" }
                    };
                    p.configs.push_back(config);
                }
                {
                    AV::OCIO::Config config;
                    config.fileName = "spi-vfx";
                    config.name = "spi-vfx";
                    config.display = "sRGB";
                    config.view = "Film";
                    config.fileColorSpaces =
                    {
                        { "",        "" },
                        { "Cineon",  "lg10" },
                        { "DPX",     "lg10" },
                        { "OpenEXR", "lnh" }
                    };
                    p.configs.push_back(config);
                }

                _load();

                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                for (const auto& i : p.configs)
                {
                    ocioSystem->addConfig(i);
                }
                ocioSystem->setCurrentIndex(p.currentIndex);

                auto weak = std::weak_ptr<ColorSpace>(std::dynamic_pointer_cast<ColorSpace>(shared_from_this()));
                p.configsObserver = ListObserver<AV::OCIO::Config>::create(
                    ocioSystem->observeConfigs(),
                    [weak](const std::vector<AV::OCIO::Config>& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->configs = value;
                        }
                    });
                p.currentIndexObserver = ValueObserver<int>::create(
                    ocioSystem->observeCurrentIndex(),
                    [weak](int value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->currentIndex = value;
                        }
                    });
            }

            ColorSpace::ColorSpace() :
                _p(new Private)
            {}

            ColorSpace::~ColorSpace()
            {}

            std::shared_ptr<ColorSpace> ColorSpace::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<ColorSpace>(new ColorSpace);
                out->_init(context);
                return out;
            }

            void ColorSpace::load(const picojson::value & value)
            {
                DJV_PRIVATE_PTR();
                if (value.is<picojson::object>())
                {
                    const auto& object = value.get<picojson::object>();
                    std::vector<AV::OCIO::Config> configs;
                    UI::Settings::read("Configs", object, p.configs);
                    UI::Settings::read("CurrentIndex", object, p.currentIndex);
                }
            }

            picojson::value ColorSpace::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto& object = out.get<picojson::object>();
                UI::Settings::write("Configs", p.configs, object);
                UI::Settings::write("CurrentIndex", p.currentIndex, object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


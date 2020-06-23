// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ColorSpaceSettings.h>

#include <djvUI/Widget.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
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

            void ColorSpace::load(const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                if (value.IsObject())
                {
                    std::vector<AV::OCIO::Config> configs;
                    UI::Settings::read("Configs", value, p.configs);
                    UI::Settings::read("CurrentIndex", value, p.currentIndex);
                }
            }

            rapidjson::Value ColorSpace::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                UI::Settings::write("Configs", p.configs, out, allocator);
                UI::Settings::write("CurrentIndex", p.currentIndex, out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


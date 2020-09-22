// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/ColorSpaceSettings.h>

#include <djvUI/Widget.h>

#include <djvOCIO/OCIOSystemFunc.h>

#include <djvSystem/Context.h>

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
                std::shared_ptr<OCIO::OCIOSystem> ocioSystem;

                OCIO::ConfigMode configMode = OCIO::ConfigMode::User;
                OCIO::UserConfigs userConfigs;
                OCIO::Config envConfig;

                std::shared_ptr<ValueObserver<OCIO::ConfigMode> > configModeObserver;
                std::shared_ptr<ValueObserver<OCIO::UserConfigs> > userConfigsObserver;
                std::shared_ptr<ValueObserver<OCIO::Config> > envConfigObserver;
            };

            void ColorSpace::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::ColorSpace", context);
                DJV_PRIVATE_PTR();

                p.ocioSystem = context->getSystemT<OCIO::OCIOSystem>();

                {
                    OCIO::Config config;
                    config.fileName = "nuke-default";
                    config.name = "nuke-default";
                    config.display = "default";
                    config.view = "sRGB";
                    config.imageColorSpaces =
                    {
                        { "",        "sRGB" },
                        { "Cineon",  "Cineon" },
                        { "DPX",     "Cineon" },
                        { "OpenEXR", "linear" }
                    };
                    p.userConfigs.first.emplace_back(config);
                }
                {
                    OCIO::Config config;
                    config.fileName = "spi-anim";
                    config.name = "spi-anim";
                    config.display = "sRGB";
                    config.view = "Film";
                    config.imageColorSpaces =
                    {
                        { "",        "" },
                        { "OpenEXR", "lnh" }
                    };
                    p.userConfigs.first.emplace_back(config);
                }
                {
                    OCIO::Config config;
                    config.fileName = "spi-vfx";
                    config.name = "spi-vfx";
                    config.display = "sRGB";
                    config.view = "Film";
                    config.imageColorSpaces =
                    {
                        { "",        "" },
                        { "Cineon",  "lg10" },
                        { "DPX",     "lg10" },
                        { "OpenEXR", "lnh" }
                    };
                    p.userConfigs.first.emplace_back(config);
                }

                _load();

                if (p.ocioSystem->observeConfigMode()->get() != OCIO::ConfigMode::Env)
                {
                    p.ocioSystem->setConfigMode(
                        OCIO::ConfigMode::None == p.configMode ?
                        OCIO::ConfigMode::None :
                        OCIO::ConfigMode::User);
                }
                for (const auto& i : p.userConfigs.first)
                {
                    p.ocioSystem->addUserConfig(i);
                }
                p.ocioSystem->setCurrentUserConfig(p.userConfigs.second);
                if (p.envConfig.isValid())
                {
                    p.ocioSystem->setEnvConfig(p.envConfig);
                }

                auto weak = std::weak_ptr<ColorSpace>(std::dynamic_pointer_cast<ColorSpace>(shared_from_this()));
                p.configModeObserver = ValueObserver<OCIO::ConfigMode>::create(
                    p.ocioSystem->observeConfigMode(),
                    [weak](const OCIO::ConfigMode& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->configMode = value;
                        }
                    });
                p.userConfigsObserver = ValueObserver<OCIO::UserConfigs>::create(
                    p.ocioSystem->observeUserConfigs(),
                    [weak](const OCIO::UserConfigs& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->userConfigs = value;
                        }
                    });
                p.envConfigObserver = ValueObserver<OCIO::Config>::create(
                    p.ocioSystem->observeEnvConfig(),
                    [weak](const OCIO::Config& value)
                    {
                        if (auto settings = weak.lock())
                        {
                            settings->_p->envConfig = value;
                        }
                    });
            }

            ColorSpace::ColorSpace() :
                _p(new Private)
            {}

            ColorSpace::~ColorSpace()
            {}

            std::shared_ptr<ColorSpace> ColorSpace::create(const std::shared_ptr<System::Context>& context)
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
                    std::vector<OCIO::Config> configs;
                    UI::Settings::read("ConfigMode", value, p.configMode);
                    UI::Settings::read("UserConfigs", value, p.userConfigs.first);
                    UI::Settings::read("CurrentUserConfig", value, p.userConfigs.second);
                    if (value.HasMember("EnvConfig") && p.ocioSystem->hasEnvConfig())
                    {
                        UI::Settings::read("EnvConfig", value, p.envConfig);
                    }
                }
            }

            rapidjson::Value ColorSpace::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                UI::Settings::write("ConfigMode", p.configMode, out, allocator);
                UI::Settings::write("UserConfigs", p.userConfigs.first, out, allocator);
                UI::Settings::write("CurrentUserConfig", p.userConfigs.second, out, allocator);
                if (p.envConfig.isValid())
                {
                    UI::Settings::write("EnvConfig", p.envConfig, out, allocator);
                }
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


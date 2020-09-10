// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OCIOSystemTest.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/ValueObserver.h>

#include <iostream>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        OCIOSystemTest::OCIOSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::AVTest::OCIOSystemTest", tempPath, context)
        {}
        
        void OCIOSystemTest::run()
        {
            _enum();
            _config();
            _system();
            _operators();
            _serialize();
        }
        
        void OCIOSystemTest::_enum()
        {
            for (const auto& i : OCIO::getConfigModeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Config mode: " + _getText(ss.str()));
            }
        }
        
        void OCIOSystemTest::_config()
        {
            {
                const OCIO::Config config;
                DJV_ASSERT(config.fileName.empty());
                DJV_ASSERT(config.name.empty());
                DJV_ASSERT(config.display.empty());
                DJV_ASSERT(config.view.empty());
                DJV_ASSERT(config.imageColorSpaces.empty());
            }
            
            {
                DJV_ASSERT("color" == OCIO::Config::getNameFromFileName("/color/config.ocio"));
            }
        }
        
        namespace
        {
            OCIO::Config createConfig()
            {
                OCIO::Config config;
                config.fileName = "fileName";
                config.name = "name";
                config.display = "display";
                config.view = "view";
                config.imageColorSpaces = { { "PNG", "sRGB" }, { "DPX", "Cineon" } };
                return config;
            }
        
        } // namespace
        
        void OCIOSystemTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<OCIO::System>();
                
                {
                    std::stringstream ss;
                    ss << system->hasEnvConfig();
                    _print("Env config: " + ss.str());
                }
                
                OCIO::ConfigMode configMode = OCIO::ConfigMode::First;
                OCIO::Config cmdLineConfig;
                OCIO::Config envConfig;
                OCIO::UserConfigs userConfigs;
                OCIO::Config currentConfig;
                OCIO::Displays displays;
                OCIO::Views views;
                OCIO::ImageColorSpaces imageColorSpaces;
                std::vector<std::string> colorSpaces;
                
                auto configModeObserver = ValueObserver<OCIO::ConfigMode>::create(
                    system->observeConfigMode(),
                    [&configMode](OCIO::ConfigMode value)
                    {
                        configMode = value;
                    });
                
                auto cmdLineConfigObserver = ValueObserver<OCIO::Config>::create(
                    system->observeCmdLineConfig(),
                    [&cmdLineConfig](const OCIO::Config& value)
                    {
                        cmdLineConfig = value;
                    });
                
                auto envConfigObserver = ValueObserver<OCIO::Config>::create(
                    system->observeEnvConfig(),
                    [&envConfig](const OCIO::Config& value)
                    {
                        envConfig = value;
                    });
                
                auto userConfigsObserver = ValueObserver<OCIO::UserConfigs>::create(
                    system->observeUserConfigs(),
                    [&userConfigs](const OCIO::UserConfigs& value)
                    {
                        userConfigs = value;
                    });
                
                auto currentConfigObserver = ValueObserver<OCIO::Config>::create(
                    system->observeCurrentConfig(),
                    [&currentConfig](const OCIO::Config& value)
                    {
                        currentConfig = value;
                    });

                auto displaysObserver = ValueObserver<OCIO::Displays>::create(
                    system->observeDisplays(),
                    [&displays](const OCIO::Displays& value)
                    {
                        displays = value;
                    });

                auto viewsObserver = ValueObserver<OCIO::Views>::create(
                    system->observeViews(),
                    [&views](const OCIO::Views& value)
                    {
                        views = value;
                    });

                auto imageColorSpacesObserver = MapObserver<std::string, std::string>::create(
                    system->observeImageColorSpaces(),
                    [&imageColorSpaces](const std::map<std::string, std::string>& value)
                    {
                        imageColorSpaces = value;
                    });

                auto colorSpacesObserver = ListObserver<std::string>::create(
                    system->observeColorSpaces(),
                    [&colorSpaces](const std::vector<std::string>& value)
                    {
                        colorSpaces = value;
                    });

                auto resourceSystem = context->getSystemT<ResourceSystem>();
                OCIO::Config config;
                const std::string fileName = FileSystem::Path(
                    resourceSystem->getPath(FileSystem::ResourcePath::Color),
                    "spi-vfx/config.ocio").get();
                config.fileName = fileName;
                system->setConfigMode(OCIO::ConfigMode::CmdLine);
                system->setCmdLineConfig(config);
                if (displays.first.size() > 1)
                {
                    system->setCurrentDisplay(0);
                    system->setCurrentDisplay(1);
                }
                if (views.first.size() > 1)
                {
                    system->setCurrentView(0);
                    system->setCurrentView(1);
                }
                OCIO::ImageColorSpaces imageColorSpaces2;
                imageColorSpaces2["Cineon"] = "lg10";
                imageColorSpaces2["OpenEXR"] = "lnh";
                system->setImageColorSpaces(imageColorSpaces2);
                OCIO::Config errorConfig;
                errorConfig.fileName = "error";
                system->setCmdLineConfig(errorConfig);

                system->setConfigMode(OCIO::ConfigMode::Env);
                system->setEnvConfig(config);
                if (displays.first.size() > 1)
                {
                    system->setCurrentDisplay(0);
                    system->setCurrentDisplay(1);
                }
                if (views.first.size() > 1)
                {
                    system->setCurrentView(0);
                    system->setCurrentView(1);
                }
                system->setImageColorSpaces(imageColorSpaces2);
                system->setEnvConfig(errorConfig);

                system->setConfigMode(OCIO::ConfigMode::User);
                if (!userConfigs.first.empty())
                {
                    OCIO::Config config = userConfigs.first[0];
                    system->removeUserConfig(0);
                    int index = system->addUserConfig(config);
                    system->setCurrentUserConfig(index);
                    system->setCurrentUserConfig(index);
                    DJV_ASSERT(config == system->observeCurrentConfig()->get());
                }
                system->addUserConfig(fileName);
                if (displays.first.size() > 1)
                {
                    system->setCurrentDisplay(0);
                    system->setCurrentDisplay(1);
                }
                if (views.first.size() > 1)
                {
                    system->setCurrentView(0);
                    system->setCurrentView(1);
                }
                if (displays.first.size() > 1)
                {
                    for (const auto& j : views.first)
                    {
                        std::stringstream ss;
                        ss << "Color space " << displays.first[1] << "/";
                        ss << (!j.empty() ? j : "Default") << ": ";
                        ss << system->getColorSpace(displays.first[1], j);
                        _print(ss.str());
                    }
                }
                system->setImageColorSpaces(imageColorSpaces2);
                for (size_t i = 0; i < userConfigs.first.size(); ++i)
                {
                    system->removeUserConfig(i);
                }
                DJV_ASSERT(system->getColorSpace(std::string(), std::string()).empty());
                system->addUserConfig(errorConfig);
            }
        }
        
        void OCIOSystemTest::_operators()
        {
            {
                OCIO::Config config = createConfig();
                DJV_ASSERT(config == config);
            }
        }
        
        void OCIOSystemTest::_serialize()
        {
            {
                OCIO::ConfigMode value = OCIO::ConfigMode::First;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                OCIO::ConfigMode value2;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value();
                OCIO::ConfigMode value;
                fromJSON(json, value);
                DJV_ASSERT(false);
            }
            
            catch (const std::exception&)
            {}
            {
                OCIO::Config config = createConfig();
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(config, allocator);
                OCIO::Config config2;
                fromJSON(json, config2);
                DJV_ASSERT(config == config2);
            }

            try
            {
                auto json = rapidjson::Value();
                OCIO::Config config;
                fromJSON(json, config);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace AVTest
} // namespace djv


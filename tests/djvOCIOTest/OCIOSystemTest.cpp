// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvOCIOTest/OCIOSystemTest.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/ResourceSystem.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/ValueObserver.h>

#include <sstream>

using namespace djv::Core;
using namespace djv::OCIO;

namespace djv
{
    namespace OCIOTest
    {
        OCIOSystemTest::OCIOSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::OCIOTest::OCIOSystemTest", tempPath, context)
        {}
        
        void OCIOSystemTest::run()
        {
            _config();
            _system();
            _operators();
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
        
        void OCIOSystemTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<OCIO::OCIOSystem>();
                
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
                
                auto configModeObserver = Observer::Value<OCIO::ConfigMode>::create(
                    system->observeConfigMode(),
                    [&configMode](OCIO::ConfigMode value)
                    {
                        configMode = value;
                    });
                
                auto cmdLineConfigObserver = Observer::Value<OCIO::Config>::create(
                    system->observeCmdLineConfig(),
                    [&cmdLineConfig](const OCIO::Config& value)
                    {
                        cmdLineConfig = value;
                    });
                
                auto envConfigObserver = Observer::Value<OCIO::Config>::create(
                    system->observeEnvConfig(),
                    [&envConfig](const OCIO::Config& value)
                    {
                        envConfig = value;
                    });
                
                auto userConfigsObserver = Observer::Value<OCIO::UserConfigs>::create(
                    system->observeUserConfigs(),
                    [&userConfigs](const OCIO::UserConfigs& value)
                    {
                        userConfigs = value;
                    });
                
                auto currentConfigObserver = Observer::Value<OCIO::Config>::create(
                    system->observeCurrentConfig(),
                    [&currentConfig](const OCIO::Config& value)
                    {
                        currentConfig = value;
                    });

                auto displaysObserver = Observer::Value<OCIO::Displays>::create(
                    system->observeDisplays(),
                    [&displays](const OCIO::Displays& value)
                    {
                        displays = value;
                    });

                auto viewsObserver = Observer::Value<OCIO::Views>::create(
                    system->observeViews(),
                    [&views](const OCIO::Views& value)
                    {
                        views = value;
                    });

                auto imageColorSpacesObserver = Observer::Map<std::string, std::string>::create(
                    system->observeImageColorSpaces(),
                    [&imageColorSpaces](const std::map<std::string, std::string>& value)
                    {
                        imageColorSpaces = value;
                    });

                auto colorSpacesObserver = Observer::List<std::string>::create(
                    system->observeColorSpaces(),
                    [&colorSpaces](const std::vector<std::string>& value)
                    {
                        colorSpaces = value;
                    });

                auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                OCIO::Config config;
                const std::string fileName = System::File::Path(
                    resourceSystem->getPath(System::File::ResourcePath::Color),
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

        void OCIOSystemTest::_operators()
        {
            {
                OCIO::Config config = createConfig();
                DJV_ASSERT(config == config);
            }
        }
        
    } // namespace OCIOTest
} // namespace djv


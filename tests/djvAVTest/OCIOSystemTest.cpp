// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/OCIOSystemTest.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        OCIOSystemTest::OCIOSystemTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::OCIOSystemTest", context)
        {}
        
        void OCIOSystemTest::run()
        {
            _config();
            _system();
            _operators();
            _serialize();
        }
        
        void OCIOSystemTest::_config()
        {
            {
                const OCIO::Config config;
                DJV_ASSERT(config.fileName.empty());
                DJV_ASSERT(config.name.empty());
                DJV_ASSERT(config.display.empty());
                DJV_ASSERT(config.view.empty());
                DJV_ASSERT(config.fileColorSpaces.empty());
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
                config.fileColorSpaces = { { "PNG", "sRGB" }, { "DPX", "Cineon" } };
                return config;
            }
        
        } // namespace
        
        void OCIOSystemTest::_system()
        {
            if (auto context = getContext().lock())
            {
                auto system = context->getSystemT<OCIO::System>();
                
                auto configsObserver = ListObserver<OCIO::Config>::create(
                    system->observeConfigs(),
                    [this](const std::vector<OCIO::Config>& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << "config " << j << ": " << i.name;
                            _print(ss.str());
                            ++j;
                        }
                    });
                
                auto currentConfigObserver = ValueObserver<OCIO::Config>::create(
                    system->observeCurrentConfig(),
                    [this](const OCIO::Config& value)
                    {
                        std::stringstream ss;
                        ss << "current config: " << value.name;
                        _print(ss.str());
                    });
                auto currentIndexObserver = ValueObserver<int>::create(
                    system->observeCurrentIndex(),
                    [this](int value)
                    {
                        std::stringstream ss;
                        ss << "current index: " << value;
                        _print(ss.str());
                    });
                auto colorSpacesObserver = ListObserver<std::string>::create(
                    system->observeColorSpaces(),
                    [this](const std::vector<std::string>& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << "color space " << j << ": " << i;
                            _print(ss.str());
                            ++j;
                        }
                    });
                auto displaysObserver = ListObserver<OCIO::Display>::create(
                    system->observeDisplays(),
                    [this, system](const std::vector<OCIO::Display>& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << "display " << j << ": " << i.name;
                            _print(ss.str());
                            for (const auto& k : i.views)
                            {
                                {
                                    std::stringstream ss;
                                    ss << "    view: " << k.name;
                                    _print(ss.str());
                                }
                                {
                                    std::stringstream ss;
                                    ss << "    color space: " << system->getColorSpace(i.name, k.name);
                                    _print(ss.str());
                                }
                            }
                            ++j;
                        }
                    });
                auto viewsObserver = ListObserver<std::string>::create(
                    system->observeViews(),
                    [this](const std::vector<std::string>& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value)
                        {
                            std::stringstream ss;
                            ss << "view " << j << ": " << i;
                            _print(ss.str());
                            ++j;
                        }
                    });
                
                auto configs = system->observeConfigs()->get();
                if (!configs.empty())
                {
                    OCIO::Config config = configs[0];
                    system->removeConfig(0);
                    int index = system->addConfig(config);
                    system->setCurrentIndex(index);
                    system->setCurrentIndex(index);
                    DJV_ASSERT(index == system->addConfig(config));
                }
                
                OCIO::Config config;
                system->setCurrentConfig(config);
                system->setCurrentConfig(config);
                DJV_ASSERT(-1 == system->addConfig(config));

                for (size_t i = 0; i < configs.size(); ++i)
                {
                    system->removeConfig(i);                    
                }

                DJV_ASSERT(system->getColorSpace(std::string(), std::string()).empty());
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
                OCIO::Config config = createConfig();
                auto json = toJSON(config);
                OCIO::Config config2;
                fromJSON(json, config2);
                DJV_ASSERT(config == config2);
            }

            try
            {
                auto json = picojson::value(picojson::string_type, true);
                OCIO::Config config;
                fromJSON(json, config);
                DJV_ASSERT(false);
            }
            catch (const std::exception&)
            {}
        }

    } // namespace AVTest
} // namespace djv


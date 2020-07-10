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
                
                auto userConfigsObserver = ValueObserver<OCIO::UserConfigs>::create(
                    system->observeUserConfigs(),
                    [this](const OCIO::UserConfigs& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value.first)
                        {
                            std::stringstream ss;
                            ss << "config " << j << ": " << i.name;
                            _print(ss.str());
                            ++j;
                        }
                        std::stringstream ss;
                        ss << "current config: " << value.second;
                        _print(ss.str());
                    });
                
                auto currentConfigObserver = ValueObserver<OCIO::Config>::create(
                    system->observeCurrentConfig(),
                    [this](const OCIO::Config& value)
                    {
                        std::stringstream ss;
                        ss << "current config: " << value.name;
                        _print(ss.str());
                    });

                auto displaysObserver = ValueObserver<OCIO::Displays>::create(
                    system->observeDisplays(),
                    [this](const OCIO::Displays& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value.first)
                        {
                            std::stringstream ss;
                            ss << "display " << j << ": " << i;
                            _print(ss.str());
                            ++j;
                        }
                        std::stringstream ss;
                        ss << "current display: " << value.second;
                        _print(ss.str());
                    });
                auto viewsObserver = ValueObserver<OCIO::Views>::create(
                    system->observeViews(),
                    [this](const OCIO::Views& value)
                    {
                        size_t j = 0;
                        for (const auto& i : value.first)
                        {
                            std::stringstream ss;
                            ss << "view " << j << ": " << i;
                            _print(ss.str());
                            ++j;
                        }
                        std::stringstream ss;
                        ss << "current view: " << value.second;
                        _print(ss.str());
                    });

                auto userConfigs = system->observeUserConfigs()->get();
                if (!userConfigs.first.empty())
                {
                    OCIO::Config config = userConfigs.first[0];
                    system->removeUserConfig(0);
                    int index = system->addUserConfig(config);
                    system->setCurrentUserConfig(index);
                    system->setCurrentUserConfig(index);
                    DJV_ASSERT(config == system->observeCurrentConfig()->get());
                }

                for (size_t i = 0; i < userConfigs.first.size(); ++i)
                {
                    system->removeUserConfig(i);
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


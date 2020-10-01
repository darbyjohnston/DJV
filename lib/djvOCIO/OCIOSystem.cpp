// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/CoreSystem.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/ResourceSystem.h>

#include <djvCore/OSFunc.h>

#include <OpenColorIO/OpenColorIO.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>

using namespace djv::Core;
namespace _OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace OCIO
    {
        Config::Config()
        {}

        std::string Config::getNameFromFileName(const std::string& value)
        {
            System::File::Path path(value);
            path.cdUp();
            return path.getFileName();
        }

        struct OCIOSystem::Private
        {
            Private(OCIOSystem& p) :
                p(p)
            {}

            OCIOSystem& p;

            typedef std::pair<_OCIO::ConstConfigRcPtr, Config> ConfigPair;

            ConfigMode configMode = ConfigMode::First;
            ConfigPair cmdLineConfig;
            bool hasEnvConfig = false;
            ConfigPair envConfig;
            std::vector<ConfigPair> userConfigs;
            int currentUserConfig = -1;
            std::vector<Display> displays;
            std::vector<std::string> colorSpaces;

            std::shared_ptr<Observer::ValueSubject<ConfigMode> > configModeSubject;
            std::shared_ptr<Observer::ValueSubject<Config> > cmdLineConfigSubject;
            std::shared_ptr<Observer::ValueSubject<Config> > envConfigSubject;
            std::shared_ptr<Observer::ValueSubject<UserConfigs> > userConfigsSubject;
            std::shared_ptr<Observer::ValueSubject<Config> > currentConfigSubject;
            std::shared_ptr<Observer::ValueSubject<Displays> > displaysSubject;
            std::shared_ptr<Observer::ValueSubject<Views> > viewsSubject;
            std::shared_ptr<Observer::MapSubject<std::string, std::string> > imageColorSpacesSubject;
            std::shared_ptr<Observer::ListSubject<std::string> > colorSpacesSubject;

            std::vector<Config> getUserConfigs() const;

            Config getCurrentConfig() const;

            std::string getDisplayName(int) const;
            std::string getViewName(int displayIndex, int) const;

            int getDisplayIndex(const std::string&) const;

            int addUserConfig(const Config&, bool init);
            void configUpdate();
        };

        void OCIOSystem::_init(const std::shared_ptr<System::Context>& context)
        {
            ISystem::_init("djv::OCIO::OCIOSystem", context);
            DJV_PRIVATE_PTR();

            addDependency(context->getSystemT<System::CoreSystem>());

            p.configModeSubject = Observer::ValueSubject<ConfigMode>::create();
            p.cmdLineConfigSubject = Observer::ValueSubject<Config>::create();
            p.envConfigSubject = Observer::ValueSubject<Config>::create();
            p.userConfigsSubject = Observer::ValueSubject<UserConfigs>::create();
            p.currentConfigSubject = Observer::ValueSubject<Config>::create();
            p.displaysSubject = Observer::ValueSubject<Displays>::create();
            p.viewsSubject = Observer::ValueSubject<Displays>::create();
            p.imageColorSpacesSubject = Observer::MapSubject<std::string, std::string>::create();
            p.colorSpacesSubject = Observer::ListSubject<std::string>::create();

            _OCIO::SetLoggingLevel(_OCIO::LOGGING_LEVEL_NONE);

            {
                std::stringstream ss;
                ss << "OCIO version: " << _OCIO::GetVersion();
                _log(ss.str());
            }
            
            std::string env;
            if (OS::getEnv("OCIO", env) && !env.empty())
            {
                p.hasEnvConfig = true;
                if (auto ocioConfig = _OCIO::Config::CreateFromEnv())
                {
                    Config config;
                    config.fileName = env;
                    config.name = OCIO::Config::getNameFromFileName(env);
                    const char* display = ocioConfig->getDefaultDisplay();
                    config.display = display;
                    config.view = ocioConfig->getDefaultView(display);
                    p.configMode = ConfigMode::Env;
                    p.envConfig = std::make_pair(ocioConfig, config);
                    p.configUpdate();
                }
            }
        }

        OCIOSystem::OCIOSystem() :
            _p(new Private(*this))
        {}

        OCIOSystem::~OCIOSystem()
        {}

        std::shared_ptr<OCIOSystem> OCIOSystem::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = context->getSystemT<OCIOSystem>();
            if (!out)
            {
                out = std::shared_ptr<OCIOSystem>(new OCIOSystem);
                out->_init(context);
            }
            return out;
        }

        bool OCIOSystem::hasEnvConfig() const
        {
            return _p->hasEnvConfig;
        }

        std::shared_ptr<Observer::IValueSubject<ConfigMode> > OCIOSystem::observeConfigMode() const
        {
            return _p->configModeSubject;
        }

        std::shared_ptr<Observer::IValueSubject<Config> > OCIOSystem::observeCmdLineConfig() const
        {
            return _p->cmdLineConfigSubject;
        }

        std::shared_ptr<Observer::IValueSubject<Config> > OCIOSystem::observeEnvConfig() const
        {
            return _p->envConfigSubject;
        }

        std::shared_ptr<Observer::IValueSubject<UserConfigs> > OCIOSystem::observeUserConfigs() const
        {
            return _p->userConfigsSubject;
        }

        std::shared_ptr<Observer::IValueSubject<Config> > OCIOSystem::observeCurrentConfig() const
        {
            return _p->currentConfigSubject;
        }

        std::shared_ptr<Observer::IValueSubject<Displays> > OCIOSystem::observeDisplays() const
        {
            return _p->displaysSubject;
        }

        std::shared_ptr<Observer::IValueSubject<Views> > OCIOSystem::observeViews() const
        {
            return _p->viewsSubject;
        }

        void OCIOSystem::setConfigMode(ConfigMode value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.configMode)
                return;
            p.configMode = value;
            p.configUpdate();
        }

        void OCIOSystem::setCmdLineConfig(const Config& config)
        {
            DJV_PRIVATE_PTR();
            try
            {
                _OCIO::ConstConfigRcPtr ocioConfig;
                if (config.isValid())
                {
                    ocioConfig = _OCIO::Config::CreateFromFile(config.fileName.c_str());
                }
                Config tmp = config;
                if (tmp.display.empty() && ocioConfig)
                {
                    tmp.display = ocioConfig->getDefaultDisplay();
                }
                if (tmp.view.empty() && ocioConfig)
                {
                    tmp.view = ocioConfig->getDefaultView(tmp.display.c_str());
                }
                p.cmdLineConfig = std::make_pair(ocioConfig, tmp);
                p.configUpdate();
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << e.what();
                _log(ss.str(), System::LogLevel::Error);
            }
        }

        void OCIOSystem::setEnvConfig(const Config& config)
        {
            DJV_PRIVATE_PTR();
            try
            {
                _OCIO::ConstConfigRcPtr ocioConfig;
                if (config.isValid())
                {
                    ocioConfig = _OCIO::Config::CreateFromFile(config.fileName.c_str());
                }
                p.envConfig = std::make_pair(ocioConfig, config);
                p.configUpdate();
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << e.what();
                _log(ss.str(), System::LogLevel::Error);
            }
        }

        int OCIOSystem::addUserConfig(const std::string& fileName)
        {
            DJV_PRIVATE_PTR();
            OCIO::Config config;
            config.fileName = fileName;
            config.name = OCIO::Config::getNameFromFileName(fileName);
            return p.addUserConfig(config, true);
        }

        int OCIOSystem::addUserConfig(const Config& config)
        {
            DJV_PRIVATE_PTR();
            return p.addUserConfig(config, false);
        }

        void OCIOSystem::removeUserConfig(int value)
        {
            DJV_PRIVATE_PTR();
            if (value >= 0 && value < static_cast<int>(p.userConfigs.size()))
            {
                p.userConfigs.erase(p.userConfigs.begin() + value);
            }
            const size_t size = p.userConfigs.size();
            if (p.currentUserConfig >= size)
            {
                p.currentUserConfig = static_cast<int>(size) - 1;
            }
            p.configUpdate();
        }

        std::shared_ptr<Observer::IMapSubject<std::string, std::string> > OCIOSystem::observeImageColorSpaces() const
        {
            return _p->imageColorSpacesSubject;
        }

        std::shared_ptr<Observer::IListSubject<std::string> > OCIOSystem::observeColorSpaces() const
        {
            return _p->colorSpacesSubject;
        }

        void OCIOSystem::setCurrentUserConfig(int value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.currentUserConfig)
                return;
            p.currentUserConfig = value;
            p.configUpdate();
        }

        void OCIOSystem::setCurrentDisplay(int value)
        {
            DJV_PRIVATE_PTR();
            const std::string displayName = p.getDisplayName(value);
            switch (p.configMode)
            {
            case ConfigMode::CmdLine:
                if (displayName != p.cmdLineConfig.second.display)
                {
                    p.cmdLineConfig.second.display = displayName;
                    p.configUpdate();
                }
                break;
            case ConfigMode::Env:
                if (displayName != p.envConfig.second.display)
                {
                    p.envConfig.second.display = displayName;
                    p.configUpdate();
                }
                break;
            case ConfigMode::User:
                if (p.currentUserConfig >= 0 &&
                    p.currentUserConfig < static_cast<int>(p.userConfigs.size()))
                {
                    if (displayName != p.userConfigs[p.currentUserConfig].second.display)
                    {
                        p.userConfigs[p.currentUserConfig].second.display = displayName;
                        p.configUpdate();
                    }
                }
                break;
            default: break;
            }
        }

        void OCIOSystem::setCurrentView(int value)
        {
            DJV_PRIVATE_PTR();
            switch (p.configMode)
            {
            case ConfigMode::CmdLine:
            {
                const int displayIndex = p.getDisplayIndex(p.cmdLineConfig.second.display);
                const std::string viewName = p.getViewName(displayIndex, value);
                if (viewName != p.cmdLineConfig.second.view)
                {
                    p.cmdLineConfig.second.view = viewName;
                    p.configUpdate();
                }
                break;
            }
            case ConfigMode::Env:
            {
                const int displayIndex = p.getDisplayIndex(p.envConfig.second.display);
                const std::string viewName = p.getViewName(displayIndex, value);
                if (viewName != p.envConfig.second.view)
                {
                    p.envConfig.second.view = viewName;
                    p.configUpdate();
                }
                break;
            }
            case ConfigMode::User:
                if (p.currentUserConfig >= 0 &&
                    p.currentUserConfig < static_cast<int>(p.userConfigs.size()))
                {
                    const int displayIndex = p.getDisplayIndex(p.userConfigs[p.currentUserConfig].second.display);
                    const std::string viewName = p.getViewName(displayIndex, value);
                    if (viewName != p.userConfigs[p.currentUserConfig].second.view)
                    {
                        p.userConfigs[p.currentUserConfig].second.view = viewName;
                        p.configUpdate();
                    }
                }
                break;
            default: break;
            }
        }

        void OCIOSystem::setImageColorSpaces(const ImageColorSpaces& value)
        {
            DJV_PRIVATE_PTR();
            switch (p.configMode)
            {
            case ConfigMode::CmdLine:
                p.cmdLineConfig.second.imageColorSpaces = value;
                p.configUpdate();
                break;
            case ConfigMode::Env:
                p.envConfig.second.imageColorSpaces = value;
                p.configUpdate();
                break;
            case ConfigMode::User:
                if (p.currentUserConfig >= 0 &&
                    p.currentUserConfig < static_cast<int>(p.userConfigs.size()) &&
                    value != p.userConfigs[p.currentUserConfig].second.imageColorSpaces)
                {
                    p.userConfigs[p.currentUserConfig].second.imageColorSpaces = value;
                    p.configUpdate();
                }
                break;
            default: break;
            }
        }

        std::string OCIOSystem::getColorSpace(const std::string& display, const std::string& view) const
        {
            for (const auto& i : _p->displays)
            {
                if (display == i.name)
                {
                    for (const auto& j : i.views)
                    {
                        if (view == j.name)
                        {
                            return j.colorSpace;
                        }
                    }
                }
            }
            return std::string();
        }

        std::vector<Config> OCIOSystem::Private::getUserConfigs() const
        {
            std::vector<Config> out;
            for (const auto& i : userConfigs)
            {
                out.push_back(i.second);
            }
            return out;
        }

        Config OCIOSystem::Private::getCurrentConfig() const
        {
            Config out;
            switch (configMode)
            {
            case ConfigMode::CmdLine:
                out = cmdLineConfig.second;
                break;
            case ConfigMode::Env:
                out = envConfig.second;
                break;
            case ConfigMode::User:
                if (currentUserConfig >= 0 && currentUserConfig < static_cast<int>(userConfigs.size()))
                {
                    out = userConfigs[currentUserConfig].second;
                }
                break;
            default: break;
            }
            return out;
        }

        std::string OCIOSystem::Private::getDisplayName(int value) const
        {
            std::string out;
            if (value >= 0 && value < static_cast<int>(displays.size()))
            {
                out = displays[value].name;
            }
            return out;
        }

        std::string OCIOSystem::Private::getViewName(int displayIndex, int value) const
        {
            std::string out;
            if (displayIndex >= 0 && displayIndex < static_cast<int>(displays.size()))
            {
                const auto& views = displays[displayIndex].views;
                if (value >= 0 && value < static_cast<int>(views.size()))
                {
                    out = views[value].name;
                }
            }
            return out;
        }

        int OCIOSystem::Private::getDisplayIndex(const std::string& value) const
        {
            const auto i = std::find_if(
                displays.begin(),
                displays.end(),
                [value](const Display& display)
                {
                    return value == display.name;
                });
            return i != displays.end() ? i - displays.begin() : -1;
        }

        int OCIOSystem::Private::addUserConfig(const Config& config, bool init)
        {
            int out = static_cast<int>(userConfigs.size());
            if (auto context = p.getContext().lock())
            {
                try
                {
                    Config tmp = config;
                    System::File::Info fileInfo(tmp.fileName);

                    // If the file doesn't exist try looking for it in the resource path.
                    if (!fileInfo.doesExist())
                    {
                        auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                        auto resourcePath = System::File::Path(resourceSystem->getPath(System::File::ResourcePath::Color), tmp.fileName);
                        System::File::DirectoryListOptions options;
                        options.filter = ".*\\.ocio$";
                        auto directoryList = System::File::directoryList(resourcePath, options);
                        if (directoryList.size())
                        {
                            fileInfo = directoryList[0];
                            tmp.fileName = fileInfo.getFileName();
                        }
                    }

                    // Add the configuration.
                    auto ocioConfig = _OCIO::Config::CreateFromFile(tmp.fileName.c_str());
                    if (init)
                    {
                        tmp.display = ocioConfig->getDefaultDisplay();
                        tmp.view = ocioConfig->getDefaultView(tmp.display.c_str());
                    }
                    userConfigs.push_back(std::make_pair(ocioConfig, tmp));

                    configUpdate();
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << e.what();
                    p._log(ss.str(), System::LogLevel::Error);
                }
            }
            return out;
        }

        void OCIOSystem::Private::configUpdate()
        {
            displays.clear();
            colorSpaces.clear();

            _OCIO::ConstConfigRcPtr ocioConfig;
            std::string displayName;
            std::string viewName;
            switch (configMode)
            {
            case ConfigMode::CmdLine:
                ocioConfig = cmdLineConfig.first;
                displayName = cmdLineConfig.second.display;
                viewName = cmdLineConfig.second.view;
                break;
            case ConfigMode::Env:
                ocioConfig = envConfig.first;
                displayName = envConfig.second.display;
                viewName = envConfig.second.view;
                break;
            case ConfigMode::User:
                if (currentUserConfig >= 0 &&
                    currentUserConfig < static_cast<int>(userConfigs.size()))
                {
                    const auto& config = userConfigs[currentUserConfig];
                    ocioConfig = config.first;
                    displayName = config.second.display;
                    viewName = config.second.view;
                }
                break;
            default: break;
            }

            if (ocioConfig)
            {
                try
                {
                    _OCIO::SetCurrentConfig(ocioConfig);

                    colorSpaces.push_back(std::string());
                    for (int i = 0; i < ocioConfig->getNumColorSpaces(); ++i)
                    {
                        const char* colorSpace = ocioConfig->getColorSpaceNameByIndex(i);
                        colorSpaces.push_back(colorSpace);
                    }

                    displays.push_back(Display());
                    for (int i = 0; i < ocioConfig->getNumDisplays(); ++i)
                    {
                        const char* displayName = ocioConfig->getDisplay(i);
                        Display display;
                        display.name = displayName;
                        display.defaultView = ocioConfig->getDefaultView(displayName);
                        for (int j = 0; j < ocioConfig->getNumViews(displayName); ++j)
                        {
                            const char* viewName = ocioConfig->getView(displayName, j);
                            View view;
                            view.name = viewName;
                            view.colorSpace = ocioConfig->getDisplayColorSpaceName(displayName, viewName);
                            view.looks = ocioConfig->getDisplayLooks(displayName, viewName);
                            display.views.push_back(view);
                        }
                        displays.push_back(display);
                    }
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << e.what();
                    p._log(ss.str(), System::LogLevel::Error);
                }
            }

            const auto& currentConfig = getCurrentConfig();
            Displays displaysTmp;
            Views viewsTmp;
            for (size_t i = 0; i < displays.size(); ++i)
            {
                const auto& display = displays[i];
                displaysTmp.first.push_back(display.name);
                if (display.name == currentConfig.display)
                {
                    displaysTmp.second = static_cast<int>(i);
                    for (size_t j = 0; j < display.views.size(); ++j)
                    {
                        const auto& view = display.views[j];
                        viewsTmp.first.push_back(view.name);
                        if (view.name == currentConfig.view)
                        {
                            viewsTmp.second = static_cast<int>(j);
                        }
                    }
                }
            }

            configModeSubject->setIfChanged(configMode);
            userConfigsSubject->setIfChanged(std::make_pair(getUserConfigs(), currentUserConfig));
            envConfigSubject->setIfChanged(envConfig.second);
            cmdLineConfigSubject->setIfChanged(cmdLineConfig.second);
            currentConfigSubject->setIfChanged(currentConfig);
            displaysSubject->setIfChanged(displaysTmp);
            viewsSubject->setIfChanged(viewsTmp);
            imageColorSpacesSubject->setIfChanged(currentConfig.imageColorSpaces);
            colorSpacesSubject->setIfChanged(colorSpaces);
        }

    } // namespace OCIO
} // namespace djv


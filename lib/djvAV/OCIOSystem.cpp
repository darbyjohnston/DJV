// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>

#include <OpenColorIO/OpenColorIO.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

using namespace djv::Core;
namespace _OCIO = OCIO_NAMESPACE;

namespace djv
{
    namespace AV
    {
        namespace OCIO
        {
            std::string Config::getNameFromFileName(const std::string& value)
            {
                FileSystem::Path path(value);
                path.cdUp();
                return path.getFileName();
            }

            struct System::Private
            {
                Private(System& p) :
                    p(p)
                {}

                System& p;

                typedef std::pair<_OCIO::ConstConfigRcPtr, Config> ConfigPair;

                ConfigMode configMode = ConfigMode::First;
                ConfigPair cmdLineConfig;
                bool hasEnvConfig = false;
                ConfigPair envConfig;
                std::vector<ConfigPair> userConfigs;
                int currentUserConfig = -1;
                std::vector<Display> displays;
                std::vector<std::string> colorSpaces;

                std::shared_ptr<ValueSubject<ConfigMode> > configModeSubject;
                std::shared_ptr<ValueSubject<Config> > cmdLineConfigSubject;
                std::shared_ptr<ValueSubject<Config> > envConfigSubject;
                std::shared_ptr<ValueSubject<UserConfigs> > userConfigsSubject;
                std::shared_ptr<ValueSubject<Config> > currentConfigSubject;
                std::shared_ptr<ValueSubject<Displays> > displaysSubject;
                std::shared_ptr<ValueSubject<Views> > viewsSubject;
                std::shared_ptr<MapSubject<std::string, std::string> > imageColorSpacesSubject;
                std::shared_ptr<ListSubject<std::string> > colorSpacesSubject;

                std::vector<Config> getUserConfigs() const;

                Config getCurrentConfig() const;

                std::string getDisplayName(int) const;
                static std::vector<std::string> getViews(const std::string& display, const std::vector<Display>&);
                std::string getViewName(int displayIndex, int) const;

                int getDisplayIndex(const std::string&) const;

                int addUserConfig(const Config&, bool init);
                void configUpdate();
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::OCIO::System", context);
                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.configModeSubject = ValueSubject<ConfigMode>::create();
                p.cmdLineConfigSubject = ValueSubject<Config>::create();
                p.envConfigSubject = ValueSubject<Config>::create();
                p.userConfigsSubject = ValueSubject<UserConfigs>::create();
                p.currentConfigSubject = ValueSubject<Config>::create();
                p.displaysSubject = ValueSubject<Displays>::create();
                p.viewsSubject = ValueSubject<Displays>::create();
                p.imageColorSpacesSubject = MapSubject<std::string, std::string>::create();
                p.colorSpacesSubject = ListSubject<std::string>::create();

                _OCIO::SetLoggingLevel(_OCIO::LOGGING_LEVEL_NONE);

                {
                    std::stringstream ss;
                    ss << "OCIO version: " << _OCIO::GetVersion();
                    _log(ss.str());
                }
                
                std::string env = OS::getEnv("OCIO");
                if (!env.empty())
                {
                    p.hasEnvConfig = true;
                    if (auto ocioConfig = _OCIO::Config::CreateFromEnv())
                    {
                        Config config;
                        config.fileName = env;
                        config.name = AV::OCIO::Config::getNameFromFileName(env);
                        const char* display = ocioConfig->getDefaultDisplay();
                        config.display = display;
                        config.view = ocioConfig->getDefaultView(display);
                        p.configMode = ConfigMode::Env;
                        p.envConfig = std::make_pair(ocioConfig, config);
                        p.configUpdate();
                    }
                }
            }

            System::System() :
                _p(new Private(*this))
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            bool System::hasEnvConfig() const
            {
                return _p->hasEnvConfig;
            }

            std::shared_ptr<IValueSubject<ConfigMode> > System::observeConfigMode() const
            {
                return _p->configModeSubject;
            }

            std::shared_ptr<IValueSubject<Config> > System::observeCmdLineConfig() const
            {
                return _p->cmdLineConfigSubject;
            }

            std::shared_ptr<IValueSubject<Config> > System::observeEnvConfig() const
            {
                return _p->envConfigSubject;
            }

            std::shared_ptr<IValueSubject<UserConfigs> > System::observeUserConfigs() const
            {
                return _p->userConfigsSubject;
            }

            std::shared_ptr<IValueSubject<Config> > System::observeCurrentConfig() const
            {
                return _p->currentConfigSubject;
            }

            std::shared_ptr<IValueSubject<Displays> > System::observeDisplays() const
            {
                return _p->displaysSubject;
            }

            std::shared_ptr<IValueSubject<Views> > System::observeViews() const
            {
                return _p->viewsSubject;
            }

            void System::setConfigMode(ConfigMode value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.configMode)
                    return;
                p.configMode = value;
                p.configUpdate();
            }

            void System::setCmdLineConfig(const Config& config)
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
                    _log(ss.str(), LogLevel::Error);
                }
            }

            void System::setEnvConfig(const Config& config)
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
                    _log(ss.str(), LogLevel::Error);
                }
            }


            int System::addUserConfig(const std::string& fileName)
            {
                DJV_PRIVATE_PTR();
                AV::OCIO::Config config;
                config.fileName = fileName;
                config.name = AV::OCIO::Config::getNameFromFileName(fileName);
                return p.addUserConfig(config, true);
            }

            int System::addUserConfig(const Config& config)
            {
                DJV_PRIVATE_PTR();
                return p.addUserConfig(config, false);
            }

            void System::removeUserConfig(int value)
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

            std::shared_ptr<IMapSubject<std::string, std::string> > System::observeImageColorSpaces() const
            {
                return _p->imageColorSpacesSubject;
            }

            std::shared_ptr<IListSubject<std::string> > System::observeColorSpaces() const
            {
                return _p->colorSpacesSubject;
            }

            void System::setCurrentUserConfig(int value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.currentUserConfig)
                    return;
                p.currentUserConfig = value;
                p.configUpdate();
            }

            void System::setCurrentDisplay(int value)
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

            void System::setCurrentView(int value)
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

            void System::setImageColorSpaces(const ImageColorSpaces& value)
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

            std::string System::getColorSpace(const std::string& display, const std::string& view) const
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

            std::vector<Config> System::Private::getUserConfigs() const
            {
                std::vector<Config> out;
                for (const auto& i : userConfigs)
                {
                    out.push_back(i.second);
                }
                return out;
            }

            Config System::Private::getCurrentConfig() const
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

            std::string System::Private::getDisplayName(int value) const
            {
                std::string out;
                if (value >= 0 && value < static_cast<int>(displays.size()))
                {
                    out = displays[value].name;
                }
                return out;
            }

            std::vector<std::string> System::Private::getViews(const std::string& display, const std::vector<Display>& displays)
            {
                std::vector<std::string> out;
                for (const auto& i : displays)
                {
                    if (display == i.name)
                    {
                        for (const auto& j : i.views)
                        {
                            out.push_back(j.name);
                        }
                        break;
                    }
                }
                return out;
            }

            std::string System::Private::getViewName(int displayIndex, int value) const
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

            int System::Private::getDisplayIndex(const std::string& value) const
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

            int System::Private::addUserConfig(const Config& config, bool init)
            {
                int out = static_cast<int>(userConfigs.size());
                if (auto context = p.getContext().lock())
                {
                    try
                    {
                        Config tmp = config;
                        FileSystem::FileInfo fileInfo(tmp.fileName);

                        // If the file doesn't exist try looking for it in the resource path.
                        if (!fileInfo.doesExist())
                        {
                            auto resourceSystem = context->getSystemT<ResourceSystem>();
                            FileSystem::Path resourcePath = FileSystem::Path(resourceSystem->getPath(FileSystem::ResourcePath::Color), tmp.fileName);
                            FileSystem::DirectoryListOptions options;
                            options.filter = ".*\\.ocio$";
                            auto directoryList = FileSystem::FileInfo::directoryList(resourcePath, options);
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
                        p._log(ss.str(), LogLevel::Error);
                    }
                }
                return out;
            }

            void System::Private::configUpdate()
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
                        p._log(ss.str(), LogLevel::Error);
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
    } // namespace AV
   
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::OCIO,
        ConfigMode,
        DJV_TEXT("av_ocio_config_mode_none"),
        DJV_TEXT("av_ocio_config_mode_cmd_line"),
        DJV_TEXT("av_ocio_config_mode_env"),
        DJV_TEXT("av_ocio_config_mode_user"));

    rapidjson::Value toJSON(AV::OCIO::ConfigMode value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    rapidjson::Value toJSON(const AV::OCIO::Config& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("FileName", toJSON(value.fileName, allocator), allocator);
        out.AddMember("Name", toJSON(value.name, allocator), allocator);
        out.AddMember("Display", toJSON(value.display, allocator), allocator);
        out.AddMember("View", toJSON(value.view, allocator), allocator);
        out.AddMember("ImageColorSpaces", toJSON(value.imageColorSpaces, allocator), allocator);
        return out;
    }

    void fromJSON(const rapidjson::Value& value, AV::OCIO::ConfigMode& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

    void fromJSON(const rapidjson::Value& value, AV::OCIO::Config& out)
    {
        if (value.IsObject())
        {
            for (const auto& i : value.GetObject())
            {
                if (0 == strcmp("FileName", i.name.GetString()))
                {
                    fromJSON(i.value, out.fileName);
                }
                else if (0 == strcmp("Name", i.name.GetString()))
                {
                    fromJSON(i.value, out.name);
                }
                else if (0 == strcmp("Display", i.name.GetString()))
                {
                    fromJSON(i.value, out.display);
                }
                else if (0 == strcmp("View", i.name.GetString()))
                {
                    fromJSON(i.value, out.view);
                }
                else if (0 == strcmp("ImageColorSpaces", i.name.GetString()))
                {
                    fromJSON(i.value, out.imageColorSpaces);
                }
            }
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv


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
            Config::Config()
            {}
            
            std::string Config::getNameFromFileName(const std::string& value)
            {
                FileSystem::Path path(value);
                path.cdUp();
                return path.getFileName();
            }

            bool Config::operator == (const Config& other) const
            {
                return
                    type == other.type &&
                    fileName == other.fileName &&
                    name == other.name &&
                    display == other.display &&
                    view == other.view &&
                    fileColorSpaces == other.fileColorSpaces;
            }

            bool Config::operator != (const Config& other) const
            {
                return !(*this == other);
            }

            bool ConfigData::operator == (const ConfigData& other) const
            {
                return
                    fileNames == other.fileNames &&
                    names == other.names &&
                    current == other.current;
            }

            bool DisplayData::operator == (const DisplayData& other) const
            {
                return
                    names == other.names &&
                    current == other.current;
            }

            bool ViewData::operator == (const ViewData& other) const
            {
                return
                    names == other.names &&
                    current == other.current;
            }

            struct System::Private
            {
                std::vector<_OCIO::ConstConfigRcPtr> ocioConfigs;
                std::vector<Config> configs;
                Config currentConfig;
                std::vector<Display> displays;

                std::shared_ptr<ListSubject<Config> > configsSubject;
                std::shared_ptr<ValueSubject<Config> > currentConfigSubject;
                std::shared_ptr<ValueSubject<ConfigData> > configDataSubject;
                std::shared_ptr<ValueSubject<DisplayData> > displayDataSubject;
                std::shared_ptr<ValueSubject<ViewData> > viewDataSubject;
                std::shared_ptr<ListSubject<std::string> > colorSpacesSubject;
                std::shared_ptr<MapSubject<std::string, std::string> > fileColorSpacesSubject;

                std::string getDisplayName(int) const;
                std::string getViewName(int) const;

                int getConfigIndex(const std::string&) const;
                int getDisplayIndex(const std::string&) const;
                int getViewIndex(int displayIndex, const std::string&) const;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::AV::OCIO::System", context);
                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.configsSubject = ListSubject<AV::OCIO::Config>::create();
                p.currentConfigSubject = ValueSubject<Config>::create();
                p.configDataSubject = ValueSubject<ConfigData>::create();
                p.displayDataSubject = ValueSubject<DisplayData>::create();
                p.viewDataSubject = ValueSubject<ViewData>::create();
                p.colorSpacesSubject = ListSubject<std::string>::create();
                p.fileColorSpacesSubject = MapSubject<std::string, std::string>::create();

                _OCIO::SetLoggingLevel(_OCIO::LOGGING_LEVEL_NONE);

                {
                    std::stringstream ss;
                    ss << "OCIO version: " << _OCIO::GetVersion();
                    _log(ss.str());
                }
                
                std::string env = OS::getEnv("OCIO");
                if (!env.empty())
                {
                    if (auto ocioConfig = _OCIO::Config::CreateFromEnv())
                    {
                        Config config;
                        config.type = ConfigType::Env;
                        config.name = AV::OCIO::Config::getNameFromFileName(env);
                        config.fileName = env;
                        p.ocioConfigs.push_back(ocioConfig);
                        p.configs.push_back(config);
                        p.currentConfig = config;
                        _configUpdate();
                        p.configsSubject->setIfChanged(p.configs);
                        p.currentConfigSubject->setIfChanged(config);
                        _dataUpdate();
                    }
                }
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<Config> > System::observeConfigs() const
            {
                return _p->configsSubject;
            }

            std::shared_ptr<IValueSubject<Config> > System::observeCurrentConfig() const
            {
                return _p->currentConfigSubject;
            }

            int System::addConfig(const std::string& fileName)
            {
                AV::OCIO::Config config;
                config.fileName = fileName;
                config.name = AV::OCIO::Config::getNameFromFileName(fileName);
                return _addConfig(config, true);
            }

            int System::addConfig(const Config& config)
            {
                return _addConfig(config, false);
            }

            void System::removeConfig(int value)
            {
                DJV_PRIVATE_PTR();
                if (value >= 0 && value < static_cast<int>(p.configs.size()))
                {
                    p.ocioConfigs.erase(p.ocioConfigs.begin() + value);
                    p.configs.erase(p.configs.begin() + value);
                }
                const size_t size = p.configs.size();
                if (value >= size)
                {
                    value = static_cast<int>(size) - 1;
                }
                p.currentConfig =
                    value >= 0 && value < static_cast<int>(size) ?
                    p.configs[value] :
                    Config();
                _configUpdate();
                p.configsSubject->setIfChanged(p.configs);
                p.currentConfigSubject->setIfChanged(p.currentConfig);
                _dataUpdate();
            }

            std::shared_ptr<IValueSubject<ConfigData> > System::observeConfigData() const
            {
                return _p->configDataSubject;
            }

            std::shared_ptr<IValueSubject<DisplayData> > System::observeDisplayData() const
            {
                return _p->displayDataSubject;
            }

            std::shared_ptr<IValueSubject<ViewData> > System::observeViewData() const
            {
                return _p->viewDataSubject;
            }

            std::shared_ptr<IListSubject<std::string> > System::observeColorSpaces() const
            {
                return _p->colorSpacesSubject;
            }

            std::shared_ptr<IMapSubject<std::string, std::string> > System::observeFileColorSpaces() const
            {
                return _p->fileColorSpacesSubject;
            }

            void System::setCurrentConfig(int value)
            {
                DJV_PRIVATE_PTR();
                const Config config = value >= 0 && value < static_cast<int>(p.configs.size()) ?
                    p.configs[value] :
                    Config();
                if (config == p.currentConfig)
                    return;
                p.currentConfig = config;
                _configUpdate();
                p.currentConfigSubject->setIfChanged(config);
                _dataUpdate();
            }

            void System::setCurrentDisplay(int value)
            {
                DJV_PRIVATE_PTR();
                Config config = p.currentConfig;
                config.display = p.getDisplayName(value);
                if (config == p.currentConfig)
                    return;
                p.currentConfig = config;
                _configUpdate();
                p.currentConfigSubject->setIfChanged(config);
                _dataUpdate();
            }

            void System::setCurrentView(int value)
            {
                DJV_PRIVATE_PTR();
                Config config = p.currentConfig;
                config.view = p.getViewName(value);
                if (config == p.currentConfig)
                    return;
                p.currentConfig = config;
                _configUpdate();
                p.currentConfigSubject->setIfChanged(config);
                _dataUpdate();
            }

            void System::setFileColorSpaces(const std::map<std::string, std::string>& value)
            {
                DJV_PRIVATE_PTR();
                Config config = p.currentConfig;
                config.fileColorSpaces = value;
                if (config == p.currentConfig)
                    return;
                p.currentConfig = config;
                _configUpdate();
                p.currentConfigSubject->setIfChanged(config);
                _dataUpdate();
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

            int System::_addConfig(const Config& config, bool init)
            {
                DJV_PRIVATE_PTR();
                int out = static_cast<int>(p.configs.size());
                if (auto context = getContext().lock())
                {
                    try
                    {
                        FileSystem::FileInfo fileInfo(config.fileName);

                        // If the file doesn't exist try looking for it in the resource path.
                        if (!fileInfo.doesExist())
                        {
                            auto resourceSystem = context->getSystemT<ResourceSystem>();
                            FileSystem::Path resourcePath = FileSystem::Path(resourceSystem->getPath(FileSystem::ResourcePath::Color), config.fileName);
                            FileSystem::DirectoryListOptions options;
                            options.filter = ".*\\.ocio$";
                            auto directoryList = FileSystem::FileInfo::directoryList(resourcePath, options);
                            if (directoryList.size())
                            {
                                fileInfo = directoryList[0];
                            }
                        }

                        // Remove any existing configurations with the same file name.
                        const std::string& fileName = fileInfo.getFileName();
                        auto i = p.configs.begin();
                        auto j = p.ocioConfigs.begin();
                        while (i != p.configs.end() && j != p.ocioConfigs.end())
                        {
                            if (i->fileName == fileName)
                            {
                                i = p.configs.erase(i);
                                j = p.ocioConfigs.erase(j);
                            }
                            else
                            {
                                ++i;
                                ++j;
                            }
                        }

                        // Add the configuration.
                        auto ocioConfig = _OCIO::Config::CreateFromFile(fileName.c_str());
                        p.ocioConfigs.push_back(ocioConfig);
                        Config tmp = config;
                        tmp.fileName = fileName;
                        if (init)
                        {
                            tmp.display = ocioConfig->getDefaultDisplay();
                            tmp.view = ocioConfig->getDefaultView(tmp.display.c_str());
                        }
                        p.configs.push_back(tmp);
                        _configUpdate();
                        p.configsSubject->setIfChanged(p.configs);
                        _dataUpdate();
                    }
                    catch (const std::exception& e)
                    {
                        std::stringstream ss;
                        ss << e.what();
                        _log(ss.str(), LogLevel::Error);
                    }
                }
                return out;
            }

            namespace
            {
                std::vector<std::string> getViews(const std::string& display, const std::vector<Display>& displays)
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
            } // namespace

            void System::_configUpdate()
            {
                DJV_PRIVATE_PTR();

                DJV_ASSERT(p.ocioConfigs.size() == p.configs.size());

                int configIndex = -1;
                p.displays.clear();
                std::vector<std::string> colorSpaces;
                try
                {
                    configIndex = p.getConfigIndex(p.currentConfig.name);
                    if (configIndex >= 0 && configIndex < p.ocioConfigs.size())
                    {
                        const auto ocioConfig = p.ocioConfigs[configIndex];
                        _OCIO::SetCurrentConfig(ocioConfig);

                        colorSpaces.push_back(std::string());
                        for (int i = 0; i < ocioConfig->getNumColorSpaces(); ++i)
                        {
                            const char* colorSpace = ocioConfig->getColorSpaceNameByIndex(i);
                            colorSpaces.push_back(colorSpace);
                        }

                        p.displays.push_back(Display());
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
                            p.displays.push_back(display);
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << e.what();
                    _log(ss.str(), LogLevel::Error);
                }

                p.colorSpacesSubject->setIfChanged(colorSpaces);
            }

            void System::_dataUpdate()
            {
                DJV_PRIVATE_PTR();

                ConfigData configData;
                DisplayData displayData;
                ViewData viewData;
                for (const auto& config : p.configs)
                {
                    configData.names.push_back(config.name);
                    configData.fileNames.push_back(config.fileName);
                }
                for (const auto& display : p.displays)
                {
                    displayData.names.push_back(display.name);
                    if (p.currentConfig.display == display.name)
                    {
                        for (const auto& view : display.views)
                        {
                            viewData.names.push_back(view.name);
                        }
                    }
                }
                DJV_ASSERT(p.displays.size() == displayData.names.size());

                configData.current = p.getConfigIndex(p.currentConfig.name);
                displayData.current = p.getDisplayIndex(p.currentConfig.display);
                viewData.current = p.getViewIndex(displayData.current, p.currentConfig.view);

                p.configDataSubject->setIfChanged(configData);
                p.displayDataSubject->setIfChanged(displayData);
                p.viewDataSubject->setIfChanged(viewData);
                p.fileColorSpacesSubject->setIfChanged(p.currentConfig.fileColorSpaces);
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

            std::string System::Private::getViewName(int value) const
            {
                std::string out;
                const int displayIndex = getDisplayIndex(currentConfig.display);
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

            int System::Private::getConfigIndex(const std::string& value) const
            {
                const auto i = std::find_if(
                    configs.begin(),
                    configs.end(),
                    [value](const Config& config)
                    {
                        return value == config.name;
                    });
                return i != configs.end() ? i - configs.begin() : -1;
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

            int System::Private::getViewIndex(int displayIndex, const std::string& value) const
            {
                int out = -1;
                if (displayIndex >= 0 && displayIndex < static_cast<int>(displays.size()))
                {
                    const auto& display = displays[displayIndex];
                    const auto i = std::find_if(
                        display.views.begin(),
                        display.views.end(),
                        [value](const View& view)
                        {
                            return value == view.name;
                        });
                    if (i != display.views.end())
                    {
                        out = i - display.views.begin();
                    }
                }
                return out;
            }

        } // namespace OCIO
    } // namespace AV

    rapidjson::Value toJSON(const AV::OCIO::Config& value, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        out.AddMember("FileName", toJSON(value.fileName, allocator), allocator);
        out.AddMember("Name", toJSON(value.name, allocator), allocator);
        out.AddMember("Display", toJSON(value.display, allocator), allocator);
        out.AddMember("View", toJSON(value.view, allocator), allocator);
        out.AddMember("FileColorSpaces", toJSON(value.fileColorSpaces, allocator), allocator);
        return out;
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
                else if (0 == strcmp("FileColorSpaces", i.name.GetString()))
                {
                    fromJSON(i.value, out.fileColorSpaces);
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


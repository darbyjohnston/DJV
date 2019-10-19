//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/CoreSystem.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/ResourceSystem.h>

#include <OpenColorIO/OpenColorIO.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
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
                Core::FileSystem::Path path(value);
                path.cdUp();
                return path.getFileName();
            }

            bool Config::operator == (const Config& value) const
            {
                return
                    fileName == value.fileName &&
                    name == value.name &&
                    colorSpaces == value.colorSpaces &&
                    display == value.display &&
                    view == value.view;
            }

            struct System::Private
            {
                std::vector<_OCIO::ConstConfigRcPtr> ocioConfigs;
                std::vector<Config> configs;
                Config currentConfig;
                int currentIndex = -1;
                std::shared_ptr<ListSubject<Config> > configsSubject;
                std::shared_ptr<ValueSubject<Config> > currentConfigSubject;
                std::shared_ptr<ValueSubject<int> > currentIndexSubject;
                std::shared_ptr<ListSubject<std::string> > colorSpacesSubject;
                std::shared_ptr<ListSubject<Display> > displaysSubject;
                std::shared_ptr<ListSubject<std::string> > viewsSubject;
            };

            void System::_init(const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::AV::OCIO::System", context);
                DJV_PRIVATE_PTR();

                addDependency(context->getSystemT<CoreSystem>());

                p.configsSubject = ListSubject<AV::OCIO::Config>::create();
                p.currentConfigSubject = ValueSubject<AV::OCIO::Config>::create();
                p.currentIndexSubject = ValueSubject<int>::create(p.currentIndex);
                p.colorSpacesSubject = ListSubject<std::string>::create();
                p.displaysSubject = ListSubject<Display>::create();
                p.viewsSubject = ListSubject<std::string>::create();

                _OCIO::SetLoggingLevel(_OCIO::LOGGING_LEVEL_NONE);

                {
                    std::stringstream ss;
                    ss << "OCIO version: " << _OCIO::GetVersion();
                    _log(ss.str());
                }
                
                _configUpdate();

                std::string env = OS::getEnv("OCIO");
                if (!env.empty())
                {
                    if (auto ocioConfig = _OCIO::Config::CreateFromEnv())
                    {
                        Config config;
                        config.fileName = env;
                        config.name = AV::OCIO::Config::getNameFromFileName(env);
                        p.ocioConfigs.push_back(ocioConfig);
                        p.configs.push_back(config);
                        p.configsSubject->setIfChanged(p.configs);
                        setCurrentIndex(0);
                    }
                }
            }

            System::System() :
                _p(new Private)
            {}

            System::~System()
            {}

            std::shared_ptr<System> System::create(const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Core::IListSubject<Config> > System::observeConfigs() const
            {
                return _p->configsSubject;
            }

            int System::addConfig(const Config& config)
            {
                DJV_PRIVATE_PTR();
                int out = -1;
                if (auto context = getContext().lock())
                {
                    try
                    {
                        FileSystem::FileInfo fileInfo(config.fileName);
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

                        auto ocioConfig = _OCIO::Config::CreateFromFile(fileName.c_str());
                        p.ocioConfigs.push_back(ocioConfig);
                        out = static_cast<int>(p.configs.size());
                        Config tmp = config;
                        tmp.fileName = fileName;
                        p.configs.push_back(tmp);
                        p.configsSubject->setIfChanged(p.configs);
                        setCurrentIndex(out);
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

            void System::removeConfig(int value)
            {
                DJV_PRIVATE_PTR();
                if (value >= 0 && value < p.configs.size())
                {
                    p.ocioConfigs.erase(p.ocioConfigs.begin() + value);
                    p.configs.erase(p.configs.begin() + value);
                }
                p.configsSubject->setIfChanged(p.configs);
                const size_t size = p.configs.size();
                if (value >= size)
                {
                    value = static_cast<int>(size) - 1;
                }
                Config config;
                if (value >= 0 && value < p.configs.size())
                {
                    config = p.configs[value];
                }
                p.currentConfig = config;
                p.currentConfigSubject->setIfChanged(p.currentConfig);
                p.currentIndex = value;
                p.currentIndexSubject->setIfChanged(value);
                _configUpdate();
            }

            std::shared_ptr<Core::IValueSubject<Config> > System::observeCurrentConfig() const
            {
                return _p->currentConfigSubject;
            }

            std::shared_ptr<Core::IValueSubject<int> > System::observeCurrentIndex() const
            {
                return _p->currentIndexSubject;
            }

            std::shared_ptr<Core::IListSubject<std::string> > System::observeColorSpaces() const
            {
                return _p->colorSpacesSubject;
            }

            std::shared_ptr<Core::IListSubject<Display> > System::observeDisplays() const
            {
                return _p->displaysSubject;
            }

            std::shared_ptr<Core::IListSubject<std::string> > System::observeViews() const
            {
                return _p->viewsSubject;
            }

            void System::setCurrentConfig(const Config& value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.currentConfig)
                    return;
                const int index = p.currentIndex;
                Config config;
                if (index >= 0 && index < p.configs.size())
                {
                    p.configs[index] = value;
                    config = value;
                }
                p.currentConfig = config;
                _configUpdate();
                p.configsSubject->setIfChanged(p.configs);
                p.currentConfigSubject->setIfChanged(p.currentConfig);
            }

            void System::setCurrentIndex(int value)
            {
                DJV_PRIVATE_PTR();
                int tmp = -1;
                if (p.configs.size())
                {
                    tmp = std::min(value, static_cast<int>(p.configs.size()) - 1);
                }
                if (tmp == p.currentIndex)
                    return;
                p.currentIndex = tmp;
                Config config;
                if (tmp != -1)
                {
                    config = p.configs[tmp];
                }
                p.currentConfig = config;
                _configUpdate();
                p.currentConfigSubject->setIfChanged(p.currentConfig);
                p.currentIndexSubject->setIfChanged(p.currentIndex);
            }

            std::string System::getColorSpace(const std::string& display, const std::string& view) const
            {
                for (const auto& i : _p->displaysSubject->get())
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

                std::vector<std::string> colorSpaces;
                std::vector<Display> displays;
                std::vector<std::string> views;
                std::string defaultDisplay;
                std::string defaultView;

                _OCIO::ConstConfigRcPtr ocioConfig;
                int index = p.currentIndex;
                if (index >= 0 && index < p.ocioConfigs.size())
                {
                    ocioConfig = p.ocioConfigs[index];
                }
                try
                {
                    if (!ocioConfig)
                    {
                        ocioConfig = _OCIO::Config::Create();
                    }
                    _OCIO::SetCurrentConfig(ocioConfig);

                    colorSpaces.push_back(std::string());
                    for (int j = 0; j < ocioConfig->getNumColorSpaces(); ++j)
                    {
                        const char* colorSpace = ocioConfig->getColorSpaceNameByIndex(j);
                        colorSpaces.push_back(colorSpace);
                    }

                    defaultDisplay = ocioConfig->getDefaultDisplay();

                    displays.push_back(Display());
                    for (int j = 0; j < ocioConfig->getNumDisplays(); ++j)
                    {
                        const char* displayName = ocioConfig->getDisplay(j);
                        Display display;
                        display.name = displayName;
                        display.defaultView = ocioConfig->getDefaultView(displayName);
                        if (display.name == defaultDisplay)
                        {
                            defaultView = display.defaultView;
                        }
                        for (int k = 0; k < ocioConfig->getNumViews(displayName); ++k)
                        {
                            const char* viewName = ocioConfig->getView(displayName, k);
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
                    _log(ss.str(), LogLevel::Error);
                }

                p.colorSpacesSubject->setIfChanged(colorSpaces);
                p.displaysSubject->setIfChanged(displays);
                if (index >= 0 && index < p.configs.size())
                {
                    views = getViews(p.configs[index].display, displays);
                }
                if (p.viewsSubject->setIfChanged(views))
                {
                    size_t i = 0;
                    for (; i < views.size(); ++i)
                    {
                        if (p.currentConfig.view == views[i])
                        {
                            break;
                        }
                    }
                    if (i == views.size())
                    {
                        p.currentConfig.view = views.size() > 0 ? views[0] : std::string();
                    }
                    p.currentConfigSubject->setIfChanged(p.currentConfig);
                }
            }

        } // namespace OCIO
    } // namespace AV

    picojson::value toJSON(const AV::OCIO::Config& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["FileName"] = toJSON(value.fileName);
        out.get<picojson::object>()["Name"] = toJSON(value.name);
        out.get<picojson::object>()["ColorSpaces"] = toJSON(value.colorSpaces);
        out.get<picojson::object>()["Display"] = toJSON(value.display);
        out.get<picojson::object>()["View"] = toJSON(value.view);
        return out;
    }

    void fromJSON(const picojson::value& value, AV::OCIO::Config& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("FileName" == i.first)
                {
                    fromJSON(i.second, out.fileName);
                }
                else if ("Name" == i.first)
                {
                    fromJSON(i.second, out.name);
                }
                else if ("ColorSpaces" == i.first)
                {
                    fromJSON(i.second, out.colorSpaces);
                }
                else if ("Display" == i.first)
                {
                    fromJSON(i.second, out.display);
                }
                else if ("View" == i.first)
                {
                    fromJSON(i.second, out.view);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv


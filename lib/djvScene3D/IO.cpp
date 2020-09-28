// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/IO.h>

#include <djvScene3D/OBJ.h>
#if defined(OpenNURBS_FOUND)
#include <djvScene3D/OpenNURBS.h>
#endif // OpenNURBS_FOUND

#include <djvSystem/Context.h>
#include <djvSystem/File.h>
#include <djvSystem/LogSystem.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/StringFormat.h>
#include <djvCore/StringFunc.h>

#include <map>
#include <sstream>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        namespace IO
        {
            Info::Info()
            {}

            void IIO::_init(
                const System::File::Info& fileInfo,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                _logSystem = logSystem;
                _resourceSystem = resourceSystem;
                _textSystem = textSystem;
                _fileInfo = fileInfo;
            }

            IIO::~IIO()
            {}

            void IRead::_init(
                const System::File::Info & fileInfo,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem> & resourceSystem,
                const std::shared_ptr<System::LogSystem> & logSystem)
            {
                IIO::_init(fileInfo, textSystem, resourceSystem, logSystem);
            }

            IRead::~IRead()
            {}

            void IWrite::_init(
                const System::File::Info & fileInfo,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem> & resourceSystem,
                const std::shared_ptr<System::LogSystem> & logSystem)
            {
                IIO::_init(fileInfo, textSystem, resourceSystem, logSystem);
            }

            IWrite::~IWrite()
            {}

            void IPlugin::_init(
                const std::string & pluginName,
                const std::string & pluginInfo,
                const std::set<std::string> & fileExtensions,
                const std::shared_ptr<System::Context> & context)
            {
                _context = context;
                _logSystem = context->getSystemT<System::LogSystem>();
                _resourceSystem = context->getSystemT<System::ResourceSystem>();
                _textSystem = context->getSystemT<System::TextSystem>();
                _pluginName = pluginName;
                _pluginInfo = pluginInfo;
                _fileExtensions = fileExtensions;
            }

            IPlugin::~IPlugin()
            {}

            namespace
            {
                bool checkExtension(const std::string& value, const std::set<std::string>& extensions)
                {
                    std::string extension = System::File::Path(value).getExtension();
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
                }

            } // namespace

            bool IPlugin::canRead(const System::File::Info& fileInfo) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            bool IPlugin::canWrite(const System::File::Info& fileInfo) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            rapidjson::Value IPlugin::getOptions(rapidjson::Document::AllocatorType&) const
            {
                return rapidjson::Value();
            }

            void IPlugin::setOptions(const rapidjson::Value&)
            {
                // Default implementation does nothing.
            }

            std::shared_ptr<IRead> IPlugin::read(const System::File::Info&) const
            {
                return nullptr;
            }

            std::shared_ptr<IWrite> IPlugin::write(const System::File::Info&) const
            {
                return nullptr;
            }

            struct IOSystem::Private
            {
                std::shared_ptr<ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                std::set<std::string> sequenceExtensions;
            };

            void IOSystem::_init(const std::shared_ptr<System::Context>& context)
            {
                ISystem::_init("djv::Scene::IO::IOSystem", context);

                DJV_PRIVATE_PTR();

                p.optionsChanged = ValueSubject<bool>::create();

                p.plugins[OBJ::pluginName] = OBJ::Plugin::create(context);
#if defined(OpenNURBS_FOUND)
                p.plugins[OpenNURBS::pluginName] = OpenNURBS::Plugin::create(context);
#endif // OpenNURBS_FOUND

                for (const auto& i : p.plugins)
                {
                    std::stringstream ss;
                    ss << "I/O plugin: " << i.second->getPluginName() << '\n';
                    ss << "    Information: " << i.second->getPluginInfo() << '\n';
                    ss << "    File extensions: " << String::joinSet(i.second->getFileExtensions(), ", ") << '\n';
                    _log(ss.str());
                }
            }

            IOSystem::IOSystem() :
                _p(new Private)
            {}

            IOSystem::~IOSystem()
            {}

            std::shared_ptr<IOSystem> IOSystem::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<IOSystem>(new IOSystem);
                out->_init(context);
                return out;
            }

            std::set<std::string> IOSystem::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    out.insert(i.second->getPluginName());
                }
                return out;
            }

            std::set<std::string> IOSystem::getFileExtensions() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    const auto& fileExtensions = i.second->getFileExtensions();
                    out.insert(fileExtensions.begin(), fileExtensions.end());
                }
                return out;
            }

            rapidjson::Value IOSystem::getOptions(const std::string& pluginName, rapidjson::Document::AllocatorType& allocator) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    return i->second->getOptions(allocator);
                }
                return rapidjson::Value();
            }

            void IOSystem::setOptions(const std::string& pluginName, const rapidjson::Value& value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    i->second->setOptions(value);
                    p.optionsChanged->setAlways(true);
                }
            }

            std::shared_ptr<IValueSubject<bool> > IOSystem::observeOptionsChanged() const
            {
                return _p->optionsChanged;
            }

            const std::set<std::string>& IOSystem::getSequenceExtensions() const
            {
                return _p->sequenceExtensions;
            }

            bool IOSystem::canRead(const System::File::Info& fileInfo) const
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool IOSystem::canWrite(const System::File::Info& fileInfo, const Info& info) const
            {
                DJV_PRIVATE_PTR();
                for (const auto& i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo))
                    {
                        return true;
                    }
                }
                return false;
            }

            std::shared_ptr<IRead> IOSystem::read(const System::File::Info& fileInfo)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IRead> out;
                for (const auto& i : p.plugins)
                {
                    if (i.second->canRead(fileInfo))
                    {
                        out = i.second->read(fileInfo);
                        break;
                    }
                }
                if (!out)
                {
                    if (auto context = getContext().lock())
                    {
                        auto textSystem = context->getSystemT<System::TextSystem>();
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_read"))));
                    }
                }
                return out;
            }

            std::shared_ptr<IWrite> IOSystem::write(const System::File::Info& fileInfo)
            {
                DJV_PRIVATE_PTR();
                std::shared_ptr<IWrite> out;
                for (const auto& i : p.plugins)
                {
                    if (i.second->canWrite(fileInfo))
                    {
                        out = i.second->write(fileInfo);
                        break;
                    }
                }
                if (!out)
                {
                    if (auto context = getContext().lock())
                    {
                        auto textSystem = context->getSystemT<System::TextSystem>();
                        throw System::File::Error(String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_write"))));
                    }
                }
                return out;
            }

        } // namespace IO
    } // namespace Scene3D
} // namespace djv


//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvScene/IO.h>

#include <djvScene/OBJ.h>
#if defined(OpenNURBS_FOUND)
#include <djvScene/OpenNURBS.h>
#endif // OpenNURBS_FOUND

#include <djvCore/Context.h>
#include <djvCore/FileSystem.h>
#include <djvCore/LogSystem.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/StringFormat.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            void IIO::_init(
                const FileSystem::FileInfo& fileInfo,
                const std::shared_ptr<Core::TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem,
                const std::shared_ptr<LogSystem>& logSystem)
            {
                _logSystem = logSystem;
                _resourceSystem = resourceSystem;
                _textSystem = textSystem;
                _fileInfo = fileInfo;
            }

            IIO::~IIO()
            {}

            void IRead::_init(
                const FileSystem::FileInfo & fileInfo,
                const std::shared_ptr<Core::TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem> & resourceSystem,
                const std::shared_ptr<LogSystem> & logSystem)
            {
                IIO::_init(fileInfo, textSystem, resourceSystem, logSystem);
            }

            IRead::~IRead()
            {}

            void IWrite::_init(
                const FileSystem::FileInfo & fileInfo,
                const std::shared_ptr<Core::TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem> & resourceSystem,
                const std::shared_ptr<LogSystem> & logSystem)
            {
                IIO::_init(fileInfo, textSystem, resourceSystem, logSystem);
            }

            IWrite::~IWrite()
            {}

            void IPlugin::_init(
                const std::string & pluginName,
                const std::string & pluginInfo,
                const std::set<std::string> & fileExtensions,
                const std::shared_ptr<Context> & context)
            {
                _context = context;
                _logSystem = context->getSystemT<LogSystem>();
                _resourceSystem = context->getSystemT<ResourceSystem>();
                _textSystem = context->getSystemT<TextSystem>();
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
                    std::string extension = FileSystem::Path(value).getExtension();
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
                }

            } // namespace

            bool IPlugin::canRead(const FileSystem::FileInfo& fileInfo) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            bool IPlugin::canWrite(const FileSystem::FileInfo& fileInfo) const
            {
                return checkExtension(std::string(fileInfo), _fileExtensions);
            }

            picojson::value IPlugin::getOptions() const
            {
                return picojson::value(std::string());
            }

            void IPlugin::setOptions(const picojson::value&)
            {
                // Default implementation does nothing.
            }

            std::shared_ptr<IRead> IPlugin::read(const FileSystem::FileInfo&) const
            {
                return nullptr;
            }

            std::shared_ptr<IWrite> IPlugin::write(const FileSystem::FileInfo&) const
            {
                return nullptr;
            }

            struct System::Private
            {
                std::shared_ptr<ValueSubject<bool> > optionsChanged;
                std::map<std::string, std::shared_ptr<IPlugin> > plugins;
                std::set<std::string> sequenceExtensions;
            };

            void System::_init(const std::shared_ptr<Context>& context)
            {
                ISystem::_init("djv::Scene::IO::System", context);

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

            std::set<std::string> System::getPluginNames() const
            {
                DJV_PRIVATE_PTR();
                std::set<std::string> out;
                for (const auto& i : p.plugins)
                {
                    out.insert(i.second->getPluginName());
                }
                return out;
            }

            std::set<std::string> System::getFileExtensions() const
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

            picojson::value System::getOptions(const std::string& pluginName) const
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    return i->second->getOptions();
                }
                return picojson::value();
            }

            void System::setOptions(const std::string& pluginName, const picojson::value& value)
            {
                DJV_PRIVATE_PTR();
                const auto i = p.plugins.find(pluginName);
                if (i != p.plugins.end())
                {
                    i->second->setOptions(value);
                    p.optionsChanged->setAlways(true);
                }
            }

            std::shared_ptr<IValueSubject<bool> > System::observeOptionsChanged() const
            {
                return _p->optionsChanged;
            }

            const std::set<std::string>& System::getSequenceExtensions() const
            {
                return _p->sequenceExtensions;
            }

            bool System::canRead(const FileSystem::FileInfo& fileInfo) const
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

            bool System::canWrite(const FileSystem::FileInfo& fileInfo, const Info& info) const
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

            std::shared_ptr<IRead> System::read(const FileSystem::FileInfo& fileInfo)
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
                        auto textSystem = context->getSystemT<TextSystem>();
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_read"))));
                    }
                }
                return out;
            }

            std::shared_ptr<IWrite> System::write(const FileSystem::FileInfo& fileInfo)
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
                        auto textSystem = context->getSystemT<TextSystem>();
                        throw FileSystem::Error(String::Format("{0}: {1}").
                            arg(fileInfo.getFileName()).
                            arg(textSystem->getText(DJV_TEXT("error_file_write"))));
                    }
                }
                return out;
            }

        } // namespace IO
    } // namespace Scene
} // namespace djv


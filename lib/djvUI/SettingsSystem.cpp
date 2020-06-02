// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/SettingsSystem.h>

#include <djvUI/ISettings.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>
#include <djvCore/ResourceSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            namespace
            {
                const size_t settingsVersion = 30;

            } // namespace

            void System::_init(bool reset, const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::UI::Settings::System", context);
                
                //! \todo Is there a better way to disable settings for tests?
                if ("djvTest" == context->getName())
                {
                    _settingsIO = false;
                }

                addDependency(context->getSystemT<AV::AVSystem>());

                if (auto resourceSystem = context->getSystemT<ResourceSystem>())
                {
                    _settingsPath = resourceSystem->getPath(FileSystem::ResourcePath::SettingsFile);
                }

                if (!reset)
                {
                    _readSettingsFile(_settingsPath, _json);
                }
            }

            System::System()
            {}

            System::~System()
            {
                _saveSettings();
            }

            std::shared_ptr<System> System::create(bool reset, const std::shared_ptr<Core::Context>& context)
            {
                auto out = std::shared_ptr<System>(new System);
                out->_init(reset, context);
                return out;
            }

            void System::_addSettings(const std::shared_ptr<ISettings> & value)
            {
                _settings.push_back(value);
            }

            void System::_removeSettings(const std::shared_ptr<ISettings> & value)
            {
                const auto i = std::find(_settings.begin(), _settings.end(), value);
                if (i != _settings.end())
                {
                    _settings.erase(i);
                }
            }

            void System::_loadSettings(const std::shared_ptr<ISettings> & settings)
            {
                if (!_settingsIO)
                    return;

                std::stringstream ss;
                ss << "Loading settings: " << settings->getName();
                _log(ss.str());

                picojson::value json;
                const auto & name = settings->getName();
                auto i = _json.find(name);
                if (i != _json.end())
                {
                    try
                    {
                        settings->load(i->second);
                    }
                    catch (const std::exception & e)
                    {
                        std::stringstream ss;
                        ss << "Cannot read settings" << " '" << name << "'. " << e.what();
                        _log(ss.str(), LogLevel::Error);
                    }
                }
            }

            void System::_saveSettings()
            {
                if (!_settingsIO)
                    return;

                picojson::value object(picojson::object_type, true);
                object.get<picojson::object>()["SettingsVersion"] = toJSON(settingsVersion);

                // Serialize the settings.
                for (const auto & settings : _settings)
                {
                    object.get<picojson::object>()[settings->getName()] = settings->save();
                }

                // Write the JSON to disk.
                _writeSettingsFile(_settingsPath, object);
            }

            void System::_readSettingsFile(const FileSystem::Path & path, std::map<std::string, picojson::value> & out)
            {
                try
                {
                    if (FileSystem::FileInfo(path).doesExist())
                    {
                        std::stringstream ss;
                        ss << "Reading settings: " << path;
                        _log(ss.str());

                        auto fileIO = FileSystem::FileIO::create();
                        fileIO->open(std::string(path), FileSystem::FileIO::Mode::Read);
#if defined(DJV_MMAP)
                        const char * bufP = reinterpret_cast<const char *>(fileIO->mmapP());
                        const char * bufEnd = reinterpret_cast<const char *>(fileIO->mmapEnd());
#else // DJV_MMAP
                        std::vector<char> buf;
                        const size_t fileSize = fileIO->getSize();
                        buf.resize(fileSize);
                        fileIO->read(buf.data(), fileSize);
                        const char* bufP = buf.data();
                        const char* bufEnd = bufP + fileSize;
#endif // DJV_MMAP

                        picojson::value v;
                        std::string error;
                        picojson::parse(v, bufP, bufEnd, &error);
                        if (!error.empty())
                        {
                            throw std::invalid_argument(error);
                        }

                        if (v.is<picojson::object>())
                        {
                            const auto & object = v.get<picojson::object>();
                            size_t readSettingsVersion = 0;
                            for (const auto& value : object)
                            {
                                if ("SettingsVersion" == value.first)
                                {
                                    fromJSON(value.second, readSettingsVersion);
                                    break;
                                }
                            }
                            if (settingsVersion == readSettingsVersion)
                            {
                                for (const auto& value : object)
                                {
                                    out[value.first] = value.second;
                                }
                            }
                        }
                    }
                }
                catch (const std::exception & e)
                {
                    std::stringstream ss;
                    ss << "Cannot read settings" << " '" << path << "'. " << e.what();
                    _log(ss.str(), LogLevel::Error);
                }
            }

            void System::_writeSettingsFile(const FileSystem::Path & path, const picojson::value & value)
            {
                try
                {
                    std::stringstream ss;
                    ss << "Writing settings: " << path;
                    _log(ss.str());

                    auto fileIO = FileSystem::FileIO::create();
                    fileIO->open(std::string(path), FileSystem::FileIO::Mode::Write);
                    PicoJSON::write(value, fileIO);
                    fileIO->write("\n");
                }
                catch (const std::exception & e)
                {
                    std::stringstream ss;
                    ss << "Cannot write settings" << " '" << path << "'. " << e.what();
                    _log(ss.str(), LogLevel::Error);
                }
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

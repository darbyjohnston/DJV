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
#include <djvCore/StringFormat.h>

#include <rapidjson/error/en.h>
#include <rapidjson/prettywriter.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            namespace
            {
                const std::string settingsVersion = "37";

            } // namespace

            void System::_init(bool reset, const std::shared_ptr<Core::Context>& context)
            {
                ISystem::_init("djv::UI::Settings::System", context);
                
                //! \bug Is there a better way to disable settings for tests?
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
                    _readSettingsFile();
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

            void System::_addSettings(const std::shared_ptr<ISettings>& value)
            {
                _settings.push_back(value);
            }

            void System::_removeSettings(const std::shared_ptr<ISettings>& value)
            {
                const auto i = std::find(_settings.begin(), _settings.end(), value);
                if (i != _settings.end())
                {
                    _settings.erase(i);
                }
            }

            void System::_loadSettings(const std::shared_ptr<ISettings>& settings)
            {
                if (!_settingsIO)
                    return;

                std::stringstream ss;
                ss << "Loading settings: " << settings->getName();
                _log(ss.str());

                if (_document.IsObject())
                {
                    const auto& name = settings->getName();
                    auto i = _document.FindMember(name.c_str());
                    if (i != _document.MemberEnd())
                    {
                        try
                        {
                            settings->load(i->value);
                        }
                        catch (const std::exception& e)
                        {
                            std::stringstream ss;
                            ss << "Cannot read settings" << " '" << name << "': " << e.what();
                            _log(ss.str(), LogLevel::Error);
                        }
                    }
                }
            }

            void System::_saveSettings()
            {
                if (!_settingsIO)
                    return;

                rapidjson::Document document;
                document.SetObject();
                auto& allocator = document.GetAllocator();
                document.AddMember("SettingsVersion", toJSON(settingsVersion, allocator), allocator);

                // Serialize the settings.
                for (const auto& settings : _settings)
                {
                    document.AddMember(rapidjson::Value(settings->getName().c_str(), allocator), settings->save(allocator), allocator);
                }

                // Write the JSON to disk.
                _writeSettingsFile(document);
            }

            void System::_readSettingsFile()
            {
                std::string readSettingsVersion;
                try
                {
                    if (FileSystem::FileInfo(_settingsPath).doesExist())
                    {
                        std::stringstream ss;
                        ss << "Reading settings: " << _settingsPath;
                        _log(ss.str());

                        auto fileIO = FileSystem::FileIO::create();
                        fileIO->open(_settingsPath.get(), FileSystem::FileIO::Mode::Read);
                        size_t bufSize = 0;
#if defined(DJV_MMAP)
                        const char * bufP = reinterpret_cast<const char *>(fileIO->mmapP());
                        const char* bufEnd = reinterpret_cast<const char*>(fileIO->mmapEnd());
                        bufSize = bufEnd - bufP;
#else // DJV_MMAP
                        std::vector<char> buf;
                        bufSize = fileIO->getSize();
                        buf.resize(bufSize);
                        fileIO->read(buf.data(), bufSize);
                        const char* bufP = buf.data();
#endif // DJV_MMAP

                        rapidjson::ParseResult result = _document.Parse(bufP, bufSize);
                        if (!result)
                        {
                            size_t line = 0;
                            size_t character = 0;
                            RapidJSON::errorLineNumber(bufP, bufSize, result.Offset(), line, character);
                            throw std::runtime_error(String::Format("{0} {1} {2}, {3} {4}").
                                arg(rapidjson::GetParseError_En(result.Code())).
                                arg(_getText(DJV_TEXT("error_line_number"))).
                                arg(line).
                                arg(_getText(DJV_TEXT("error_character_number"))).
                                arg(character));
                        }

                        for (const auto& i : _document.GetObject())
                        {
                            if (0 == strcmp("SettingsVersion", i.name.GetString()))
                            {
                                fromJSON(i.value, readSettingsVersion);
                                break;
                            }
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Cannot read settings" << " '" << _settingsPath << "': " << e.what();
                    _log(ss.str(), LogLevel::Error);
                }
                if (settingsVersion != readSettingsVersion)
                {
                    if (_document.IsObject())
                    {
                        _document.RemoveAllMembers();
                    }
                }
            }

            void System::_writeSettingsFile(const rapidjson::Document& document)
            {
                try
                {
                    std::stringstream ss;
                    ss << "Writing settings: " << _settingsPath;
                    _log(ss.str());

                    rapidjson::StringBuffer buffer;
                    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                    document.Accept(writer);

                    auto fileIO = FileSystem::FileIO::create();
                    fileIO->open(_settingsPath.get(), FileSystem::FileIO::Mode::Write);
                    fileIO->write(buffer.GetString());
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Cannot write settings" << " '" << _settingsPath << "': " << e.what();
                    _log(ss.str(), LogLevel::Error);
                }
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

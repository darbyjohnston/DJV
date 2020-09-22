// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/SettingsSystem.h>

#include <djvUI/ISettings.h>

#include <djvAV/AVSystem.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileIO.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/ResourceSystem.h>

#include <djvCore/ErrorFunc.h>
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
                const std::string settingsVersion = "39";

            } // namespace

            void SettingsSystem::_init(bool reset, const std::shared_ptr<System::Context>& context)
            {
                ISystem::_init("djv::UI::Settings::SettingsSystem", context);
                
                //! \bug Is there a better way to disable settings for tests?
                if ("djvTest" == context->getName())
                {
                    _settingsIO = false;
                }

                addDependency(context->getSystemT<AV::AVSystem>());

                if (auto resourceSystem = context->getSystemT<System::ResourceSystem>())
                {
                    _settingsPath = resourceSystem->getPath(System::File::ResourcePath::SettingsFile);
                }

                if (!reset)
                {
                    _readSettingsFile();
                }
            }

            SettingsSystem::SettingsSystem()
            {}

            SettingsSystem::~SettingsSystem()
            {
                _saveSettings();
            }

            std::shared_ptr<SettingsSystem> SettingsSystem::create(bool reset, const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
                out->_init(reset, context);
                return out;
            }

            void SettingsSystem::_addSettings(const std::shared_ptr<ISettings>& value)
            {
                _settings.push_back(value);
            }

            void SettingsSystem::_removeSettings(const std::shared_ptr<ISettings>& value)
            {
                const auto i = std::find(_settings.begin(), _settings.end(), value);
                if (i != _settings.end())
                {
                    _settings.erase(i);
                }
            }

            void SettingsSystem::_loadSettings(const std::shared_ptr<ISettings>& settings)
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
                            _log(ss.str(), System::LogLevel::Error);
                        }
                    }
                }
            }

            void SettingsSystem::_saveSettings()
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

            void SettingsSystem::_readSettingsFile()
            {
                std::string readSettingsVersion;
                try
                {
                    if (System::File::Info(_settingsPath).doesExist())
                    {
                        std::stringstream ss;
                        ss << "Reading settings: " << _settingsPath;
                        _log(ss.str());

                        auto fileIO = System::File::IO::create();
                        fileIO->open(_settingsPath.get(), System::File::IO::Mode::Read);
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
                    _log(ss.str(), System::LogLevel::Error);
                }
                if (settingsVersion != readSettingsVersion)
                {
                    if (_document.IsObject())
                    {
                        _document.RemoveAllMembers();
                    }
                }
            }

            void SettingsSystem::_writeSettingsFile(const rapidjson::Document& document)
            {
                try
                {
                    std::stringstream ss;
                    ss << "Writing settings: " << _settingsPath;
                    _log(ss.str());

                    rapidjson::StringBuffer buffer;
                    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                    document.Accept(writer);

                    auto fileIO = System::File::IO::create();
                    fileIO->open(_settingsPath.get(), System::File::IO::Mode::Write);
                    fileIO->write(buffer.GetString());
                }
                catch (const std::exception& e)
                {
                    std::stringstream ss;
                    ss << "Cannot write settings" << " '" << _settingsPath << "': " << e.what();
                    _log(ss.str(), System::LogLevel::Error);
                }
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv

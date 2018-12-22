//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/SettingsSystem.h>

#include <djvUI/ISettings.h>

#include <djvCore/Context.h>
#include <djvCore/Error.h>
#include <djvCore/FileIO.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct SettingsSystem::Private
        {
            std::map<std::string, picojson::value> json;
            std::map<std::string, std::shared_ptr<ISettings> > settings;
            FileSystem::Path settingsPath;
        };

        void SettingsSystem::_init(Core::Context * context)
        {
            ISystem::_init("djv::UI::SettingsSystem", context);
            _p->settingsPath = context->getPath(FileSystem::ResourcePath::SettingsFile);
            _readSettingsFile(_p->settingsPath, _p->json);
        }

        SettingsSystem::SettingsSystem() :
            _p(new Private)
        {}

        SettingsSystem::~SettingsSystem()
        {
            _saveSettings();
        }

        std::shared_ptr<SettingsSystem> SettingsSystem::create(Core::Context * context)
        {
            auto out = std::shared_ptr<SettingsSystem>(new SettingsSystem);
            out->_init(context);
            return out;
        }

        void SettingsSystem::_addSettings(const std::shared_ptr<ISettings>& value)
        {
            _p->settings[value->getName()] = value;
        }

        void SettingsSystem::_removeSettings(const std::shared_ptr<ISettings>& value)
        {
            const auto i = _p->settings.find(value->getName());
            if (i != _p->settings.end())
            {
                _p->settings.erase(i);
            }
        }

        void SettingsSystem::_loadSettings(const std::shared_ptr<ISettings>& settings)
        {
            std::stringstream s;
            s << "Loading settings: " << settings->getName();
            _log(s.str());

            picojson::value json;
            auto i = _p->json.find(settings->getName());
            if (i != _p->json.end())
            {
                settings->load(i->second);
            }
        }

        void SettingsSystem::_saveSettings()
        {
            picojson::value object(picojson::object_type, true);

            // Serialize the settings.
            for (const auto& settings : _p->settings)
            {
                object.get<picojson::object>()[settings.second->getName()] = settings.second->save();
            }

            // Write the JSON to disk.
            _writeSettingsFile(_p->settingsPath, object);
        }

        void SettingsSystem::_readSettingsFile(const FileSystem::Path& path, std::map<std::string, picojson::value>& out)
        {
            try
            {
                if (FileSystem::FileInfo(path).doesExist())
                {
                    std::stringstream s;
                    s << "Reading settings: " << path;
                    _log(s.str());

                    FileSystem::FileIO fileIO;
                    fileIO.open(path, FileSystem::FileIO::Mode::Read);
                    const char* p = reinterpret_cast<const char*>(fileIO.mmapP());
                    const char* end = reinterpret_cast<const char*>(fileIO.mmapEnd());

                    picojson::value v;
                    std::string error;
                    picojson::parse(v, p, end, &error);
                    if (!error.empty())
                    {
                        throw std::runtime_error(error);
                    }

                    if (v.is<picojson::object>())
                    {
                        const auto& object = v.get<picojson::object>();
                        for (const auto& value : object)
                        {
                            out[value.first] = value.second;
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                std::stringstream s;
                s << DJV_TEXT("Cannot read settings") << " '" << path << "'. " << e.what();
                _log(s.str(), LogLevel::Error);
            }
        }

        void SettingsSystem::_writeSettingsFile(const FileSystem::Path& path, const picojson::value& value)
        {
            try
            {
                std::stringstream s;
                s << "Writing settings: " << path;
                _log(s.str());

                FileSystem::FileIO fileIO;
                fileIO.open(path, FileSystem::FileIO::Mode::Write);
                PicoJSON::write(value, fileIO);
                fileIO.write("\n");
            }
            catch (const std::exception& e)
            {
                std::stringstream s;
                s << DJV_TEXT("Cannot write settings") << " '" << path << "'. " << e.what();
                _log(s.str(), LogLevel::Error);
            }
        }

    } // namespace UI
} // namespace djv

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

#pragma once

#include <djvCore/ISystem.h>

#include <djvCore/PicoJSON.h>
#include <djvCore/Path.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            class ISettings;

            //! This class provides a system for saving and restoring user settings.
            //!
            //! \bug How can we merge settings changes from multiple application instances?
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);
                void _init(Core::Context *);
                System();

            public:
                virtual ~System();

                //! Create a new settings system.
                static std::shared_ptr<System> create(Core::Context *);

				//! Get the list of settings.
				inline const std::vector<std::shared_ptr<ISettings> > & getSettings() const;

				//! Get a setting of the given type.
				template<typename T>
				inline std::shared_ptr<T> getSettingsT() const;

            private:
                void _addSettings(const std::shared_ptr<ISettings>&);
                void _removeSettings(const std::shared_ptr<ISettings>&);

                void _loadSettings(const std::shared_ptr<ISettings>&);
                void _saveSettings();

                void _readSettingsFile(const Core::FileSystem::Path&, std::map<std::string, picojson::value>&);
                void _writeSettingsFile(const Core::FileSystem::Path&, const picojson::value&);

				std::map<std::string, picojson::value> _json;
				std::vector<std::shared_ptr<ISettings> > _settings;
				Core::FileSystem::Path _settingsPath;

                friend class ISettings;
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

#include <djvUI/SettingsSystemInline.h>

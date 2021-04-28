// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvSystem/ISystem.h>
#include <djvSystem/Path.h>

#include <djvCore/RapidJSON.h>

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            class ISettings;

            //! User settings system.
            //!
            //! \bug How can we merge settings changes from multiple application instances?
            class SettingsSystem : public System::ISystem
            {
                DJV_NON_COPYABLE(SettingsSystem);
                void _init(bool reset, const std::shared_ptr<System::Context>&);
                SettingsSystem();

            public:
                ~SettingsSystem() override;

                static std::shared_ptr<SettingsSystem> create(bool reset, const std::shared_ptr<System::Context>&);

                //! Get the list of settings.
                const std::vector<std::shared_ptr<ISettings> >& getSettings() const;

                //! Get settings of the given type.
                template<typename T>
                std::shared_ptr<T> getSettingsT() const;

            private:
                void _addSettings(const std::shared_ptr<ISettings>&);
                void _removeSettings(const std::shared_ptr<ISettings>&);

                void _loadSettings(const std::shared_ptr<ISettings>&);
                void _saveSettings();

                void _readSettingsFile();
                void _writeSettingsFile(const rapidjson::Document&);

                bool _reset = false;
                rapidjson::Document _document;
                std::vector<std::shared_ptr<ISettings> > _settings;
                System::File::Path _settingsPath;
                bool _settingsIO = true;

                friend class ISettings;
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

#include <djvUI/SettingsSystemInline.h>

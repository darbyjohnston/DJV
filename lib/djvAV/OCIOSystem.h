// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OCIO.h>

#include <djvCore/ISystem.h>
#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/RapidJSON.h>
#include <djvCore/ValueObserver.h>

#include <OpenColorIO/OpenColorIO.h>

namespace djv
{
    namespace AV
    {
        namespace OCIO
        {
            //! This enumeration provides the configuration types.
            enum class ConfigType
            {
                User,
                Env,
                CmdLine
            };

            //! This struct provides a color space configuration.
            struct Config
            {
                Config();
                
                ConfigType  type     = ConfigType::User;
                std::string fileName;
                std::string name;
                std::string display;
                std::string view;
                std::map<std::string, std::string> imageColorSpaces;

                static std::string getNameFromFileName(const std::string&);

                bool operator == (const Config&) const;
                bool operator != (const Config&) const;
            };

            //! This struct provides configuration data for the UI.
            struct ConfigData
            {
                std::vector<std::string> fileNames;
                std::vector<std::string> names;
                int current = -1;

                bool operator == (const ConfigData&) const;
            };

            //! This struct provides display data for the UI.
            struct DisplayData
            {
                std::vector<std::string> names;
                int current = -1;

                bool operator == (const DisplayData&) const;
            };

            //! This struct provides view data for the UI.
            struct ViewData
            {
                std::vector<std::string> names;
                int current = -1;

                bool operator == (const ViewData&) const;
            };

            //! This class manages color space configurations.
            class System : public Core::ISystem
            {
                DJV_NON_COPYABLE(System);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                System();

            public:
                ~System() override;

                static std::shared_ptr<System> create(const std::shared_ptr<Core::Context>&);

                //! \name Configurations
                ///@{

                std::shared_ptr<Core::IListSubject<Config> > observeConfigs() const;
                std::shared_ptr<Core::IValueSubject<Config> > observeCurrentConfig() const;
                int addConfig(const std::string& fileName);
                int addConfig(const Config&);
                void removeConfig(int);

                std::shared_ptr<Core::IValueSubject<ConfigData> > observeConfigData() const;
                std::shared_ptr<Core::IValueSubject<DisplayData> > observeDisplayData() const;
                std::shared_ptr<Core::IValueSubject<ViewData> > observeViewData() const;
                std::shared_ptr<Core::IListSubject<std::string> > observeColorSpaces() const;
                std::shared_ptr<Core::IMapSubject<std::string, std::string> > observeImageColorSpaces() const;
                void setCurrentConfig(int);
                void setCurrentDisplay(int);
                void setCurrentView(int);
                void setImageColorSpaces(const std::map<std::string, std::string>&);

                ///@}

                //! \name Utilities
                ///@{

                //! Get the color space for the given display and view.
                std::string getColorSpace(const std::string& display, const std::string& view) const;

                ///@}
                
            private:
                int _addConfig(const Config&, bool init);
                void _configUpdate();
                void _dataUpdate();

                DJV_PRIVATE();
            };

        } // namespace OCIO
    } // namespace AV

    rapidjson::Value toJSON(const AV::OCIO::Config&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::OCIO::Config&);

} // namespace djv

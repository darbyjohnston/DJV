// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/OCIO.h>

#include <djvCore/Enum.h>
#include <djvCore/ISystem.h>
#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/RapidJSONFunc.h>
#include <djvCore/ValueObserver.h>

#include <OpenColorIO/OpenColorIO.h>

namespace djv
{
    namespace AV
    {
        namespace OCIO
        {
            //! This enumeration provides the configuration modes.
            enum class ConfigMode
            {
                None,
                CmdLine,
                Env,
                User,

                Count,
                First = None
            };
            DJV_ENUM_HELPERS(ConfigMode);

            //! Thsi typedef provides the image color space associations.
            typedef std::map<std::string, std::string> ImageColorSpaces;

            //! This struct provides a color space configuration.
            struct Config
            {
                Config();
                
                std::string      fileName;
                std::string      name;
                std::string      display;
                std::string      view;
                ImageColorSpaces imageColorSpaces;

                bool isValid() const;

                static std::string getNameFromFileName(const std::string&);

                bool operator == (const Config&) const;
                bool operator != (const Config&) const;
            };

            typedef std::pair<std::vector<Config>, int> UserConfigs;
            typedef std::pair<std::vector<std::string>, int> Displays;
            typedef std::pair<std::vector<std::string>, int> Views;

            //! This class manages color space configurations.
            //!
            //! \todo Split out the UI interface from this class into a
            //! view model.
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

                bool hasEnvConfig() const;

                std::shared_ptr<Core::IValueSubject<ConfigMode> > observeConfigMode() const;
                std::shared_ptr<Core::IValueSubject<Config> > observeCmdLineConfig() const;
                std::shared_ptr<Core::IValueSubject<Config> > observeEnvConfig() const;
                std::shared_ptr<Core::IValueSubject<UserConfigs> > observeUserConfigs() const;
                std::shared_ptr<Core::IValueSubject<Config> > observeCurrentConfig() const;
                std::shared_ptr<Core::IValueSubject<Displays> > observeDisplays() const;
                std::shared_ptr<Core::IValueSubject<Views> > observeViews() const;
                std::shared_ptr<Core::IMapSubject<std::string, std::string> > observeImageColorSpaces() const;
                std::shared_ptr<Core::IListSubject<std::string> > observeColorSpaces() const;

                void setConfigMode(ConfigMode);
                void setCmdLineConfig(const Config&);
                void setEnvConfig(const Config&);
                int addUserConfig(const std::string& fileName);
                int addUserConfig(const Config&);
                void removeUserConfig(int);
                void setCurrentUserConfig(int);
                void setCurrentDisplay(int);
                void setCurrentView(int);
                void setImageColorSpaces(const ImageColorSpaces&);

                ///@}

                //! \name Utilities
                ///@{

                //! Get the color space for the given display and view.
                std::string getColorSpace(const std::string& display, const std::string& view) const;

                ///@}
                
            private:
                DJV_PRIVATE();
            };

        } // namespace OCIO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::OCIO::ConfigMode);

    rapidjson::Value toJSON(AV::OCIO::ConfigMode, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const AV::OCIO::Config&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::OCIO::ConfigMode&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::OCIO::Config&);

} // namespace djv

#include <djvAV/OCIOSystemInline.h>

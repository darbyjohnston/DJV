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
            //! This struct provides a color space configuration.
            class Config
            {
            public:
                Config();
                
                std::string fileName;
                std::string name;
                std::string display;
                std::string view;
                std::map<std::string, std::string> fileColorSpaces;

                static std::string getNameFromFileName(const std::string&);

                bool operator == (const Config&) const;
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

                int addConfig(const std::string& fileName);
                int addConfig(const Config&);
                void removeConfig(int);

                ///@}

                //! \name Current Configuration
                ///@{

                std::shared_ptr<Core::IValueSubject<Config> > observeCurrentConfig() const;
                std::shared_ptr<Core::IValueSubject<int> > observeCurrentIndex() const;
                std::shared_ptr<Core::IListSubject<std::string> > observeColorSpaces() const;
                std::shared_ptr<Core::IListSubject<Display> > observeDisplays() const;
                std::shared_ptr<Core::IListSubject<std::string> > observeViews() const;
            
                void setCurrentConfig(const Config&);
                void setCurrentIndex(int);

                ///@}

                //! \name Utilities
                ///@{

                //! Get the color space for the given display and view.
                std::string getColorSpace(const std::string& display, const std::string& view) const;

                ///@}
                
            private:
                int _addConfig(const Config&, bool init);
                void _configUpdate();

                DJV_PRIVATE();
            };

        } // namespace OCIO
    } // namespace AV

    rapidjson::Value toJSON(const AV::OCIO::Config&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::OCIO::Config&);

} // namespace djv

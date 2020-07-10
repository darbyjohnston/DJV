// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/ViewApp.h>

#include <djvAV/OCIOSystem.h>

namespace djv
{
    namespace ViewApp
    {
        //! This struct provides color space configuration data.
        struct ColorSpaceConfigData
        {
            struct Item
            {
                std::string fileName;
                std::string name;

                bool operator == (const Item&) const;
            };

            std::vector<Item> items;
            int               current = -1;

            bool operator == (const ColorSpaceConfigData&) const;
        };

        //! This struct provides color space display data.
        struct ColorSpaceDisplayData
        {
            std::vector<std::string> names;
            int                      current = -1;

            bool operator == (const ColorSpaceDisplayData&) const;
        };

        //! This struct provides color space view data.
        struct ColorSpaceViewData
        {
            std::vector<std::string> names;
            int                      current = -1;

            bool operator == (const ColorSpaceViewData&) const;
        };

        //! This class provides the color space view model.
        class ColorSpaceViewModel : public std::enable_shared_from_this<ColorSpaceViewModel>
        {
            DJV_NON_COPYABLE(ColorSpaceViewModel);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ColorSpaceViewModel();

        public:
            ~ColorSpaceViewModel();

            static std::shared_ptr<ColorSpaceViewModel> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<ColorSpaceConfigData> > observeUserConfigData() const;
            std::shared_ptr<Core::IValueSubject<ColorSpaceConfigData::Item> > observeEnvConfigData() const;
            std::shared_ptr<Core::IValueSubject<ColorSpaceConfigData::Item> > observeCmdLineConfigData() const;
            std::shared_ptr<Core::IValueSubject<ColorSpaceDisplayData> > observeDisplayData() const;
            std::shared_ptr<Core::IValueSubject<ColorSpaceViewData> > observeViewData() const;
            std::shared_ptr<Core::IListSubject<std::string> > observeColorSpaces() const;
            std::shared_ptr<Core::IMapSubject<std::string, std::string> > observeImageColorSpaces() const;
            void setCurrentUserConfig(int);
            void setCurrentDisplay(int);
            void setCurrentView(int);
            void setImageColorSpaces(const AV::OCIO::ImageColorSpaces&);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv

#include <djvViewApp/ColorSpaceViewModelInline.h>

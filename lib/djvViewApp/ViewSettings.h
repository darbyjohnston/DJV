// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>
#include <djvViewApp/HUD.h>
#include <djvViewApp/ImageView.h>

#include <djvUI/ISettings.h>

#include <djvCore/BBox.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Color;

        } // namespace Image
    } // namespace AV

    namespace ViewApp
    {
        //! This class provides the view settings.
        class ViewSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ViewSettings);

        protected:
            void _init(const std::shared_ptr<Core::Context>& context);

            ViewSettings();

        public:
            static std::shared_ptr<ViewSettings> create(const std::shared_ptr<Core::Context>&);

            int getWidgetCurrentTab() const;
            void setWidgetCurrentTab(int);

            std::shared_ptr<Core::IValueSubject<ImageViewLock> > observeLock() const;
            void setLock(ImageViewLock);

            std::shared_ptr<Core::IValueSubject<GridOptions> > observeGridOptions() const;
            void setGridOptions(const GridOptions&);

            std::shared_ptr<Core::IValueSubject<HUDOptions> > observeHUDOptions() const;
            void setHUDOptions(const HUDOptions&);

            std::shared_ptr<Core::IValueSubject<AV::Image::Color> > observeBackgroundColor() const;
            void setBackgroundColor(const AV::Image::Color&);

            const std::map<std::string, Core::BBox2f>& getWidgetGeom() const;
            void setWidgetGeom(const std::map<std::string, Core::BBox2f>&);

            void load(const picojson::value &) override;
            picojson::value save() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


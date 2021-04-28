// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/ISettings.h>

#include <djvMath/BBox.h>

#include <djvCore/ValueObserver.h>

#include <map>

namespace djv
{
    namespace ViewApp
    {
        struct GridOptions;
        struct HUDOptions;
        struct ViewBackgroundOptions;

        //! View settings.
        class ViewSettings : public UI::Settings::ISettings
        {
            DJV_NON_COPYABLE(ViewSettings);

        protected:
            void _init(const std::shared_ptr<System::Context>& context);

            ViewSettings();

        public:
            static std::shared_ptr<ViewSettings> create(const std::shared_ptr<System::Context>&);

            //! \name Bellows
            ///@{

            const std::map<std::string, bool> getBellowsState() const;

            void setBellowsState(const std::map<std::string, bool>&);

            ///@}

            //! \name View Lock
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<ViewLock> > observeLock() const;

            void setLock(ViewLock);

            ///@}

            //! \name Options
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<GridOptions> > observeGridOptions() const;
            std::shared_ptr<Core::Observer::IValueSubject<HUDOptions> > observeHUDOptions() const;
            std::shared_ptr<Core::Observer::IValueSubject<ViewBackgroundOptions> > observeBackgroundOptions() const;

            void setGridOptions(const GridOptions&);
            void setHUDOptions(const HUDOptions&);
            void setBackgroundOptions(const ViewBackgroundOptions&);

            ///@}

            //! \name Geometry
            ///@{

            const std::map<std::string, Math::BBox2f>& getWidgetGeom() const;

            void setWidgetGeom(const std::map<std::string, Math::BBox2f>&);

            ///@}

            void load(const rapidjson::Value &) override;
            rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


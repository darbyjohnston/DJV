// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Enum.h>

#include <djvCore/ISystem.h>
#include <djvCore/Speed.h>
#include <djvCore/Time.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace Render2D
        {
            class ImageFilterOptions;

        } // namespace Render2D

        //! This class provides an AV system.
        class AVSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(AVSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            AVSystem();

        public:
            ~AVSystem() override;

            static std::shared_ptr<AVSystem> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<Core::Time::Units> > observeTimeUnits() const;
            void setTimeUnits(Core::Time::Units);

            std::shared_ptr<Core::IValueSubject<AlphaBlend> > observeAlphaBlend() const;
            void setAlphaBlend(AlphaBlend);

            std::shared_ptr<Core::IValueSubject<Core::Time::FPS> > observeDefaultSpeed() const;
            void setDefaultSpeed(Core::Time::FPS);

            std::shared_ptr<Core::IValueSubject<Render2D::ImageFilterOptions> > observeImageFilterOptions() const;
            void setImageFilterOptions(const Render2D::ImageFilterOptions&);

            std::shared_ptr<Core::IValueSubject<bool> > observeLCDText() const;
            void setLCDText(bool);

        private:
            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv

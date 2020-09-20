// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Speed.h>
#include <djvAV/Time.h>

#include <djvSystem/ISystem.h>

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
        class AVSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(AVSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            AVSystem();

        public:
            ~AVSystem() override;

            static std::shared_ptr<AVSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<Time::Units> > observeTimeUnits() const;
            std::shared_ptr<Core::IValueSubject<FPS> > observeDefaultSpeed() const;

            void setTimeUnits(Time::Units);
            void setDefaultSpeed(FPS);

        private:
            DJV_PRIVATE();
        };

    } // namespace AV
} // namespace djv

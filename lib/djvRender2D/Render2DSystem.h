// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvRender2D/Enum.h>

#include <djvSystem/ISystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Render2D
    {
        class ImageFilterOptions;

        //! This class provides a render 2D system.
        class Render2DSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(Render2DSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            Render2DSystem();

        public:
            ~Render2DSystem() override;

            static std::shared_ptr<Render2DSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<AlphaBlend> > observeAlphaBlend() const;
            std::shared_ptr<Core::IValueSubject<Render2D::ImageFilterOptions> > observeImageFilterOptions() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeTextLCDRendering() const;

            void setAlphaBlend(AlphaBlend);
            void setImageFilterOptions(const Render2D::ImageFilterOptions&);
            void setTextLCDRendering(bool);

        private:
            DJV_PRIVATE();
        };

    } // namespace Render2D
} // namespace djv

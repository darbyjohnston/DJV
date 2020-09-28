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

        //! This class provides a 2D render system.
        class RenderSystem : public System::ISystem
        {
            DJV_NON_COPYABLE(RenderSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            RenderSystem();

        public:
            ~RenderSystem() override;

            static std::shared_ptr<RenderSystem> create(const std::shared_ptr<System::Context>&);

            std::shared_ptr<Core::IValueSubject<Render2D::ImageFilterOptions> > observeImageFilterOptions() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeTextLCDRendering() const;

            void setImageFilterOptions(const Render2D::ImageFilterOptions&);
            void setTextLCDRendering(bool);

        private:
            DJV_PRIVATE();
        };

    } // namespace Render2D
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Style.h>

#include <djvCore/ISystem.h>

#include <future>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Image;

        } // namespace Image
    } // namespace AV

    namespace UI
    {
        //! This class provides an icon system.
        class IconSystem : public Core::ISystem
        {
            DJV_NON_COPYABLE(IconSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            IconSystem();

        public:
            ~IconSystem() override;

            static std::shared_ptr<IconSystem> create(const std::shared_ptr<Core::Context>&);
            
            //! Get an icon.
            std::future<std::shared_ptr<AV::Image::Image> > getIcon(const std::string&, float size);

            //! Get the cache percentage used.
            float getCachePercentage() const;

        private:
            void _handleImageRequests();

            DJV_PRIVATE();
        };

    } // namespace UI
} // namespace djv

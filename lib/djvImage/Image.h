// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvImage/ImageData.h>
#include <djvImage/Tags.h>

namespace djv
{
    namespace Image
    {
        //! This class provides an image.
        class Image : public Data
        {
            DJV_NON_COPYABLE(Image);

        protected:
            void _init(const Info&);
            Image();

        public:
            ~Image();

            static std::shared_ptr<Image> create(const Info&);

            const std::string& getPluginName() const;

            void setPluginName(const std::string&);

            const Tags& getTags() const;

            void setTags(const Tags&);

        private:
            std::string _pluginName;
            Tags _tags;
        };

    } // namespace Image
} // namespace djv

#include <djvImage/ImageInline.h>

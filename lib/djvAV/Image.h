// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/ImageData.h>
#include <djvAV/Tags.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            //! This class provides an image.
            class Image : public Data
            {
                DJV_NON_COPYABLE(Image);

            protected:
                void _init(const Info&, const std::shared_ptr<Core::FileSystem::FileIO>&);
                Image();

            public:
                ~Image();

#if defined(DJV_MMAP)
                static std::shared_ptr<Image> create(const Info&, const std::shared_ptr<Core::FileSystem::FileIO>& = nullptr);
#else // DJV_MMAP
                static std::shared_ptr<Image> create(const Info&);
#endif // DJV_MMAP

                const std::string& getPluginName() const;
                void setPluginName(const std::string&);

                const Tags& getTags() const;
                void setTags(const Tags&);

            private:
                std::string _pluginName;
                Tags _tags;
            };

        } // namespace Image
    } // namespace AV
} // namespace djv

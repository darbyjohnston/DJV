// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Image.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            void Image::_init(const Info& value, const std::shared_ptr<Core::FileSystem::FileIO>& io)
            {
                Data::_init(value, io);
            }

            Image::Image()
            {}

            Image::~Image()
            {}

#if defined(DJV_MMAP)
            std::shared_ptr<Image> Image::create(const Info& value, const std::shared_ptr<Core::FileSystem::FileIO>& io)
            {
                auto out = std::shared_ptr<Image>(new Image);
                out->_init(value, io);
                return out;
            }
#else // DJV_MMAP
            std::shared_ptr<Image> Image::create(const Info& value)
            {
                auto out = std::shared_ptr<Image>(new Image);
                out->_init(value, nullptr);
                return out;
            }
#endif // DJV_MMAP

            const std::string& Image::getPluginName() const
            {
                return _pluginName;
            }

            void Image::setPluginName(const std::string& value)
            {
                _pluginName = value;
            }

            const Tags& Image::getTags() const
            {
                return _tags;
            }

            void Image::setTags(const Tags& value)
            {
                _tags = value;
            }

        } // namespace Image
    } // namespace AV
} // namespace djv


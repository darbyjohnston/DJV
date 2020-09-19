// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Image.h>

namespace djv
{
    namespace Image
    {
        void Image::_init(const Info& value)
        {
            Data::_init(value);
        }

        Image::Image()
        {}

        Image::~Image()
        {}

        std::shared_ptr<Image> Image::create(const Info& value)
        {
            auto out = std::shared_ptr<Image>(new Image);
            out->_init(value);
            return out;
        }

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
} // namespace djv


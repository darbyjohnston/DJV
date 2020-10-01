// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Image
    {
        inline const std::string& Image::getPluginName() const
        {
            return _pluginName;
        }

        inline void Image::setPluginName(const std::string& value)
        {
            _pluginName = value;
        }

        inline const Tags& Image::getTags() const
        {
            return _tags;
        }

        inline void Image::setTags(const Tags& value)
        {
            _tags = value;
        }

    } // namespace Image
} // namespace djv

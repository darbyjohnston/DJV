// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/Tags.h>

namespace djv
{
    namespace AV
    {
        const std::string Tags::_empty;

        Tags::Tags()
        {}
        
        bool Tags::isEmpty() const
        {
            return 0 == _tags.size();
        }

        size_t Tags::getCount() const
        {
            return _tags.size();
        }

        const std::map<std::string, std::string>& Tags::getTags() const
        {
            return _tags;
        }

        bool Tags::hasTag(const std::string& key) const
        {
            return _tags.find(key) != _tags.end();
        }

        const std::string& Tags::getTag(const std::string& key) const
        {
            static const std::string empty;
            const auto i = _tags.find(key);
            return i != _tags.end() ? i->second : _empty;
        }

        void Tags::setTags(const std::map<std::string, std::string>& tags)
        {
            _tags = tags;
        }

        void Tags::setTag(const std::string& key, const std::string& value)
        {
            _tags[key] = value;
        }

        bool Tags::operator == (const Tags& other) const
        {
            return _tags == other._tags;
        }

    } // namespace AV
} // namespace djv


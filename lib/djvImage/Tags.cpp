// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImage/Tags.h>

namespace djv
{
    namespace Image
    {
        const std::string Tags::_empty;

        Tags::Tags()
        {}
        
        bool Tags::isEmpty() const
        {
            return 0 == _map.size();
        }

        size_t Tags::getCount() const
        {
            return _map.size();
        }

        const std::map<std::string, std::string>& Tags::get() const
        {
            return _map;
        }

        const std::string& Tags::get(const std::string& key) const
        {
            static const std::string empty;
            const auto i = _map.find(key);
            return i != _map.end() ? i->second : _empty;
        }

        bool Tags::contains(const std::string& key) const
        {
            return _map.find(key) != _map.end();
        }

        void Tags::set(const std::map<std::string, std::string>& tags)
        {
            _map = tags;
        }

        void Tags::set(const std::string& key, const std::string& value)
        {
            _map[key] = value;
        }

        bool Tags::operator == (const Tags& other) const
        {
            return _map == other._map;
        }

    } // namespace Image
} // namespace djv


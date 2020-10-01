// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Image
    {
        inline bool Tags::isEmpty() const
        {
            return 0 == _map.size();
        }

        inline size_t Tags::getCount() const
        {
            return _map.size();
        }

        inline const std::map<std::string, std::string>& Tags::get() const
        {
            return _map;
        }

        inline const std::string& Tags::get(const std::string& key) const
        {
            static const std::string empty;
            const auto i = _map.find(key);
            return i != _map.end() ? i->second : _empty;
        }

        inline bool Tags::contains(const std::string& key) const
        {
            return _map.find(key) != _map.end();
        }

        inline void Tags::set(const std::map<std::string, std::string>& tags)
        {
            _map = tags;
        }

        inline void Tags::set(const std::string& key, const std::string& value)
        {
            _map[key] = value;
        }

        inline bool Tags::operator == (const Tags& other) const
        {
            return _map == other._map;
        }

    } // namespace Image
} // namespace djv

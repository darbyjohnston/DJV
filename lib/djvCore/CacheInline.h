// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            template<typename T, typename U>
            inline size_t Cache<T, U>::getMax() const
            {
                return _max;
            }

            template<typename T, typename U>
            inline void Cache<T, U>::setMax(size_t value)
            {
                _max = value;
                _updateMax();
            }

            template<typename T, typename U>
            inline size_t Cache<T, U>::getSize() const
            {
                return _map.size();
            }

            template<typename T, typename U>
            inline bool Cache<T, U>::contains(const T & key) const
            {
                return _map.find(key) != _map.end();
            }

            template<typename T, typename U>
            inline bool Cache<T, U>::get(const T & key, U & value) const
            {
                auto i = _map.find(key);
                if (i != _map.end())
                {
                    value = i->second;
                    auto j = _counts.find(key);
                    if (j != _counts.end())
                    {
                        ++_counter;
                        j->second = _counter;
                    }
                    return true;
                }
                return i != _map.end();
            }

            template<typename T, typename U>
            inline void Cache<T, U>::add(const T & key, const U & value)
            {
                _map[key] = value;
                ++_counter;
                _counts[key] = _counter;
                _updateMax();
            }

            template<typename T, typename U>
            inline void Cache<T, U>::remove(const T& key)
            {
                const auto i = _map.find(key);
                if (i != _map.end())
                {
                    _map.erase(i);
                }
                const auto j = _counts.find(key);
                if (j != _counts.end())
                {
                    _counts.erase(j);
                }
                _updateMax();
            }
            
            template<typename T, typename U>
            inline void Cache<T, U>::clear()
            {
                _map.clear();
            }

            template<typename T, typename U>
            inline float Cache<T, U>::getPercentageUsed() const
            {
                return _map.size() / static_cast<float>(_max) * 100.F;
            }

            template<typename T, typename U>
            inline std::vector<T> Cache<T, U>::getKeys() const
            {
                std::vector<T> out;
                for (const auto & i : _map)
                {
                    out.push_back(i.first);
                }
                return out;
            }

            template<typename T, typename U>
            inline std::vector<U> Cache<T, U>::getValues() const
            {
                std::vector<U> out;
                for (const auto & i : _map)
                {
                    out.push_back(i.second);
                }
                return out;
            }

            template<typename T, typename U>
            inline void Cache<T, U>::_updateMax()
            {
                if (_map.size() > _max)
                {
                    std::map<int64_t, T> sorted;
                    for (const auto& i : _counts)
                    {
                        sorted[i.second] = i.first;
                    }
                    while (_map.size() > _max)
                    {
                        auto begin = sorted.begin();
                        auto i = _map.find(begin->second);
                        if (i != _map.end())
                        {
                            _map.erase(i);
                        }
                        auto j = _counts.find(begin->second);
                        if (j != _counts.end())
                        {
                            _counts.erase(j);
                        }
                        sorted.erase(begin);
                    }
                }
            }

        } // namespace Memory
    } // namespace Core
} // namespace djv


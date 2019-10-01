//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

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


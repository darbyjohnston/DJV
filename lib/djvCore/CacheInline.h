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
                return _list.size();
            }

            template<typename T, typename U>
            inline bool Cache<T, U>::contains(const T & key) const
            {
                const auto i = std::find_if(_list.begin(), _list.end(),
                    [key](const std::pair<T, U> & value)
                {
                    return key == value.first;
                });
                return i != _list.end();
            }

            template<typename T, typename U>
            inline const U & Cache<T, U>::get(const T & key) const
            {
                auto i = std::find_if(
                    _list.begin(),
                    _list.end(),
                    [key](const std::pair<T, U> & value)
                {
                    return key == value.first;
                });
                if (i != _list.end())
                {
                    auto value = i->second;
                    _list.erase(i);
                    _list.push_front(std::make_pair(key, value));
                    i = _list.begin();
                }
                return i->second;
            }

            template<typename T, typename U>
            inline void Cache<T, U>::add(const T & key, const U & value)
            {
                const auto i = std::find_if(
                    _list.begin(),
                    _list.end(),
                    [key](const std::pair<T, U> & value)
                {
                    return key == value.first;
                });
                if (i != _list.end())
                {
                    _list.erase(i);
                }
                _list.push_front(std::make_pair(key, value));
                _updateMax();
            }

            template<typename T, typename U>
            inline void Cache<T, U>::clear()
            {
                _list.clear();
            }

            template<typename T, typename U>
            inline float Cache<T, U>::getPercentageUsed() const
            {
                return _list.size() / static_cast<float>(_max) * 100.f;
            }

            template<typename T, typename U>
            inline std::vector<T> Cache<T, U>::getKeys() const
            {
                std::vector<T> out;
                for (const auto & i : _list)
                {
                    out.push_back(i.first);
                }
                return out;
            }

            template<typename T, typename U>
            inline std::vector<U> Cache<T, U>::getValues() const
            {
                std::vector<U> out;
                for (const auto & i : _list)
                {
                    out.push_back(i.second);
                }
                return out;
            }

            template<typename T, typename U>
            inline void Cache<T, U>::_updateMax()
            {
                while (_list.size() > _max)
                {
                    _list.pop_back();
                }
            }

        } // namespace Memory
    } // namespace Core
} // namespace djv


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

#pragma once

#include <djvCore/Core.h>

#include <map>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            //! This class provides a cache.
            //!
            //! \todo [1.0 M] Return an interator from get() instead of a value.
            template<typename T, typename U>
            class Cache
            {
            public:
                inline size_t getMax() const;
                inline void setMax(size_t);

                inline size_t getSize() const;
                inline bool contains(const T & key) const;
                inline const U & get(const T & key) const;
                inline void add(const T & key, const U & value);
                inline void clear();

                inline float getPercentageUsed() const;

                inline std::vector<T> getKeys() const;
                inline std::vector<U> getValues() const;

            private:
                void _updateMax();

                size_t _max = 10000;
                mutable std::map<T, std::pair<U, uint64_t> > _map;
                mutable std::map<uint64_t, T> _lru;
                mutable uint64_t _counter = 0;
            };

        } // namespace Memory
    } // namespace Core
} // namesapce djv

#include <djvCore/CacheInline.h>

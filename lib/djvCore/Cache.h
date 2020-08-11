// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <map>
#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            //! This class provides a cache.
            //!
            //! \todo Return an iterator from get() instead of a value.
            template<typename T, typename U>
            class Cache
            {
            public:
                //! \name Cache Maximum Size
                ///@{

                size_t getMax() const;
                void setMax(size_t);

                ///@}

                //! \name Cache Contents
                ///@{

                size_t getSize() const;
                float getPercentageUsed() const;
                bool contains(const T& key) const;
                bool get(const T& key, U& value) const;
                void add(const T& key, const U& value);
                void remove(const T& key);
                void clear();

                std::vector<T> getKeys() const;
                std::vector<U> getValues() const;

                ///@}

            private:
                void _maxUpdate();

                size_t _max = 10000;
                std::map<T, U> _map;
                mutable std::map<T, int64_t> _counts;
                mutable int64_t _counter = 0;
            };

        } // namespace Memory
    } // namespace Core
} // namesapce djv

#include <djvCore/CacheInline.h>

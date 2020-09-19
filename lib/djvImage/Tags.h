// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <map>
#include <string>

namespace djv
{
    namespace Image
    {
        //! This class provides string tags.
        class Tags
        {
        public:
            Tags();
            
            bool isEmpty() const;
            size_t getCount() const;
            const std::map<std::string, std::string>& get() const;
            const std::string& get(const std::string& key) const;
            bool contains(const std::string& key) const;

            void set(const std::map<std::string, std::string>&);
            void set(const std::string& key, const std::string& value);

            bool operator == (const Tags&) const;

        private:
            std::map<std::string, std::string> _map;
            static const std::string _empty;
        };

    } // namespace Image
} // namespace djv

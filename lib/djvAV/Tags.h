// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/AV.h>

#include <map>
#include <string>

namespace djv
{
    namespace AV
    {
        //! This class provides string tags.
        class Tags
        {
        public:
            Tags();
            
            bool isEmpty() const;
            size_t getCount() const;
            const std::map<std::string, std::string>& getTags() const;
            bool hasTag(const std::string& key) const;
            const std::string& getTag(const std::string& key) const;

            void setTags(const std::map<std::string, std::string>&);
            void setTag(const std::string& key, const std::string& value);

            bool operator == (const Tags&) const;

        private:
            std::map<std::string, std::string> _tags;
            static const std::string _empty;
        };

    } // namespace AV
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <sstream>
#include <vector>

//! This macro provides enum helpers.
#define DJV_ENUM_HELPERS(name)  \
    inline std::vector<name> get##name##Enums() \
    { \
        std::vector<name> out; \
        for (size_t i = 0; i < static_cast<size_t>(name::Count); ++i) \
        { \
            out.push_back(static_cast<name>(i)); \
        } \
        return out; \
    }

//! This macro provides enum serialziation helpers.
//!
//! oeprator >> throws:
//! - std::invalid_argument
#define DJV_ENUM_SERIALIZE_HELPERS(name) \
    std::ostream & operator << (std::ostream &, name); \
    std::istream & operator >> (std::istream &, name &);

namespace djv
{
    namespace Core
    {
        //! This enumeration provides the log levels.
        enum class LogLevel
        {
            Information,
            Warning,
            Error
        };
    }
}

//! This macro provides the enum serialziation helpers implementation.
//! Required includes:
//! - algorithm
//! - sstream
//! \todo How can we translate this?
#define DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(prefix, name, ...) \
    \
    std::ostream & operator << (std::ostream & os, prefix::name value) \
    { \
        const std::vector<std::string> data = \
        { \
            __VA_ARGS__ \
        }; \
        DJV_ASSERT(static_cast<size_t>(prefix::name::Count) == data.size()); \
        os << data[static_cast<size_t>(value)]; \
        return os; \
    } \
    \
    std::istream & operator >> (std::istream & is, prefix::name & value) \
    { \
        std::string s; \
        is >> s; \
        const std::vector<std::string> data = \
        { \
            __VA_ARGS__ \
        }; \
        DJV_ASSERT(static_cast<size_t>(prefix::name::Count) == data.size()); \
        const auto i = std::find(data.begin(), data.end(), s); \
        if (i == data.end()) \
        { \
            std::stringstream ss; \
            ss << DJV_TEXT("error_cannot_parse_the_value"); \
            throw std::invalid_argument(ss.str()); \
        } \
        value = static_cast<prefix::name>(i - data.begin()); \
        return is; \
    }

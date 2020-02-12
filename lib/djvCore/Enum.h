//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
#define DJV_ENUM_SERIALIZE_HELPERS(name) \
    std::ostream & operator << (std::ostream &, name); \
    std::istream & operator >> (std::istream &, name &);

//! This macro provides the enum serialziation helpers implementation.
//! Required includes:
//! - algorithm
//! - sstream
//! Throws:
//! - std::invalid_argument
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
            ss << DJV_TEXT("error_cannot_parse_the_value") << " '" << s << "'."; \
            throw std::invalid_argument(ss.str()); \
        } \
        value = static_cast<prefix::name>(i - data.begin()); \
        return is; \
    }

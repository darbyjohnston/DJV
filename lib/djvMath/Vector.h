// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/RapidJSON.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec2<T, P>&, const glm::tvec2<T, P>&) noexcept;
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec3<T, P>&, const glm::tvec3<T, P>&) noexcept;
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const glm::tvec4<T, P>&, const glm::tvec4<T, P>&) noexcept;

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const glm::tvec2<T, P>&);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const glm::tvec3<T, P>&);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const glm::tvec4<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, glm::tvec2<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, glm::tvec3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, glm::tvec4<T, P>&);

    rapidjson::Value toJSON(const glm::ivec2&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const glm::vec2&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const glm::vec3&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const glm::vec4&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, glm::ivec2&);
    void fromJSON(const rapidjson::Value&, glm::vec2&);
    void fromJSON(const rapidjson::Value&, glm::vec3&);
    void fromJSON(const rapidjson::Value&, glm::vec4&);

} // namespace djv

namespace std
{
    template<>
    struct hash<glm::ivec2>
    {
        std::size_t operator() (const glm::ivec2&) const noexcept;
    };

    template<>
    struct hash<glm::vec2>
    {
        std::size_t operator() (const glm::vec2&) const noexcept;
    };

} // namespace std

#include <djvMath/VectorInline.h>


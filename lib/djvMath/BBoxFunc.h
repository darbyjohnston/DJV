// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/BBox.h>

#include <djvCore/RapidJSONFunc.h>

#include <glm/mat4x4.hpp>

#include <sstream>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const Math::tBBox2<T, P>&, const Math::tBBox2<T, P>&) noexcept;
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const Math::tBBox3<T, P>&, const Math::tBBox3<T, P>&) noexcept;

    template<typename T, glm::precision P = glm::defaultp>
    Math::tBBox3<T, P> operator * (const Math::tBBox3<T, P>&, const glm::mat4&) noexcept;

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Math::tBBox2<T, P>&);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Math::tBBox3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Math::tBBox2<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Math::tBBox3<T, P>&);

    rapidjson::Value toJSON(const Math::BBox2i&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Math::BBox2f&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Math::BBox3f&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::BBox2i&);
    void fromJSON(const rapidjson::Value&, Math::BBox2f&);
    void fromJSON(const rapidjson::Value&, Math::BBox3f&);

} // namespace djv

#include <djvMath/BBoxFuncInline.h>


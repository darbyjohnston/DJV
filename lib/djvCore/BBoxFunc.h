// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/BBox.h>
#include <djvCore/RapidJSONFunc.h>

#include <glm/mat4x4.hpp>

#include <sstream>

namespace djv
{
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const Core::BBox::tBBox2<T, P>&, const Core::BBox::tBBox2<T, P>&) noexcept;
    template<typename T, glm::precision P = glm::defaultp>
    bool fuzzyCompare(const Core::BBox::tBBox3<T, P>&, const Core::BBox::tBBox3<T, P>&) noexcept;

    template<typename T, glm::precision P = glm::defaultp>
    Core::BBox::tBBox3<T, P> operator * (const Core::BBox::tBBox3<T, P>&, const glm::mat4&) noexcept;

    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Core::BBox::tBBox2<T, P>&);
    template<typename T, glm::precision P = glm::defaultp>
    std::ostream& operator << (std::ostream&, const Core::BBox::tBBox3<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Core::BBox::tBBox2<T, P>&);

    //! Throws:
    //! - std::exception
    template<typename T, glm::precision P = glm::defaultp>
    std::istream& operator >> (std::istream&, Core::BBox::tBBox3<T, P>&);

    rapidjson::Value toJSON(const Core::BBox2i&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Core::BBox2f&, rapidjson::Document::AllocatorType&);
    rapidjson::Value toJSON(const Core::BBox3f&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::BBox2i&);
    void fromJSON(const rapidjson::Value&, Core::BBox2f&);
    void fromJSON(const rapidjson::Value&, Core::BBox3f&);

} // namespace djv

#include <djvCore/BBoxFuncInline.h>


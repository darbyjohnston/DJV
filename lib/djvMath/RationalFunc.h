// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Rational.h>

#include <djvCore/RapidJSONFunc.h>

#include <sstream>

namespace djv
{
    template<typename T>
    Math::Rational<T> operator * (const Math::Rational<T>&, const Math::Rational<T>&);

    std::ostream& operator << (std::ostream&, const Math::IntRational&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Math::IntRational&);

    rapidjson::Value toJSON(const Math::IntRational&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::IntRational&);

} // namespace djv

#include <djvMath/RationalFuncInline.h>

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvMath/Rational.h>

#include <djvCore/RapidJSONFunc.h>

#include <sstream>

namespace djv
{
    Math::Rational operator * (const Math::Rational, const Math::Rational);

    std::ostream& operator << (std::ostream&, const Math::Rational&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Math::Rational&);

    rapidjson::Value toJSON(const Math::Rational&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Math::Rational&);

} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Rational.h>
#include <djvCore/RapidJSONFunc.h>

#include <sstream>

namespace djv
{
    std::ostream& operator << (std::ostream&, const Core::Math::Rational&);

    //! Throws:
    //! - std::exception
    std::istream& operator >> (std::istream&, Core::Math::Rational&);

    rapidjson::Value toJSON(const Core::Math::Rational&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Core::Math::Rational&);

} // namespace djv


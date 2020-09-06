// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Speed.h>

#include <djvCore/String.h>

#include <algorithm>
#include <array>
#include <cmath>

namespace djv
{
    namespace Core
    {
        namespace Time
        {
            namespace
            {
                FPS defaultSpeed = FPS::_24;

            } // namespace

            FPS getDefaultSpeed()
            {
                return defaultSpeed;
            }

            void setDefaultSpeed(FPS value)
            {
                defaultSpeed = value;
            }

            Math::Rational fromSpeed(FPS fps)
            {
                const std::array<int, static_cast<size_t>(FPS::Count)> scale =
                {
                    1,
                    3,
                    6,
                    12,
                    15,
                    16,
                    18,
                    24000,
                    24,
                    25,
                    30000,
                    30,
                    50,
                    60000,
                    60,
                    120,
                    240
                };
                const std::array<int, static_cast<size_t>(FPS::Count)> duration =
                {
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1,
                    1001,
                    1,
                    1,
                    1001,
                    1,
                    1,
                    1001,
                    1,
                    1,
                    1
                };
                return Math::Rational(scale[static_cast<size_t>(fps)], duration[static_cast<size_t>(fps)]);
            }

            Math::Rational fromSpeed(float value)
            {
                //! \bug Implement a proper floating-point to rational number conversion.
                //! Check-out: OpenEXR\IlmImf\ImfRational.h
                for (size_t i = 0; i < static_cast<size_t>(FPS::Count); ++i)
                {
                    const FPS fps = static_cast<FPS>(i);
                    const Math::Rational r = fromSpeed(fps);
                    const float diff = fabs(value - r.toFloat());
                    if (diff < .01F)
                    {
                        return r;
                    }
                }
                return Math::Rational::fromFloat(value);
            }

            bool toSpeed(const Math::Rational& value, FPS& out)
            {
                for (const auto i : getFPSEnums())
                {
                    if (value == fromSpeed(i))
                    {
                        out = i;
                        return true;
                    }
                }
                return false;
            }

        } // namespace Time
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Time,
        FPS,
        "1",
        "3",
        "6",
        "12",
        "15",
        "16",
        "18",
        "23.976",
        "24",
        "25",
        "29.97",
        "30",
        "50",
        "59.94",
        "60",
        "120",
        "240");

    rapidjson::Value toJSON(Core::Time::FPS value, rapidjson::Document::AllocatorType& allocator)
    {
        std::stringstream ss;
        ss << value;
        const std::string& s = ss.str();
        return rapidjson::Value(s.c_str(), s.size(), allocator);
    }

    void fromJSON(const rapidjson::Value& value, Core::Time::FPS& out)
    {
        if (value.IsString())
        {
            std::stringstream ss(value.GetString());
            ss >> out;
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv


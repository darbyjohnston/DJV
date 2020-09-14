// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/AnimationFunc.h>

namespace djv
{
    namespace Core
    {
        namespace Animation
        {
            Function getFunction(Type value)
            {
                const std::array<Function, 5> data =
                {
                    [](float t) { return t; },
                    [](float t) { return powf(t, 2.F); },
                    [](float t) { return powf(t, .5F); },
                    [](float t) { return Math::smoothStep(t, 0.F, 1.F); },
                    [](float t)
                {
                    return (sinf(t * Math::pi2 - Math::pi / 2.F) + 1.F) * .5F;
                }
                };
                return data[static_cast<size_t>(value)];
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(Type);

        } // namespace Animation
    } // namespace Core
    
    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Animation,
        Type,
        DJV_TEXT("animation_type_linear"),
        DJV_TEXT("animation_type_ease_in"),
        DJV_TEXT("animation_type_ease_out"),
        DJV_TEXT("animation_type_smooth_step"),
        DJV_TEXT("animation_type_sine"));

} // namespace djv


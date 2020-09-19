// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <vector>

namespace djv
{
    namespace Core
    {
        namespace Random
        {
            //! Get a random number between zero and one.
            float getRandom();

            //! Get a random number between zero and the given value.
            float getRandom(float);

            //! Get a random number between zero and the given value.
            int getRandom(int);

            //! Get a random number between two values.
            float getRandom(float min, float max);

            //! Get a random number between two values.
            int getRandom(int min, int max);

            //! Get a random item from a list.
            template<typename T>
            const T& getRandom(const std::vector<T>&);

            //! Seed the random value generator.
            void setRandomSeed(unsigned int);

            //! Seed the random value generator with the current time.
            void setRandomSeed();

        } // namespace Math
    } // namespace Core
} // namespace djv

#include <djvCore/RandomFuncInline.h>


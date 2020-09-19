// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvCore/RandomFunc.h>

#include <djvCore/TimeFunc.h>

#include <ctime>
#include <random>

namespace djv
{
    namespace Core
    {
        namespace Random
        {
            namespace
            {
                std::random_device rd;
                std::mt19937       rng(rd());

            } // namespace

            float getRandom()
            {
                std::uniform_int_distribution<uint32_t> uint_dist;
                return uint_dist(rng) / static_cast<float>(uint_dist.max());
            }

            float getRandom(float value)
            {
                return value * getRandom();
            }

            int getRandom(int value)
            {
                std::uniform_int_distribution<uint32_t> uint_dist;
                const float r = static_cast<float>(uint_dist(rng)) / static_cast<float>(uint_dist.max());
                return static_cast<int>(static_cast<float>(value + 1) * r);
            }

            float getRandom(float min, float max)
            {
                return min + (max - min) * getRandom();
            }

            int getRandom(int min, int max)
            {
                std::uniform_int_distribution<uint32_t> uint_dist;
                const float r = static_cast<float>(uint_dist(rng)) / static_cast<float>(uint_dist.max());
                return min + static_cast<int>(static_cast<float>(max - min + 1) * r);
            }

            void setRandomSeed(unsigned int value)
            {
                rng.seed(value);
            }

            void setRandomSeed()
            {
                const std::time_t t = std::time(nullptr);
                std::tm tm;
                Time::localtime(&t, &tm);
                setRandomSeed(tm.tm_sec);
            }

        } // namespace Random
    } // namespace Core
} // namespace djv


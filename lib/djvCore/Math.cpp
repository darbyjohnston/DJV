//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCore/Math.h>

#include <djvCore/Time.h>

#include <random>

namespace djv
{
    namespace Core
    {
        namespace Math
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
                const float r = uint_dist(rng) / static_cast<float>(uint_dist.max());
                return static_cast<int>((value + 1) * r);
            }

            float getRandom(float min, float max)
            {
                return min + (max - min) * getRandom();
            }

            int getRandom(int min, int max)
            {
                std::uniform_int_distribution<uint32_t> uint_dist;
                const float r = uint_dist(rng) / static_cast<float>(uint_dist.max());
                return min + static_cast<int>((max - min + 1) * r);
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

        } // namespace Math
    } // namespace Core
} // namespace djv

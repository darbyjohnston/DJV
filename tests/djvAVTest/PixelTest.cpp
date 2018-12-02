//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAVTest/PixelTest.h>

#include <djvAV/Pixel.h>

#include <sstream>

namespace djv
{
    namespace AVTest
    {
        PixelTest::PixelTest(const std::shared_ptr<Core::Context> & context) :
            ITest("djv::AVTest::PixelTest", context)
        {}
        
#define CONVERT(A, MIN, MAX, B) \
    { \
        auto bMin = static_cast<AV::Pixel::B##_T>(0); \
        auto bMax = static_cast<AV::Pixel::B##_T>(0); \
        AV::Pixel::convert_##A##_##B(MIN, bMin); \
        AV::Pixel::convert_##A##_##B(MAX, bMax); \
        std::stringstream ss; \
        ss << "    " << #B << " " << static_cast<int64_t>(bMin) << " " << static_cast<int64_t>(bMax); \
        _print(ss.str()); \
    }

        void PixelTest::run(int & argc, char ** argv)
        {
            {
                std::stringstream ss;
                ss << AV::Pixel::Type::RGBA_U8;
                DJV_ASSERT("RGBA_U8" == ss.str());
                AV::Pixel::Type type;
                ss >> type;
                DJV_ASSERT(AV::Pixel::Type::RGBA_U8 == type);
            }
            {
                std::stringstream ss;
                ss << "U8 " << static_cast<int64_t>(AV::Pixel::U8Min) << " " << static_cast<int64_t>(AV::Pixel::U8Max) << " =";
                _print(ss.str());
                CONVERT(U8, AV::Pixel::U8Min, AV::Pixel::U8Max, U10);
                CONVERT(U8, AV::Pixel::U8Min, AV::Pixel::U8Max, U16);
                CONVERT(U8, AV::Pixel::U8Min, AV::Pixel::U8Max, U32);
                CONVERT(U8, AV::Pixel::U8Min, AV::Pixel::U8Max, F16);
                CONVERT(U8, AV::Pixel::U8Min, AV::Pixel::U8Max, F32);
            }
            {
                std::stringstream ss;
                ss << "U10 " << static_cast<int64_t>(AV::Pixel::U10Min) << " " << static_cast<int64_t>(AV::Pixel::U10Max) << " =";
                _print(ss.str());
                CONVERT(U10, AV::Pixel::U10Min, AV::Pixel::U10Max, U8);
                CONVERT(U10, AV::Pixel::U10Min, AV::Pixel::U10Max, U16);
                CONVERT(U10, AV::Pixel::U10Min, AV::Pixel::U10Max, U32);
                CONVERT(U10, AV::Pixel::U10Min, AV::Pixel::U10Max, F16);
                CONVERT(U10, AV::Pixel::U10Min, AV::Pixel::U10Max, F32);
            }
            {
                std::stringstream ss;
                ss << "U16 " << static_cast<int64_t>(AV::Pixel::U16Min) << " " << static_cast<int64_t>(AV::Pixel::U16Max) << " =";
                _print(ss.str());
                CONVERT(U16, AV::Pixel::U16Min, AV::Pixel::U16Max, U8);
                CONVERT(U16, AV::Pixel::U16Min, AV::Pixel::U16Max, U10);
                CONVERT(U16, AV::Pixel::U16Min, AV::Pixel::U16Max, U32);
                CONVERT(U16, AV::Pixel::U16Min, AV::Pixel::U16Max, F16);
                CONVERT(U16, AV::Pixel::U16Min, AV::Pixel::U16Max, F32);
            }
            {
                std::stringstream ss;
                ss << "U32 " << static_cast<int64_t>(AV::Pixel::U32Min) << " " << static_cast<int64_t>(AV::Pixel::U32Max) << " =";
                _print(ss.str());
                CONVERT(U32, AV::Pixel::U32Min, AV::Pixel::U32Max, U8);
                CONVERT(U32, AV::Pixel::U32Min, AV::Pixel::U32Max, U10);
                CONVERT(U32, AV::Pixel::U32Min, AV::Pixel::U32Max, U16);
                CONVERT(U32, AV::Pixel::U32Min, AV::Pixel::U32Max, F16);
                CONVERT(U32, AV::Pixel::U32Min, AV::Pixel::U32Max, F32);
            }
            {
                std::stringstream ss;
                ss << "F16 " << static_cast<int64_t>(AV::Pixel::F16Min) << " " << static_cast<int64_t>(AV::Pixel::F16Max) << " =";
                _print(ss.str());
                CONVERT(F16, AV::Pixel::F16Min, AV::Pixel::F16Max, U8);
                CONVERT(F16, AV::Pixel::F16Min, AV::Pixel::F16Max, U10);
                CONVERT(F16, AV::Pixel::F16Min, AV::Pixel::F16Max, U16);
                CONVERT(F16, AV::Pixel::F16Min, AV::Pixel::F16Max, U32);
                CONVERT(F16, AV::Pixel::F16Min, AV::Pixel::F16Max, F32);
            }
            {
                std::stringstream ss;
                ss << "F32 " << static_cast<int64_t>(AV::Pixel::F32Min) << " " << static_cast<int64_t>(AV::Pixel::F32Max) << " =";
                _print(ss.str());
                CONVERT(F32, AV::Pixel::F32Min, AV::Pixel::F32Max, U8);
                CONVERT(F32, AV::Pixel::F32Min, AV::Pixel::F32Max, U10);
                CONVERT(F32, AV::Pixel::F32Min, AV::Pixel::F32Max, U16);
                CONVERT(F32, AV::Pixel::F32Min, AV::Pixel::F32Max, U32);
                CONVERT(F32, AV::Pixel::F32Min, AV::Pixel::F32Max, F16);
            }
        }
        
    } // namespace AVTest
} // namespace djv


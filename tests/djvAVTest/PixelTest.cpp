//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

using namespace djv::Core;
using namespace djv::AV;

#define CONVERT(A, RANGE, B) \
    { \
        auto bMin = static_cast<Image::B##_T>(0); \
        auto bMax = static_cast<Image::B##_T>(0); \
        Image::convert_##A##_##B(RANGE.min, bMin); \
        Image::convert_##A##_##B(RANGE.max, bMax); \
        std::stringstream ss; \
        ss << "    " << #B << " " << static_cast<int64_t>(bMin) << " " << static_cast<int64_t>(bMax); \
        _print(ss.str()); \
    }

namespace djv
{
    namespace AVTest
    {
        PixelTest::PixelTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::PixelTest", context)
        {}
        
        void PixelTest::run()
        {
            _enum();
            _constants();
            _convert();
        }
                
        void PixelTest::_enum()
        {
            for (auto i : Image::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "type string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : Image::getChannelsEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "channels string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : Image::getDataTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "data type string: " << _getText(ss.str());
                _print(ss2.str());
            }
        }
        
        void PixelTest::_constants()
        {
            {
                std::stringstream ss;
                ss << "U8 range: " << Image::U8Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "U10 range: " << Image::U10Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "U12 range: " << Image::U12Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "U16 range: " << Image::U16Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "U32 range: " << Image::U32Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "F16 range: " << Image::F16Range;
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "F32 range: " << Image::F32Range;
                _print(ss.str());
            }
        }
        
        void PixelTest::_convert()
        {
            {
                std::stringstream ss;
                ss << Image::Type::RGBA_U8;
                DJV_ASSERT("av_image_type_rgba_u8" == ss.str());
                Image::Type type;
                ss >> type;
                DJV_ASSERT(Image::Type::RGBA_U8 == type);
            }
            
            {
                std::stringstream ss;
                ss << "U8 " << Image::U8Range << " =";
                _print(ss.str());
                CONVERT(U8, Image::U8Range, U10);
                CONVERT(U8, Image::U8Range, U16);
                CONVERT(U8, Image::U8Range, U32);
                CONVERT(U8, Image::U8Range, F16);
                CONVERT(U8, Image::U8Range, F32);
            }
            
            {
                std::stringstream ss;
                ss << "U10 " << Image::U10Range << " =";
                _print(ss.str());
                CONVERT(U10, Image::U10Range, U8);
                CONVERT(U10, Image::U10Range, U16);
                CONVERT(U10, Image::U10Range, U32);
                CONVERT(U10, Image::U10Range, F16);
                CONVERT(U10, Image::U10Range, F32);
            }
            
            {
                std::stringstream ss;
                ss << "U16 " << Image::U16Range << " =";
                _print(ss.str());
                CONVERT(U16, Image::U16Range, U8);
                CONVERT(U16, Image::U16Range, U10);
                CONVERT(U16, Image::U16Range, U32);
                CONVERT(U16, Image::U16Range, F16);
                CONVERT(U16, Image::U16Range, F32);
            }
            
            {
                std::stringstream ss;
                ss << "U32 " << Image::U32Range << " =";
                _print(ss.str());
                CONVERT(U32, Image::U32Range, U8);
                CONVERT(U32, Image::U32Range, U10);
                CONVERT(U32, Image::U32Range, U16);
                CONVERT(U32, Image::U32Range, F16);
                CONVERT(U32, Image::U32Range, F32);
            }
            
            {
                std::stringstream ss;
                ss << "F16 " << Image::F16Range << " =";
                _print(ss.str());
                CONVERT(F16, Image::F16Range, U8);
                CONVERT(F16, Image::F16Range, U10);
                CONVERT(F16, Image::F16Range, U16);
                CONVERT(F16, Image::F16Range, U32);
                CONVERT(F16, Image::F16Range, F32);
            }
            
            {
                std::stringstream ss;
                ss << "F32 " << Image::F32Range << " =";
                _print(ss.str());
                CONVERT(F32, Image::F32Range, U8);
                CONVERT(F32, Image::F32Range, U10);
                CONVERT(F32, Image::F32Range, U16);
                CONVERT(F32, Image::F32Range, U32);
                CONVERT(F32, Image::F32Range, F16);
            }
        }
        
    } // namespace AVTest
} // namespace djv


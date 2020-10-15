// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/TypeFuncTest.h>

#include <djvImage/TypeFunc.h>

#include <djvMath/RangeFunc.h>

using namespace djv::Core;
using namespace djv::Image;

#define CONVERT(A, RANGE, B) \
    { \
        auto bMin = static_cast<Image::B##_T>(0); \
        auto bMax = static_cast<Image::B##_T>(0); \
        Image::convert_##A##_##B(RANGE.getMin(), bMin); \
        Image::convert_##A##_##B(RANGE.getMax(), bMax); \
        std::stringstream ss; \
        ss << "    " << #B << " " << static_cast<int64_t>(bMin) << " " << static_cast<int64_t>(bMax); \
        _print(ss.str()); \
    }

namespace djv
{
    namespace ImageTest
    {
        TypeFuncTest::TypeFuncTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::TypeFuncTest", tempPath, context)
        {}
        
        void TypeFuncTest::run()
        {
            _util();
            _convert();
            _serialize();
        }                
        
        void TypeFuncTest::_util()
        {
            for (const auto& i : Image::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                {
                    std::stringstream ss2;
                    ss2 << Image::getChannels(i);
                    _print(_getText(ss.str()) + " channels: " + _getText(ss2.str()));
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getChannelCount(i);
                    _print(_getText(ss.str()) + " channel count: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getDataType(i);
                    _print(_getText(ss.str()) + " data type: " + _getText(ss2.str()));
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getBitDepth(i);
                    _print(_getText(ss.str()) + " bit depth: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getByteCount(i);
                    _print(_getText(ss.str()) + " byte count: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::isIntType(i);
                    _print(_getText(ss.str()) + " int type: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::isFloatType(i);
                    _print(_getText(ss.str()) + " float type: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getIntRange(i);
                    _print(_getText(ss.str()) + " int range: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getFloatRange(i);
                    _print(_getText(ss.str()) + " float range: " + ss2.str());
                }
            }
            
            for (const auto& i : Image::getDataTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                {
                    std::stringstream ss2;
                    ss2 << Image::getBitDepth(i);
                    _print(_getText(ss.str()) + " bit depth: " + ss2.str());
                }
                {
                    std::stringstream ss2;
                    ss2 << Image::getByteCount(i);
                    _print(_getText(ss.str()) + " byte count: " + ss2.str());
                }
            }
            
            for (int c : { 1, 2, 3, 4 })
            {
                for (int b : { 8, 10, 16, 32 })
                {
                    {
                        std::stringstream ss;
                        ss << Image::getIntType(c, b);
                        std::stringstream ss2;
                        ss2 << "Int type " << c << "/" << b << ": " << _getText(ss.str());
                        _print(ss2.str());
                    }
                    {
                        std::stringstream ss;
                        ss << Image::getFloatType(c, b);
                        std::stringstream ss2;
                        ss2 << "Float type " << c << "/" << b << ": " << _getText(ss.str());
                        _print(ss2.str());
                    }
                }
            }
        }
        
        void TypeFuncTest::_convert()
        {
            {
                std::stringstream ss;
                ss << Image::Type::RGBA_U8;
                DJV_ASSERT("image_type_rgba_u8" == ss.str());
                Image::Type type;
                ss >> type;
                DJV_ASSERT(Image::Type::RGBA_U8 == type);
            }
            
            {
                std::stringstream ss;
                ss << "U8 " << static_cast<int>(Image::U8Range.getMin()) << " " <<
                    static_cast<int>(Image::U8Range.getMax()) << " =";
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

        void TypeFuncTest::_serialize()
        {
            {
                Image::Type value = Image::Type::RGBA_F32;
                rapidjson::Document document;
                auto& allocator = document.GetAllocator();
                auto json = toJSON(value, allocator);
                Image::Type value2 = Image::Type::None;
                fromJSON(json, value2);
                DJV_ASSERT(value == value2);
            }

            try
            {
                auto json = rapidjson::Value(rapidjson::kObjectType);
                Image::Type value = Image::Type::None;
                fromJSON(json, value);
                DJV_ASSERT(true);
            }
            catch (const std::exception&)
            {}
        }
        
    } // namespace ImageTest
} // namespace djv


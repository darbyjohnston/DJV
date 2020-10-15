// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvImageTest/TypeTest.h>

#include <djvImage/TypeFunc.h>

#include <djvMath/RangeFunc.h>

using namespace djv::Core;
using namespace djv::Image;

namespace djv
{
    namespace ImageTest
    {
        TypeTest::TypeTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITest("djv::ImageTest::TypeTest", tempPath, context)
        {}
        
        void TypeTest::run()
        {
            _enum();
            _constants();
        }
                
        void TypeTest::_enum()
        {
            for (auto i : Image::getTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Type: " + _getText(ss.str()));
            }
            
            for (auto i : Image::getChannelsEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Channels: " + _getText(ss.str()));
            }
            
            for (auto i : Image::getDataTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Data type: " + _getText(ss.str()));
            }
        }
        
        void TypeTest::_constants()
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
        
    } // namespace ImageTest
} // namespace djv


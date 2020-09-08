// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUITest/EnumTest.h>

#include <djvUI/Enum.h>

using namespace djv::Core;
using namespace djv::UI;

namespace djv
{
    namespace UITest
    {
        EnumTest::EnumTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITest("djv::UITest::EnumTest", tempPath, context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getOrientationEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Orientation: " + _getText(ss.str()));
            }

            for (auto i : getOrientationEnums())
            {
                std::stringstream ss;
                ss << getOpposite(i);
                _print("Orientation opposite: " + _getText(ss.str()));
            }
            
            for (auto i : getSideEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Side: " + _getText(ss.str()));
            }
            
            for (auto i : getExpandEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Expand: " + _getText(ss.str()));
            }
            
            for (auto i : getHAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Horizontal align: " + _getText(ss.str()));
            }
            
            for (auto i : getVAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Vertical align: " + _getText(ss.str()));
            }
            
            for (auto i : getTextHAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Text horizontal align: " + _getText(ss.str()));
            }
            
            for (auto i : getTextVAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Text vertical align: " + _getText(ss.str()));
            }
            
            for (auto i : getSelectionTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Selection type: " + _getText(ss.str()));
            }
            
            for (auto i : getSortOrderEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Sort order: " + _getText(ss.str()));
            }
            
            for (auto i : getButtonTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Button type: " + _getText(ss.str()));
            }
            
            for (auto i : getColorRoleEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Color role: " + _getText(ss.str()));
            }
            
            for (auto i : getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Metrics role: " + _getText(ss.str()));
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Image aspect ratio: " + _getText(ss.str()));
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << getImageAspectRatio(i);
                _print("Image aspect ratio: " + ss.str());
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << getPixelAspectRatio(i, 1.F);
                _print("Pixel aspect ratio: " + ss.str());
            }

            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << getAspectRatioScale(i, 1.F);
                _print("Aspect ratio scale: " + ss.str());
            }
            
            for (auto i : getPopupEnums())
            {
                std::stringstream ss;
                ss << i;
                _print("Popup: " + _getText(ss.str()));
            }
        }

    } // namespace UITest
} // namespace djv


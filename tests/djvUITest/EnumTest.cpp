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
        EnumTest::EnumTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::UITest::EnumTest", context)
        {}
        
        void EnumTest::run()
        {
            for (auto i : getOrientationEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "orientation string: " << _getText(ss.str());
                _print(ss2.str());
            }

            for (auto i : getOrientationEnums())
            {
                std::stringstream ss;
                ss << getOpposite(i);
                std::stringstream ss2;
                ss2 << "orientation opposite: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getSideEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "side string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getExpandEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "expand string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getHAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "horizontal align string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getVAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "vertical align string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getTextHAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "text horizontal align string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getTextVAlignEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "text vertical align string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getSelectionTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "selection type string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getSortOrderEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "sort order string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getButtonTypeEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "button type string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getColorRoleEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "color role string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getMetricsRoleEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "metrics role string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "image aspect ratio string: " << _getText(ss.str());
                _print(ss2.str());
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << "image aspect ratio: " << getImageAspectRatio(i);
                _print(ss.str());
            }
            
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << "pixel aspect ratio: " << getPixelAspectRatio(i, 1.F);
                _print(ss.str());
            }

            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << "aspect ratio scale: " << getAspectRatioScale(i, 1.F);
                _print(ss.str());
            }
            
            for (auto i : getPopupEnums())
            {
                std::stringstream ss;
                ss << i;
                std::stringstream ss2;
                ss2 << "popup string: " << _getText(ss.str());
                _print(ss2.str());
            }
        }

    } // namespace UITest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <string>
#include <vector>

namespace djv
{
    //! This namespace provides OpenColorIO functionality.
    namespace OCIO
    {
        //! This struct provides a color space conversion.
        class Convert
        {
        public:
            Convert();
            Convert(const std::string&, const std::string&);

            std::string input;
            std::string output;

            bool isValid() const;

            bool operator == (const Convert&) const;
            bool operator < (const Convert&) const;
        };

        //! This struct provides information about a view.
        class View
        {
        public:
            View();
            
            std::string name;
            std::string colorSpace;
            std::string looks;

            bool operator == (const View&) const;
        };

        //! This struct provides information about a display.
        class Display
        {
        public:
            Display();
            
            std::string name;
            std::string defaultView;
            std::vector<View> views;

            bool operator == (const Display&) const;
        };

    } // namespace OCIO
} // namespace djv

#include <djvOCIO/OCIOInline.h>

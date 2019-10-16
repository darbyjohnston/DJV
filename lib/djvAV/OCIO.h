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

#pragma once

#include <djvAV/AV.h>

#include <string>
#include <vector>

namespace djv
{
    namespace AV
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
    } // namespace AV
} // namespace djv

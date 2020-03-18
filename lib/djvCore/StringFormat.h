//------------------------------------------------------------------------------
// Copyright (c) 2020 Darby Johnston
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

#include <djvCore/Core.h>

#include <string>

namespace djv
{
    namespace Core
    {
        namespace String
        {
            //! \class Format
            //! Example:
            //!
            //! std::string result = Format("Testing {2} {1} {0}").arg("one").arg("two").arg("three");
            //!
            //! Results in the string, "Testing three two one".
            class Format
            {
            public:
                Format(const std::string&);

                //! Replace the next argument in the string with the given value.
                //! Arguments consist of an integer enclosed by curly brackets.
                //! The argument with the smallest integer will be replaced. The
                //! object is returned so that you can chain calls together.
                Format& arg(const std::string&);
                Format& arg(int);
                Format& arg(float);
                Format& arg(size_t);

                bool hasError() const;
                const std::string& getError() const;

                operator std::string() const;

            private:
                std::string _text;
                std::string _error;
            };

        } // namespace String
    } // namespace Core
} // namespace djv

#include <djvCore/StringFormatInline.h>

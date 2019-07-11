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

#include <djvCore/Core.h>

#include <set>
#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        //! This namespace provides string functionality.
        namespace String
        {
            //! Default size for char buffers.
            static const size_t cStringLength = 255;

            //! \name Splitting and Joining
            ///@{

            inline std::vector<std::string> split(const char *, size_t, char delimeter, bool keepEmpty = false);
            inline std::vector<std::string> split(const std::string &, char delimeter, bool keepEmpty = false);
            inline std::vector<std::string> split(const std::string &, const std::vector<char> & delimeters, bool keepEmpty = false);

            inline std::string join(const std::vector<std::string> &);
            inline std::string join(const std::vector<std::string> &, char delimeter);
            inline std::string join(const std::vector<std::string> &, const std::string & delimeter);

            inline std::string joinSet(const std::set<std::string> &);
            inline std::string joinSet(const std::set<std::string> &, char delimeter);
            inline std::string joinSet(const std::set<std::string> &, const std::string & delimeter);

            ///@}

            //! \name Formatting
            ///@{

            //! Upper case.
            std::string toUpper(const std::string &);

            //! Lower case.
            std::string toLower(const std::string &);

            //! Create a string for indenting.
            std::string indent(size_t indent, size_t count = 4);

            //! Remove a trailing newline.
            void removeTrailingNewline(std::string &);

            ///@}

            //! \name Comparison
            ///@{

            bool match(const std::string & value, const std::string & expression);

            ///@}

            //! \name Conversion
            ///@{

            //! Low-level function for converting an integer type to a string.
            template<typename T>
            inline size_t intToString(
                T      value,
                char * string,
                size_t maxLen = cStringLength);

            //! Low-level function for converting a string to an integer type.
            inline void fromString(const char *, size_t size, int &);

            //! Low-level function for converting a string to an integer type.
            inline void fromString(const char *, size_t size, int64_t &);

            //! Low-level function for converting a string to an integer type.
            inline void fromString(const char *, size_t size, size_t &);

            //! Low-level function for converting a string to a floating-point type.
            void fromString(const char *, size_t size, float &);

            //! Convert a regular string to a wide string.
            //! Throws:
            //! - std::exception
            std::wstring toWide(const std::string &);

            //! Convert a wide string to a regular string.
            //! Throws:
            //! - std::exception
            std::string fromWide(const std::wstring &);

            //! Replace '\' with '\\'.
            std::string escape(const std::string &);

            //! Replace '\\' with '\'.
            std::string unescape(const std::string &);

            ///@}

            //! \name Testing
            ///@{

            std::string getAlphabetLower();
            std::string getAlphabetUpper();

            std::vector<std::string> getTestNames();
            std::vector<std::string> getRandomNames(size_t);
            std::string getRandomName();

            std::vector<std::string> getTestSentences();
            std::string getRandomSentence();
            std::vector<std::string> getRandomSentences(size_t);
            std::string getRandomText(size_t sentenceCount);

            ///@}

        } // namespace String
    } // namespace Core
} // namespace djv

#include <djvCore/StringInline.h>

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <list>
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

            std::vector<std::string> split(const char *, size_t, char delimeter, bool keepEmpty = false);
            std::vector<std::string> split(const std::string&, char delimeter, bool keepEmpty = false);
            std::vector<std::string> split(const std::string&, const std::vector<char>& delimeters, bool keepEmpty = false);

            std::string join(const std::vector<std::string>&);
            std::string join(const std::vector<std::string>&, char delimeter);
            std::string join(const std::vector<std::string>&, const std::string& delimeter);

            std::string joinList(const std::list<std::string>&);
            std::string joinList(const std::list<std::string>&, char delimeter);
            std::string joinList(const std::list<std::string>&, const std::string& delimeter);

            std::string joinSet(const std::set<std::string>&);
            std::string joinSet(const std::set<std::string>&, char delimeter);
            std::string joinSet(const std::set<std::string>&, const std::string& delimeter);

            ///@}

            //! \name Formatting
            ///@{

            //! Upper case.
            std::string toUpper(const std::string&);

            //! Lower case.
            std::string toLower(const std::string&);

            //! Create a string for indenting.
            std::string indent(size_t indent, size_t count = 4);

            //! Remove a trailing newline.
            void removeTrailingNewline(std::string&);

            ///@}

            //! \name Comparison
            ///@{

            bool match(const std::string& value, const std::string& expression);

            ///@}

            //! \name Conversion
            ///@{

            //! Low-level function for converting an integer type to a string.
            template<typename T>
            size_t intToString(
                T      value,
                char * string,
                size_t maxLen = cStringLength);

            //! Low-level function for converting a string to an integer type.
            void fromString(const char *, size_t size, int&);

            //! Low-level function for converting a string to an integer type.
            void fromString(const char *, size_t size, int64_t&);

            //! Low-level function for converting a string to an integer type.
            void fromString(const char *, size_t size, size_t&);

            //! Low-level function for converting a string to a floating-point type.
            void fromString(const char *, size_t size, float&);

            //! Convert a regular string to a wide string.
            //! Throws:
            //! - std::exception
            std::wstring toWide(const std::string&);

            //! Convert a wide string to a regular string.
            //! Throws:
            //! - std::exception
            std::string fromWide(const std::wstring&);

            //! Replace '\' with '\\'.
            std::string escape(const std::string&);

            //! Replace '\\' with '\'.
            std::string unescape(const std::string&);

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

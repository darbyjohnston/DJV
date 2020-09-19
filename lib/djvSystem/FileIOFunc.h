// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/String.h>

#include <memory>
#include <string>
#include <vector>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class IO;
            
            //! Read the contents from a file.
            //! Throws:
            //! - Error
            std::string readContents(const std::shared_ptr<IO>&);

            //! Read a word from a file.
            //! Throws:
            //! - Error
            void readWord(const std::shared_ptr<IO>&, char*, size_t maxLen = Core::String::cStringLength);

            //! Read a line from a file.
            //! Throws:
            //! - Error
            //! \todo Should we handle comments like readWord()?
            void readLine(const std::shared_ptr<IO>&, char*, size_t maxLen = Core::String::cStringLength);

            //! Read all the lines from a file.
            //! Throws:
            //! - Error
            std::vector<std::string> readLines(const std::string& fileName);

            //! Write lines to a file.
            //! Throws:
            //! - Error
            void writeLines(const std::string& fileName, const std::vector<std::string>&);

        } // namespace File
    } // namespace System
} // namespace djv


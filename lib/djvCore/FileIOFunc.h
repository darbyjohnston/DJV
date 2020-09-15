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
    namespace Core
    {
        namespace FileSystem
        {
            class FileIO;
            
            //! Read the contents from a file.
            //! Throws:
            //! - Error
            std::string readContents(const std::shared_ptr<FileIO>&);

            //! Read a word from a file.
            //! Throws:
            //! - Error
            void readWord(const std::shared_ptr<FileIO>&, char*, size_t maxLen = String::cStringLength);

            //! Read a line from a file.
            //! Throws:
            //! - Error
            //! \todo Should we handle comments like readWord()?
            void readLine(const std::shared_ptr<FileIO>&, char*, size_t maxLen = String::cStringLength);

            //! Read all the lines from a file.
            //! Throws:
            //! - Error
            std::vector<std::string> readLines(const std::string& fileName);

            //! Write lines to a file.
            //! Throws:
            //! - Error
            void writeLines(const std::string& fileName, const std::vector<std::string>&);

        } // namespace FileSystem
    } // namespace Core
} // namespace djv


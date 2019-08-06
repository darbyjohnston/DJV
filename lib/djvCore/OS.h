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

#include <djvCore/Enum.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class Path;

        } // namespace FileSystem

        //! This namespace provides operating system functionality.
        namespace OS
        {
            //! Get operating system information.
            std::string getInformation();

            //! Get the total amount of RAM available.
            size_t getRAMSize();

            //! Get the width of the terminal.
            int getTerminalWidth();

            //! \name Environment Variables
            ///@{

            //! Throws:
            //! - std::exception
            std::string getEnv(const std::string & name);

            //! Get an environment variable and convert it to an integer. If the
            //! variable is empty then zero is returned.
            //!
            //! Throws:
            //! - std::exception
            int getIntEnv(const std::string & name);

            //! Throws:
            //! - std::exception
            bool setEnv(const std::string & name, const std::string &);
            
            ///@}

            //! Get the current user.
            //! Throws:
            //! - std::exception
            std::string getUserName();
            
            //! \name Directory Shortcuts
            ///@{
            
            //! This enumeration provides directory shortcuts.
            enum class DirectoryShortcut
            {
                Home,
                Desktop,
                Documents,
                Downloads,

                Count,
                First = Home
            };
            DJV_ENUM_HELPERS(DirectoryShortcut);

            FileSystem::Path getPath(DirectoryShortcut);

            ///@}
            
        } // namespace OS
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::OS::DirectoryShortcut);

} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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
            //! \name Information
            ///@{

            //! Get operating system information.
            std::string getInformation();

            //! Get the total amount of RAM available.
            size_t getRAMSize();

            //! Get the current user.
            //! Throws:
            //! - std::exception
            std::string getUserName();

            //! Get the width of the terminal.
            int getTerminalWidth();

            ///@}

            //! \name Environment Variables
            ///@{

            //! This enumeration provides the OS specific list separators.
            enum class ListSeparator
            {
                Unix,
                Windows
            };

            //! Get the list separator.
            char getListSeparator(ListSeparator);

            //! Get the current list separator.
            char getCurrentListSeparator();

            //! Throws:
            //! - std::exception
            std::string getEnv(const std::string& name);

            //! Get an environment variable and convert it to an integer. If the
            //! variable is empty then zero is returned.
            //!
            //! Throws:
            //! - std::exception
            int getIntEnv(const std::string& name);

            //! Get an environment variable and convert it to a list of strings.
            //!
            //! Throws:
            //! - std::exception
            std::vector<std::string> getStringListEnv(const std::string& name);

            //! Throws:
            //! - std::exception
            bool setEnv(const std::string& name, const std::string&);
            
            ///@}
            
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
            
            //! \name Utility
            ///@{

            //! Throws:
            //! - std::exception
            void openURL(const std::string&);

            ///@}
            
        } // namespace OS
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS(Core::OS::DirectoryShortcut);

} // namespace djv

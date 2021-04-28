// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvCore/Core.h>

#include <string>
#include <vector>

namespace djv
{
    namespace Core
    {
        //! This namespace provides operating system functionality.
        namespace OS
        {
            //! This enumeration provides OS specific environment variable
            //! list separators.
            enum class ListSeparator
            {
                Unix,
                Windows
            };
            
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

            //! Get the list separator.
            char getListSeparator(ListSeparator);

            //! Get the current list separator.
            char getCurrentListSeparator();

            //! Throws:
            //! - std::exception
            bool getEnv(const std::string& name, std::string&);

            //! Get an environment variable and convert it to an integer. If the
            //! variable is empty then zero is returned.
            //!
            //! Throws:
            //! - std::exception
            bool getIntEnv(const std::string& name, int& value);

            //! Get an environment variable and convert it to a list of strings.
            //!
            //! Throws:
            //! - std::exception
            bool getStringListEnv(const std::string& name, std::vector<std::string>&);

            //! Throws:
            //! - std::exception
            bool setEnv(const std::string& name, const std::string&);
            
            //! Throws:
            //! - std::exception
            bool clearEnv(const std::string& name);
            
            ///@}
            
            //! \name Utility
            ///@{

            //! Throws:
            //! - std::exception
            void openURL(const std::string&);

            ///@}
            
        } // namespace OS
    } // namespace Core
} // namespace djv

#include <djvCore/OSInline.h>

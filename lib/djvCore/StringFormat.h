// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

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

                //! \name Arguments
                //! Replace the next argument in the string with the given value.
                //! Arguments consist of an integer enclosed by curly brackets.
                //! The argument with the smallest integer will be replaced. The
                //! object is returned so that you can chain calls together.
                ///@{

                Format& arg(const std::string&);
                Format& arg(int);
                Format& arg(float);
                Format& arg(size_t);

                ///@}
                
                //! \name Errors
                ///@{

                bool hasError() const;
                const std::string& getError() const;

                ///@}

                operator std::string() const;

            private:
                std::string _text;
                std::string _error;
            };

        } // namespace String
    } // namespace Core
} // namespace djv


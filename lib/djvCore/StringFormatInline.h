// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Core
    {
        namespace String
        {
            inline Format::Format(const std::string& value) :
                _text(value)
            {}

            inline bool Format::hasError() const
            {
                return _error.size() > 0;
            }

            inline const std::string& Format::getError() const
            {
                return _error;
            }

            inline Format::operator std::string() const
            {
                return _text;
            }

        } // namespace String
    } // namespace Core
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Cineon.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace Cineon
            {
                //! Check whether the value is valid.
                bool isValid(const char*, size_t size);
                
                //! Convert to std::string.
                std::string toString(const char* in, size_t size);
                
                //! Convert from std::string.
                size_t fromString(
                    const std::string& string,
                    char*              out,
                    size_t             maxLen,
                    bool               terminate);
                    
                //! Zero out the data in a Cineon file header.
                void zero(Header&);
                
                //! Convert then endian of a Cineon file header.
                void convertEndian(Header&);

                //! Read a Cineon file header.
                //!
                //! Throws:
                //! - System::File::Error
                Header read(
                    const std::shared_ptr<System::File::IO>&,
                    Info&,
                    ColorProfile&,
                    const std::shared_ptr<System::TextSystem>&);
                
                //! Write a Cineon file header.
                //!
                //! Throws:
                //! - System::File::Error
                void write(
                    const std::shared_ptr<System::File::IO>&,
                    const Info& info,
                    ColorProfile);

                //! Finish writing the Cineon file header after image data is written.
                void writeFinish(const std::shared_ptr<System::File::IO>&);

                DJV_ENUM_HELPERS(ColorProfile);
                DJV_ENUM_HELPERS(Orient);
                DJV_ENUM_HELPERS(Descriptor);

            } // namespace Cineon
        } // namespace IO
    } // namespace AV
} // namespace djv

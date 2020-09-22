// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/PPM.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace PPM
            {
                //! Get the number of bytes in a scanline.
                size_t getScanlineByteCount(
                    int    width,
                    size_t channelCount,
                    size_t bitDepth);

                //! Read PPM file ASCII data.
                void readASCII(
                    const std::shared_ptr<System::File::IO>& io,
                    uint8_t* out,
                    size_t size,
                    size_t componentSize);

                //! Save PPM file ASCII data.
                size_t writeASCII(
                    const uint8_t * in,
                    char *          out,
                    size_t          size,
                    size_t          componentSize);
                
                DJV_ENUM_HELPERS(Data);

            } // namespace PPM
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::PPM::Data);
    
    rapidjson::Value toJSON(const AV::IO::PPM::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::PPM::Options&);

} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/TIFF.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace TIFF
            {
                //! Read a TIFF file palette.
                void readPalette(
                    uint8_t *  out,
                    int        size,
                    int        bytes,
                    uint16_t * red,
                    uint16_t * green,
                    uint16_t * blue);

                DJV_ENUM_HELPERS(Compression);

            } // namespace TIFF
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::TIFF::Compression);

    rapidjson::Value toJSON(const AV::IO::TIFF::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::TIFF::Options&);

} // namespace djv

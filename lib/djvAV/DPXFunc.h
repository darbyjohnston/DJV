// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/DPX.h>

namespace djv
{
    namespace AV
    {
        namespace IO
        {
            namespace DPX
            {
                //! Zero out the data in a DPX file header.
                void zero(Header&);

                //! Convert then endian of a DPX file header.
                void convertEndian(Header&);
                
                //! Read a DPX file header.
                //!
                //! Throws:
                //! - Core::FileSystem::Error
                Header read(
                    const std::shared_ptr<Core::FileSystem::FileIO>&,
                    Info&,
                    Transfer&,
                    const std::shared_ptr<Core::TextSystem>&);
                
                //! Write a DPX file header.
                //!
                //! Throws:
                //! - Core::FileSystem::Error
                void write(
                    const std::shared_ptr<Core::FileSystem::FileIO>&,
                    const Info&,
                    Version,
                    Endian,
                    Transfer);

                //! Finish writing the DPX file header after image data is written.
                void writeFinish(const std::shared_ptr<Core::FileSystem::FileIO>&);

                DJV_ENUM_HELPERS(Version);
                DJV_ENUM_HELPERS(Endian);
                DJV_ENUM_HELPERS(Orient);
                DJV_ENUM_HELPERS(Transfer);
                DJV_ENUM_HELPERS(Components);

            } // namespace DPX
        } // namespace IO
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::DPX::Version);
    DJV_ENUM_SERIALIZE_HELPERS(AV::IO::DPX::Endian);

    rapidjson::Value toJSON(const AV::IO::DPX::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, AV::IO::DPX::Options&);

} // namespace djv

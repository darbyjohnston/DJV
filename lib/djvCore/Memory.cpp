// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Memory.h>

#include <algorithm>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            std::string getSizeLabel(uint64_t value)
            {
                std::stringstream ss;
                ss.precision(2);
                if (value >= terabyte)
                {
                    ss << std::fixed << static_cast<double>(value) / static_cast<double>(terabyte);
                }
                else if (value >= gigabyte)
                {
                    ss << std::fixed << static_cast<double>(value) / static_cast<double>(gigabyte);
                }
                else if (value >= megabyte)
                {
                    ss << std::fixed << static_cast<double>(value) / static_cast<double>(megabyte);
                }
                else
                {
                    ss << std::fixed << static_cast<double>(value) / static_cast<double>(kilobyte);
                }
                return ss.str();
            }

            std::string getUnitLabel(uint64_t value)
            {
                std::stringstream ss;
                if (value >= terabyte)
                {
                    ss << Unit::TB;
                }
                else if (value >= gigabyte)
                {
                    ss << Unit::GB;
                }
                else if (value >= megabyte)
                {
                    ss << Unit::MB;
                }
                else
                {
                    ss << Unit::KB;
                }
                return ss.str();
            }

        } // namespace Memory
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Memory,
        Unit,
        DJV_TEXT("memory_unit_kilobyte"),
        DJV_TEXT("memory_unit_megabyte"),
        DJV_TEXT("memory_unit_gigabyte"),
        DJV_TEXT("memory_unit_terabyte"));

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Memory,
        Endian,
        DJV_TEXT("endian_msb"),
        DJV_TEXT("endian_lsb"));

} // namespace djv

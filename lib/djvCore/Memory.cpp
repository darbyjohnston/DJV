// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Memory.h>

#include <algorithm>
#include <array>
#include <functional>

#include <string.h>

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

            Endian getEndian() noexcept
            {
                const int tmp = 1;
                const uint8_t* const p = reinterpret_cast<const uint8_t*>(&tmp);
                const Endian endian = *p ? Endian::LSB : Endian::MSB;
                return endian;
            }
            
            Endian opposite(Endian in) noexcept
            {
                return Endian::MSB == in ? Endian::LSB : Endian::MSB;
            }

            void endian(
                void*  in,
                size_t size,
                size_t wordSize) noexcept
            {
                uint8_t* p = reinterpret_cast<uint8_t*>(in);
                uint8_t tmp;
                switch (wordSize)
                {
                case 2:
                    while (size--)
                    {
                        tmp  = p[0];
                        p[0] = p[1];
                        p[1] = tmp;
                        p += 2;
                    }
                    break;
                case 4:
                    while (size--)
                    {
                        tmp  = p[0];
                        p[0] = p[3];
                        p[3] = tmp;
                        tmp  = p[1];
                        p[1] = p[2];
                        p[2] = tmp;
                        p += 4;
                    }
                    break;
                case 8:
                    while (size--)
                    {
                        tmp  = p[0];
                        p[0] = p[7];
                        p[7] = tmp;
                        tmp  = p[1];
                        p[1] = p[6];
                        p[6] = tmp;
                        tmp  = p[2];
                        p[2] = p[5];
                        p[5] = tmp;
                        tmp  = p[3];
                        p[3] = p[4];
                        p[4] = tmp;
                        p += 8;
                    }
                    break;
                default: break;
                }
            }

            void endian(
                const void* in,
                void*       out,
                size_t      size,
                size_t      wordSize) noexcept
            {
                const uint8_t* inP = reinterpret_cast<const uint8_t*>(in);
                uint8_t* outP = reinterpret_cast<uint8_t*>(out);
                switch (wordSize)
                {
                case 2:
                    while (size--)
                    {
                        outP[0] = inP[1];
                        outP[1] = inP[0];
                        
                        inP  += 2;
                        outP += 2;
                    }
                    break;
                case 4:
                    while (size--)
                    {
                        outP[0] = inP[3];
                        outP[1] = inP[2];
                        outP[2] = inP[1];
                        outP[3] = inP[0];
                        
                        inP  += 4;
                        outP += 4;
                    }
                    break;
                case 8:
                    while (size--)
                    {
                        outP[0] = inP[7];
                        outP[1] = inP[6];
                        outP[2] = inP[5];
                        outP[3] = inP[4];
                        outP[4] = inP[3];
                        outP[5] = inP[2];
                        outP[6] = inP[1];
                        outP[7] = inP[0];
                        
                        inP  += 8;
                        outP += 8;
                    }
                    break;
                default:
                    memcpy(out, in, size * wordSize);
                    break;
                }
            }

            DJV_ENUM_HELPERS_IMPLEMENTATION(Unit);
            DJV_ENUM_HELPERS_IMPLEMENTATION(Endian);

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

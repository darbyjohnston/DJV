//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCore/Memory.h>

#include <algorithm>
#include <sstream>

namespace djv
{
    namespace Core
    {
        namespace Memory
        {
            std::string getSizeLabel(uint64_t value)
            {
                static const std::vector<std::string> data = { "TB", "GB", "MB", "KB" };
                std::stringstream ss;
                ss.precision(2);
                if (value >= terabyte)
                {
                    ss << std::fixed << value / static_cast<double>(terabyte);
                    ss << data[0];
                }
                else if (value >= gigabyte)
                {
                    ss << std::fixed << value / static_cast<double>(gigabyte);
                    ss << data[1];
                }
                else if (value >= megabyte)
                {
                    ss << std::fixed << value / static_cast<double>(megabyte);
                    ss << data[2];
                }
                else
                {
                    ss << std::fixed << value / static_cast<double>(kilobyte);
                    ss << data[3];
                }
                return ss.str();
            }

            Endian getEndian()
            {
                static const int tmp = 1;
                static const uint8_t* const p = reinterpret_cast<const uint8_t*>(&tmp);
                return *p ? Endian::LSB : Endian::MSB;
            }

        } // namespace Memory
    } // namespace Core

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Core::Memory::,
        Endian,
        DJV_TEXT("MSB"),
        DJV_TEXT("LSB"));

} // namespace djv

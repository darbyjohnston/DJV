//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvCoreTest/MemoryTest.h>

#include <djvCore/Memory.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MemoryTest::MemoryTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MemoryTest", context)
        {}
        
        void MemoryTest::run(const std::vector<std::string>& args)
        {
            _label();
            _enum();
            _endian();
            _hash();
        }
        
        void MemoryTest::_label()
        {
            for (const auto& i :
                {
                    Memory::kilobyte,
                    Memory::megabyte,
                    Memory::gigabyte,
                    Memory::terabyte
                })
            {
                std::stringstream ss;
                ss << "size label: " << Memory::getSizeLabel(i);
                _print(ss.str());                
            }
        }
        
        void MemoryTest::_enum()
        {
            for (auto i : Memory::getEndianEnums())
            {
                std::stringstream ss;
                ss << "endian string: " << i;
                _print(ss.str());
            }
        }
        
        void MemoryTest::_endian()
        {
            {
                std::stringstream ss;
                ss << "current endian: " << Memory::getEndian();
                _print(ss.str());
            }
            
            {
                std::stringstream ss;
                ss << "opposite endian: " << Memory::opposite(Memory::getEndian());
                _print(ss.str());
            }
            
            {
                uint8_t data = 0;
                Memory::endian(&data, 1, sizeof(data));
                DJV_ASSERT(0 == data);
                uint8_t data2 = 0;
                Memory::endian(&data, &data2, 1, sizeof(data));
                DJV_ASSERT(0 == data2);
            }
            
            {
                uint16_t data = 0;
                uint8_t* p = reinterpret_cast<uint8_t*>(&data);
                p[0] = 0;
                p[1] = 1;
                Memory::endian(&data, 1, sizeof(data));
                DJV_ASSERT(1 == p[0]);
                DJV_ASSERT(0 == p[1]);
                uint16_t data2 = 0;
                uint8_t* p2 = reinterpret_cast<uint8_t*>(&data2);
                Memory::endian(&data, &data2, 1, sizeof(data));
                DJV_ASSERT(0 == p2[0]);
                DJV_ASSERT(1 == p2[1]);
            }
            
            {
                uint32_t data = 0;
                uint8_t* p = reinterpret_cast<uint8_t*>(&data);
                p[0] = 0;
                p[1] = 1;
                p[2] = 2;
                p[3] = 3;
                Memory::endian(&data, 1, sizeof(data));
                DJV_ASSERT(3 == p[0]);
                DJV_ASSERT(2 == p[1]);
                DJV_ASSERT(1 == p[2]);
                DJV_ASSERT(0 == p[3]);
                uint32_t data2 = 0;
                uint8_t* p2 = reinterpret_cast<uint8_t*>(&data2);
                Memory::endian(&data, &data2, 1, sizeof(data));
                DJV_ASSERT(0 == p2[0]);
                DJV_ASSERT(1 == p2[1]);
                DJV_ASSERT(2 == p2[2]);
                DJV_ASSERT(3 == p2[3]);
            }
            
            {
                uint64_t data = 0;
                uint8_t* p = reinterpret_cast<uint8_t*>(&data);
                p[0] = 0;
                p[1] = 1;
                p[2] = 2;
                p[3] = 3;
                p[4] = 4;
                p[5] = 5;
                p[6] = 6;
                p[7] = 7;
                Memory::endian(&data, 1, sizeof(data));
                DJV_ASSERT(7 == p[0]);
                DJV_ASSERT(6 == p[1]);
                DJV_ASSERT(5 == p[2]);
                DJV_ASSERT(4 == p[3]);
                DJV_ASSERT(3 == p[4]);
                DJV_ASSERT(2 == p[5]);
                DJV_ASSERT(1 == p[6]);
                DJV_ASSERT(0 == p[7]);
                uint64_t data2 = 0;
                uint8_t* p2 = reinterpret_cast<uint8_t*>(&data2);
                Memory::endian(&data, &data2, 1, sizeof(data));
                DJV_ASSERT(0 == p2[0]);
                DJV_ASSERT(1 == p2[1]);
                DJV_ASSERT(2 == p2[2]);
                DJV_ASSERT(3 == p2[3]);
                DJV_ASSERT(4 == p2[4]);
                DJV_ASSERT(5 == p2[5]);
                DJV_ASSERT(6 == p2[6]);
                DJV_ASSERT(7 == p2[7]);
            }
        }
        
        void MemoryTest::_hash()
        {
            size_t hash = 0;
            Memory::hashCombine(hash, 100);
            {
                std::stringstream ss;
                ss << "hash: " << hash;
                _print(ss.str());
            }
            
            Memory::hashCombine(hash, 100.f);
            {
                std::stringstream ss;
                ss << "hash: " << hash;
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv


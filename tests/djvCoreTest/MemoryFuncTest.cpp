// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCoreTest/MemoryFuncTest.h>

#include <djvCore/MemoryFunc.h>

#include <iostream>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        MemoryFuncTest::MemoryFuncTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Core::Context>& context) :
            ITest("djv::CoreTest::MemoryFuncTest", tempPath, context)
        {}
        
        void MemoryFuncTest::run()
        {
            _label();
            _endian();
            _hash();
        }
        
        void MemoryFuncTest::_label()
        {
            for (const auto& i :
                {
                    Memory::kilobyte,
                    Memory::megabyte,
                    Memory::gigabyte,
                    Memory::terabyte
                })
            {
                _print("Size: " + _getText(Memory::getSizeLabel(i)));
            }

            for (const auto& i :
                {
                    Memory::kilobyte,
                    Memory::megabyte,
                    Memory::gigabyte,
                    Memory::terabyte
                })
            {
                _print("Unit: " + _getText(Memory::getUnitLabel(i)));
            }
        }
        
        void MemoryFuncTest::_endian()
        {
            {
                std::stringstream ss;
                ss << Memory::getEndian();
                _print("Current endian: " + _getText(ss.str()));
            }
            
            {
                std::stringstream ss;
                ss << Memory::opposite(Memory::getEndian());
                _print("Opposite endian: " + _getText(ss.str()));
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
        
        void MemoryFuncTest::_hash()
        {
            size_t hash = 0;
            Memory::hashCombine(hash, 100);
            {
                std::stringstream ss;
                ss << "Hash: " << hash;
                _print(ss.str());
            }
            
            Memory::hashCombine(hash, 100.f);
            {
                std::stringstream ss;
                ss << "Hash: " << hash;
                _print(ss.str());
            }
        }
        
    } // namespace CoreTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    namespace AVTest
    {
        class IOTest : public Test::ITest
        {
        public:
            IOTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _info();
            void _videoFrame();
            void _videoQueue();
            void _audioFrame();
            void _audioQueue();
            void _cache();
            void _io();
            void _system();
        };
        
    } // namespace AVTest
} // namespace djv


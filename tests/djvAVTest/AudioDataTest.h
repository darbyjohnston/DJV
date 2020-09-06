// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    //! This namespace provides confidence tests for the AV library.
    namespace AVTest
    {
        class AudioDataTest : public Test::ITest
        {
        public:
            AudioDataTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _info();
            void _data();
            void _util();
            void _operators();
        };
        
    } // namespace AVTest
} // namespace djv


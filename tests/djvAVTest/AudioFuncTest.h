// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

namespace djv
{
    //! This namespace provides confidence tests for the AV library.
    namespace AVTest
    {
        class AudioFuncTest : public Test::ITest
        {
        public:
            AudioFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _convert();
        };
        
    } // namespace AVTest
} // namespace djv


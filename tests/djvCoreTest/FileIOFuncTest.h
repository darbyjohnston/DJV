// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class FileIOFuncTest : public Test::ITest
        {
        public:
            FileIOFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            std::string _fileName;
            std::string _text;
            std::string _text2;
        };
        
    } // namespace CoreTest
} // namespace djv


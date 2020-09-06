// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class FileIOTest : public Test::ITest
        {
        public:
            FileIOTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _io();
            void _error();
            void _endian();
            void _temp();

            std::string _fileName;
            std::string _text;
            std::string _text2;
        };
        
    } // namespace CoreTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

namespace djv
{
    namespace CoreTest
    {
        class FileInfoTest : public Test::ITest
        {
        public:
            FileInfoTest(const std::shared_ptr<Core::Context>&);
            
            void run() override;

        private:
            void _enum();
            void _ctor();
            void _path();
            void _sequences();
            void _util();
            void _operators();
            void _serialize();

            std::string _fileName;
            std::string _sequenceName;
        };
        
    } // namespace CoreTest
} // namespace djv


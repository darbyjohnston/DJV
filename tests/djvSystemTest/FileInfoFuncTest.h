// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

#include <djvMath/FrameNumber.h>

namespace djv
{
    namespace SystemTest
    {
        class FileInfoFuncTest : public Test::ITest
        {
        public:
            FileInfoFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _enum();
            void _util();
            void _serialize();

            std::string _fileName;
            std::string _sequenceName;
            Math::Frame::Sequence _sequence;
        };
        
    } // namespace SystemTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvTestLib/Test.h>

#include <djvMath/FrameNumber.h>

namespace djv
{
    namespace SystemTest
    {
        class FileInfoTest : public Test::ITest
        {
        public:
            FileInfoTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;

        private:
            void _ctor();
            void _path();
            void _sequences();
            void _operators();

            std::string _fileName;
            std::string _sequenceName;
            Math::Frame::Sequence _sequence;
        };
        
    } // namespace SystemTest
} // namespace djv


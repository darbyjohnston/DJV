// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvAV/Cineon.h>

namespace djv
{
    namespace AVTest
    {
        class CineonFuncTest : public Test::ITest
        {
        public:
            CineonFuncTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _util();
            void _header();
            void _headerIO(
                AV::IO::Cineon::Header&,
                AV::IO::Info&,
                AV::IO::Cineon::ColorProfile = AV::IO::Cineon::ColorProfile::First);
        };
        
    } // namespace AVTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvAV/Cineon.h>

namespace djv
{
    namespace AVTest
    {
        class CineonTest : public Test::ITest
        {
        public:
            CineonTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _util();
            void _header();
            void _headerIO(
                AV::Cineon::Header&,
                AV::IO::Info&,
                AV::Cineon::ColorProfile = AV::Cineon::ColorProfile::First);
        };
        
    } // namespace AVTest
} // namespace djv


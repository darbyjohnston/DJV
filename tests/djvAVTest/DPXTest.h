// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvAV/DPX.h>

namespace djv
{
    namespace AVTest
    {
        class DPXTest : public Test::ITest
        {
        public:
            DPXTest(
                const System::File::Path& tempPath,
                const std::shared_ptr<System::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _header();
            void _headerIO(
                AV::DPX::Header&,
                AV::IO::Info&,
                AV::DPX::Version = AV::DPX::Version::_2_0,
                AV::DPX::Endian = AV::DPX::Endian::Auto,
                AV::DPX::Transfer = AV::DPX::Transfer::First);
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv


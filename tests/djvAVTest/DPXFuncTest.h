// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvAV/DPX.h>

namespace djv
{
    namespace AVTest
    {
        class DPXFuncTest : public Test::ITest
        {
        public:
            DPXFuncTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _enum();
            void _header();
            void _headerIO(
                AV::IO::DPX::Header&,
                AV::IO::Info&,
                AV::IO::DPX::Version = AV::IO::DPX::Version::_2_0,
                AV::IO::DPX::Endian = AV::IO::DPX::Endian::Auto,
                AV::IO::DPX::Transfer = AV::IO::DPX::Transfer::First);
            void _serialize();
        };
        
    } // namespace AVTest
} // namespace djv


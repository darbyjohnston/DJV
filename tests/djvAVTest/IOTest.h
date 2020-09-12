// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvTestLib/Test.h>

#include <djvAV/IOSystem.h>

namespace djv
{
    namespace AVTest
    {
        class IOTest : public Test::ITest
        {
        public:
            IOTest(
                const Core::FileSystem::Path& tempPath,
                const std::shared_ptr<Core::Context>&);
            
            void run() override;
            
        private:
            void _info();
            void _videoFrame();
            void _videoQueue();
            void _audioFrame();
            void _audioQueue();
            void _inOutPoints();
            void _cache();
            void _plugin();
            void _io();
            void _io(
                const std::string& name,
                const std::string& extension,
                const AV::Image::Size,
                AV::Image::Type,
                const AV::Tags&,
                const std::shared_ptr<AV::IO::System>&);
            void _system();
        };
        
    } // namespace AVTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ThumbnailSystemTest.h>

#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvCore/Context.h>
#include <djvCore/ResourceSystem.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ThumbnailSystemTest::ThumbnailSystemTest(
            const FileSystem::Path& tempPath,
            const std::shared_ptr<Context>& context) :
            ITickTest("djv::AVTest::ThumbnailSystemTest", tempPath, context)
        {}
        
        void ThumbnailSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto resourceSystem = context->getSystemT<ResourceSystem>();
                const FileSystem::FileInfo fileInfo(FileSystem::Path(
                    resourceSystem->getPath(FileSystem::ResourcePath::Icons),
                    "96DPI/djvIconFile.png"));
                auto system = context->getSystemT<ThumbnailSystem>();
                auto infoFuture = system->getInfo(fileInfo);
                auto imageFuture = system->getImage(fileInfo, Image::Size(32, 32));
                
                auto infoCancelFuture = system->getInfo(fileInfo);
                auto imageCancelFuture = system->getImage(fileInfo, Image::Size(32, 32));
                system->cancelInfo(infoCancelFuture.uid);
                system->cancelImage(imageCancelFuture.uid);
                
                IO::Info info;
                std::shared_ptr<Image::Image> image;
                while (
                    infoFuture.future.valid() ||
                    imageFuture.future.valid())
                {
                    _tickFor(Time::getTime(Time::TimerValue::Fast));
                    if (infoFuture.future.valid() &&
                        infoFuture.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        info = infoFuture.future.get();
                    }
                    if (imageFuture.future.valid() &&
                        imageFuture.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        image = imageFuture.future.get();
                    }
                }
                
                if (info.video.size())
                {
                    std::stringstream ss;
                    ss << "Info: " << info.video[0].size;
                    _print(ss.str());
                }
                if (image)
                {
                    std::stringstream ss;
                    ss << "Image: " << image->getSize();
                    _print(ss.str());
                }
                
                {
                    std::stringstream ss;
                    ss << "Info cache percentage: " << system->getInfoCachePercentage();
                    _print(ss.str());
                }
                {
                    std::stringstream ss;
                    ss << "Image cache percentage: " << system->getImageCachePercentage();
                    _print(ss.str());
                }
                
                system->clearCache();
            }
        }
        
    } // namespace AVTest
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAVTest/ThumbnailSystemTest.h>

#include <djvAV/IOSystem.h>
#include <djvAV/ThumbnailSystem.h>

#include <djvImage/Info.h>

#include <djvSystem/Context.h>
#include <djvSystem/ResourceSystem.h>
#include <djvSystem/Timer.h>

#include <djvCore/Error.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        ThumbnailSystemTest::ThumbnailSystemTest(
            const System::File::Path& tempPath,
            const std::shared_ptr<System::Context>& context) :
            ITickTest("djv::AVTest::ThumbnailSystemTest", tempPath, context)
        {}
        
        void ThumbnailSystemTest::run()
        {
            if (auto context = getContext().lock())
            {
                auto resourceSystem = context->getSystemT<System::ResourceSystem>();
                auto system = context->getSystemT<ThumbnailSystem>();
                
                // Request a thumbnail.
                std::vector<ThumbnailSystem::InfoFuture> infoFutures;
                std::vector<ThumbnailSystem::ImageFuture> imageFutures;
                const System::File::Info fileInfo(System::File::Path(
                    resourceSystem->getPath(System::File::ResourcePath::Icons),
                    "96DPI/djvIconFile.png"));
                infoFutures.push_back(system->getInfo(fileInfo));
                imageFutures.push_back(system->getImage(fileInfo, Image::Size(32, 32)));
                
                // Request the same thumbnail.
                for (size_t i = 0; i < 10; ++i)
                {
                    infoFutures.push_back(system->getInfo(fileInfo));
                    imageFutures.push_back(system->getImage(fileInfo, Image::Size(32, 32)));
                }
                
                // Request different sizes of the thumbnail.
                for (const auto& i : {
                    Image::Size(16, 128),
                    Image::Size(128, 16),
                    Image::Size(0, 0) })
                {
                    infoFutures.push_back(system->getInfo(fileInfo));
                    imageFutures.push_back(system->getImage(fileInfo, i));
                }
                
                // Request a missing thumbnail.
                infoFutures.push_back(system->getInfo(System::File::Info()));
                imageFutures.push_back(system->getImage(System::File::Info(), Image::Size(32, 32)));

                // Request and cancel a thumbnail.
                auto infoCancelFuture = system->getInfo(fileInfo);
                auto imageCancelFuture = system->getImage(fileInfo, Image::Size(32, 32));
                system->cancelInfo(infoCancelFuture.uid);
                system->cancelImage(imageCancelFuture.uid);

                // Wait for and collect info.
                std::vector<IO::Info> infos;
                while (!infoFutures.empty())
                {
                    _tickFor(System::getTimerDuration(System::TimerValue::Fast));
                    auto i = infoFutures.begin();
                    while (i != infoFutures.end())
                    {
                        if (i->future.valid() &&
                            i->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                infos.push_back(i->future.get());
                            }
                            catch (const std::exception& e)
                            {
                                _print(Error::format(e.what()));
                            }
                            i = infoFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                for (const auto& i : infos)
                {
                    if (i.video.size())
                    {
                        std::stringstream ss;
                        ss << "Info: " << i.video[0].size;
                        _print(ss.str());
                    }
                }
                
                // Wait for and collect images.
                std::vector<std::shared_ptr<Image::Data> > images;
                while (!imageFutures.empty())
                {
                    _tickFor(System::getTimerDuration(System::TimerValue::Fast));
                    auto i = imageFutures.begin();
                    while (i != imageFutures.end())
                    {
                        if (i->future.valid() &&
                            i->future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            try
                            {
                                images.push_back(i->future.get());
                            }
                            catch (const std::exception& e)
                            {
                                _print(Error::format(e.what()));
                            }
                            i = imageFutures.erase(i);
                        }
                        else
                        {
                            ++i;
                        }
                    }
                }
                for (const auto& i : images)
                {
                    std::stringstream ss;
                    ss << "Image: " << i->getSize();
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


//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAVTest/IOTest.h>

#include <djvAV/IO.h>

#include <djvCore/Context.h>
#include <djvCore/Timer.h>

using namespace djv::Core;
using namespace djv::AV;

namespace djv
{
    namespace AVTest
    {
        IOTest::IOTest(const std::shared_ptr<Core::Context>& context) :
            ITest("djv::AVTest::IOTest", context)
        {}
        
        void IOTest::run(const std::vector<std::string>& args)
        {
            if (auto context = getContext().lock())
            {
                const std::set<std::string> extensions =
                {
                    ".cin",
                    ".dpx",
                    ".ppm",
                    ".png"
                };
                for (const auto& i : extensions)
                {
                    const Image::Info imageInfo(64, 64, Image::Type::RGB_U8);
                    auto image = Image::Image::create(imageInfo);
                    image->zero();
                    
                    FileSystem::Path path("IOTest");
                    path.setExtension(i);
                    auto io = context->getSystemT<AV::IO::System>();
                    {
                        IO::Info info;
                        info.video.push_back(imageInfo);
                        auto write = io->write(FileSystem::FileInfo(path), info);
                        {
                            std::lock_guard<std::mutex> lock(write->getMutex());
                            auto& writeQueue = write->getVideoQueue();
                            writeQueue.addFrame(IO::VideoFrame(0, image));
                            writeQueue.setFinished(true);
                        }
                        while (write->isRunning())
                        {}
                    }

                    {
                        auto read = io->read(FileSystem::FileInfo(path));
                        bool running = true;
                        while (running)
                        {
                            bool sleep = false;
                            {
                                std::unique_lock<std::mutex> lock(read->getMutex(), std::try_to_lock);
                                if (lock.owns_lock())
                                {
                                    auto& readQueue = read->getVideoQueue();
                                    if (!readQueue.isEmpty())
                                    {
                                        auto frame = readQueue.popFrame();
                                    }
                                    else if (readQueue.isFinished())
                                    {
                                        running = false;
                                    }
                                    else
                                    {
                                        sleep = true;
                                    }
                                }
                                else
                                {
                                    sleep = true;
                                }
                            }
                            if (sleep)
                            {
                                std::this_thread::sleep_for(Time::getMilliseconds(Time::TimerValue::Fast));
                            }
                        }
                    }
                }
            }
        }
                
    } // namespace AVTest
} // namespace djv


//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvCore/DrivesModel.h>

#include <djvCore/Path.h>
#include <djvCore/Timer.h>

#include <atomic>
#include <mutex>
#include <thread>

//#pragma optimize("", off)

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            struct DrivesModel::Private
            {
                std::vector<Path> drives;
                std::shared_ptr<ListSubject<Path> > drivesSubject;
                std::mutex mutex;
                std::thread thread;
                std::atomic<bool> running;
                std::shared_ptr<Time::Timer> timer;
            };

            void DrivesModel::_init(Context * context)
            {
                DJV_PRIVATE_PTR();

                p.drivesSubject = ListSubject<Path>::create();

                p.running = true;
                const auto timeout = Time::Timer::getValue(Time::Timer::Value::Medium);
                p.thread = std::thread(
                    [this, timeout]
                {
                    DJV_PRIVATE_PTR();
                    while (p.running)
                    {
                        const std::vector<Path> drives = _getDrives();
                        {
                            std::lock_guard<std::mutex> lock(p.mutex);
                            p.drives = drives;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                    }
                });

                p.timer = Time::Timer::create(context);
                p.timer->setRepeating(true);
                p.timer->start(
                    std::chrono::milliseconds(timeout),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    std::vector<Path> drives;
                    {
                        std::lock_guard<std::mutex> lock(p.mutex);
                        drives = p.drives;
                    }
                    p.drivesSubject->setIfChanged(drives);
                });
            }

            DrivesModel::DrivesModel() :
                _p(new Private)
            {}

            DrivesModel::~DrivesModel()
            {
                _p->running = false;
                if (_p->thread.joinable())
                {
                    _p->thread.join();
                }
            }

            std::shared_ptr<DrivesModel> DrivesModel::create(Context * context)
            {
                auto out = std::shared_ptr<DrivesModel>(new DrivesModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<Path> > DrivesModel::getDrives() const
            {
                return _p->drivesSubject;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

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

            void DrivesModel::_init(const std::shared_ptr<Context>& context)
            {
                DJV_PRIVATE_PTR();

                p.drivesSubject = ListSubject<Path>::create();

                p.running = true;
                p.thread = std::thread(
                    [this]
                {
                    DJV_PRIVATE_PTR();
                    const Time::Duration timeout = Time::getTime(Time::TimerValue::Medium);
                    while (p.running)
                    {
                        const std::vector<Path> drives = _getDrives();
                        {
                            std::lock_guard<std::mutex> lock(p.mutex);
                            p.drives = drives;
                        }
                        std::this_thread::sleep_for(timeout);
                    }
                });

                p.timer = Time::Timer::create(context);
                p.timer->setRepeating(true);
                p.timer->start(
                    Time::getTime(Time::TimerValue::Medium),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
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
                DJV_PRIVATE_PTR();
                p.running = false;
                if (p.thread.joinable())
                {
                    p.thread.join();
                }
            }

            std::shared_ptr<DrivesModel> DrivesModel::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DrivesModel>(new DrivesModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<Path> > DrivesModel::observeDrives() const
            {
                return _p->drivesSubject;
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnsn
// All rights reserved.

#include <djvSystem/DirectoryModel.h>

#include <djvSystem/DirectoryWatcher.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/TimerFunc.h>
#include <djvSystem/PathFunc.h>

#include <djvCore/OS.h>

#include <future>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            struct DirectoryModel::Private
            {
                std::shared_ptr<ValueSubject<Path> > path;
                std::shared_ptr<ListSubject<Info> > info;
                std::shared_ptr<ListSubject<std::string> > fileNames;
                std::shared_ptr<ListSubject<Path> > history;
                std::shared_ptr<ValueSubject<size_t> > historyIndex;
                size_t historyMax = 10;
                std::shared_ptr<ValueSubject<bool> > hasUp;
                std::shared_ptr<ValueSubject<bool> > hasBack;
                std::shared_ptr<ValueSubject<bool> > hasForward;
                std::shared_ptr<ValueSubject<DirectoryListOptions> > options;
                std::future<std::pair<std::vector<Info>, std::vector<std::string> > > future;
                std::shared_ptr<Timer> futureTimer;
                std::shared_ptr<DirectoryWatcher> directoryWatcher;
            };

            void DirectoryModel::_init(const std::shared_ptr<Context>& context)
            {
                DJV_PRIVATE_PTR();
                p.path = ValueSubject<Path>::create();
                p.info = ListSubject<Info>::create();
                p.fileNames = ListSubject<std::string>::create();
                p.history = ListSubject<Path>::create();
                p.historyIndex = ValueSubject<size_t>::create(0);
                p.hasUp = ValueSubject<bool>::create(false);
                p.hasBack = ValueSubject<bool>::create(false);
                p.hasForward = ValueSubject<bool>::create(false);
                p.options = ValueSubject<DirectoryListOptions>::create();

                p.futureTimer = Timer::create(context);
                p.futureTimer->setRepeating(true);

                p.directoryWatcher = DirectoryWatcher::create(context);

                auto weak = std::weak_ptr<DirectoryModel>(shared_from_this());
                p.directoryWatcher->setCallback(
                    [weak]
                {
                    if (auto model = weak.lock())
                    {
                        model->reload();
                    }
                });
            }

            DirectoryModel::DirectoryModel() :
                _p(new Private)
            {}

            DirectoryModel::~DirectoryModel()
            {}

            std::shared_ptr<DirectoryModel> DirectoryModel::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DirectoryModel>(new DirectoryModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IValueSubject<Path> > DirectoryModel::observePath() const
            {
                return _p->path;
            }

            void DirectoryModel::setPath(const Path& value)
            {
                DJV_PRIVATE_PTR();
                const auto absolute = getAbsolute(value);
                if (p.path->setIfChanged(absolute))
                {
                    p.hasUp->setIfChanged(!absolute.isRoot());
                    auto history = p.history->get();
                    while (history.size() ? (p.historyIndex->get() < history.size() - 1) : false)
                    {
                        history.pop_back();
                    }
                    const size_t historyIndex = history.size();
                    history.push_back(absolute);
                    p.history->setIfChanged(history);
                    p.historyIndex->setIfChanged(historyIndex);
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _pathUpdate();
                }
            }

            std::shared_ptr<IListSubject<Info> > DirectoryModel::observeInfo() const
            {
                return _p->info;
            }

            std::shared_ptr<IListSubject<std::string> > DirectoryModel::observeFileNames() const
            {
                return _p->fileNames;
            }

            void DirectoryModel::reload()
            {
                _pathUpdate();
            }

            std::shared_ptr<IValueSubject<bool> > DirectoryModel::observeHasUp() const
            {
                return _p->hasUp;
            }

            void DirectoryModel::cdUp()
            {
                DJV_PRIVATE_PTR();
                auto path = p.path->get();
                if (path.cdUp())
                {
                    setPath(path);
                }
            }

            std::shared_ptr<IListSubject<Path> > DirectoryModel::observeHistory() const
            {
                return _p->history;
            }

            std::shared_ptr<IValueSubject<size_t> > DirectoryModel::observeHistoryIndex() const
            {
                return _p->historyIndex;
            }

            std::shared_ptr<IValueSubject<bool> > DirectoryModel::observeHasBack() const
            {
                return _p->hasBack;
            }

            std::shared_ptr<IValueSubject<bool> > DirectoryModel::observeHasForward() const
            {
                return _p->hasForward;
            }

            void DirectoryModel::setHistoryMax(size_t value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.historyMax)
                    return;
                p.historyMax = value;
                std::vector<Path> history = p.history->get();
                while (history.size() > p.historyMax)
                {
                    history.pop_back();
                }
                p.history->setIfChanged(history);
            }

            void DirectoryModel::setHistoryIndex(size_t index)
            {
                DJV_PRIVATE_PTR();
                if (index < p.history->getSize())
                {
                    p.historyIndex->setIfChanged(index);
                    const auto& history = p.history->get();
                    const auto& path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _pathUpdate();
                }
            }

            void DirectoryModel::goBack()
            {
                DJV_PRIVATE_PTR();
                if (p.historyIndex->get() > 0)
                {
                    p.historyIndex->setIfChanged(p.historyIndex->get() - 1);
                    const auto& history = p.history->get();
                    const auto& path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _pathUpdate();
                }
            }

            void DirectoryModel::goForward()
            {
                DJV_PRIVATE_PTR();
                const auto& history = p.history->get();
                if (history.size() ? (p.historyIndex->get() < history.size() - 1) : false)
                {
                    p.historyIndex->setIfChanged(p.historyIndex->get() + 1);
                    const auto& path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _pathUpdate();
                }
            }

            std::shared_ptr<IValueSubject<DirectoryListOptions> > DirectoryModel::observeOptions() const
            {
                return _p->options;
            }

            void DirectoryModel::setOptions(const DirectoryListOptions& value)
            {
                DJV_PRIVATE_PTR();
                if (_p->options->setIfChanged(value))
                {
                    _pathUpdate();
                }
            }

            void DirectoryModel::_pathUpdate()
            {
                DJV_PRIVATE_PTR();
                const Path path = p.path->get();
                const auto options = p.options->get();
                p.future = std::async(
                    std::launch::async,
                    [path, options]
                {
                    std::pair<std::vector<Info>, std::vector<std::string> > out;
                    out.first = directoryList(path, options);
                    for (const auto& info : out.first)
                    {
                        out.second.push_back(info.getFileName(-1, false));
                    }
                    return out;
                });

                p.futureTimer->start(
                    getTimerDuration(TimerValue::Medium),
                    [this](const std::chrono::steady_clock::time_point&, const Time::Duration&)
                {
                    DJV_PRIVATE_PTR();
                    if (p.future.valid() &&
                        p.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        p.futureTimer->stop();

                        const auto& out = p.future.get();
                        p.info->setIfChanged(out.first);
                        p.fileNames->setIfChanged(out.second);
                    }
                });

                p.directoryWatcher->setPath(p.path->get());
            }

        } // namespace File
    } // namespace System
} // namespace djv

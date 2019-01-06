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

#include <djvCore/DirectoryModel.h>

#include <djvCore/DirectoryWatcher.h>
#include <djvCore/FileInfo.h>
#include <djvCore/Timer.h>
#include <djvCore/OS.h>

#include <future>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            struct DirectoryModel::Private
            {
                std::shared_ptr<ValueSubject<Path> > path;
                std::shared_ptr<ValueSubject<bool> > fileSequencesEnabled;
                std::shared_ptr<ListSubject<FileInfo> > fileInfos;
                std::shared_ptr<ListSubject<std::string> > fileNames;
                std::shared_ptr<ListSubject<Path> > history;
                std::shared_ptr<ValueSubject<size_t> > historyIndex;
                size_t historyMax = 10;
                std::shared_ptr<ValueSubject<bool> > hasUp;
                std::shared_ptr<ValueSubject<bool> > hasBack;
                std::shared_ptr<ValueSubject<bool> > hasForward;
                std::future<std::pair<std::vector<FileInfo>, std::vector<std::string> > > future;
                std::shared_ptr<Time::Timer> futureTimer;
                std::shared_ptr<DirectoryWatcher> directoryWatcher;
            };

            void DirectoryModel::_init(Context * context)
            {
                DJV_PRIVATE_PTR();
                p.path = ValueSubject<Path>::create();
                p.fileSequencesEnabled = ValueSubject<bool>::create(false);
                p.fileInfos = ListSubject<FileInfo>::create();
                p.fileNames = ListSubject<std::string>::create();
                p.history = ListSubject<Path>::create();
                p.historyIndex = ValueSubject<size_t>::create(0);

                p.hasUp = ValueSubject<bool>::create(false);
                p.hasBack = ValueSubject<bool>::create(false);
                p.hasForward = ValueSubject<bool>::create(false);

                p.futureTimer = Time::Timer::create(context);
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

            std::shared_ptr<DirectoryModel> DirectoryModel::create(Context * context)
            {
                auto out = std::shared_ptr<DirectoryModel>(new DirectoryModel);
                out->_init(context);
                return out;
            }

            std::shared_ptr<DirectoryModel> DirectoryModel::create(const Path& path, Context * context)
            {
                auto out = std::shared_ptr<DirectoryModel>(new DirectoryModel);
                out->_init(context);
                out->setPath(path);
                return out;
            }

            std::shared_ptr<IValueSubject<Path> > DirectoryModel::observePath() const
            {
                return _p->path;
            }

            void DirectoryModel::setPath(const Path& value)
            {
                DJV_PRIVATE_PTR();
                const auto absolute = FileSystem::Path::getAbsolute(value);
                if (p.path->setIfChanged(absolute))
                {
                    _p->hasUp->setIfChanged(!absolute.isRoot());
                    auto history = _p->history->get();
                    while (history.size() ? (_p->historyIndex->get() < history.size() - 1) : false)
                    {
                        history.pop_back();
                    }
                    const size_t historyIndex = history.size();
                    history.push_back(absolute);
                    _p->history->setIfChanged(history);
                    _p->historyIndex->setIfChanged(historyIndex);
                    _p->hasBack->setIfChanged(_p->historyIndex->get() > 0);
                    _p->hasForward->setIfChanged(history.size() ? (_p->historyIndex->get() < history.size() - 1) : false);
                    _updatePath();
                }
            }

            void DirectoryModel::reload()
            {
                _updatePath();
            }

            std::shared_ptr<IListSubject<FileInfo> > DirectoryModel::observeFileInfoList() const
            {
                return _p->fileInfos;
            }

            std::shared_ptr<IListSubject<std::string> > DirectoryModel::observeFileNames() const
            {
                return _p->fileNames;
            }

            std::shared_ptr<IValueSubject<bool> > DirectoryModel::observeFileSequencesEnabled() const
            {
                return _p->fileSequencesEnabled;
            }

            void DirectoryModel::setFileSequencesEnabled(bool value)
            {
                if (_p->fileSequencesEnabled->setIfChanged(value))
                {
                    _updatePath();
                }
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

            std::shared_ptr<IValueSubject<bool> > DirectoryModel::observeHasUp() const
            {
                return _p->hasUp;
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
                    const auto & history = p.history->get();
                    const auto & path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _updatePath();
                }
            }

            void DirectoryModel::goBack()
            {
                DJV_PRIVATE_PTR();
                if (p.historyIndex > 0)
                {
                    p.historyIndex->setIfChanged(p.historyIndex->get() - 1);
                    const auto & history = p.history->get();
                    const auto & path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _updatePath();
                }
            }

            void DirectoryModel::goForward()
            {
                DJV_PRIVATE_PTR();
                const auto & history = p.history->get();
                if (history.size() ? (p.historyIndex->get() < history.size() - 1) : false)
                {
                    p.historyIndex->setIfChanged(p.historyIndex->get() + 1);
                    const auto & path = history[p.historyIndex->get()];
                    p.path->setIfChanged(path);
                    p.hasUp->setIfChanged(!path.isRoot());
                    p.hasBack->setIfChanged(p.historyIndex->get() > 0);
                    p.hasForward->setIfChanged(history.size() ? (p.historyIndex->get() < history.size() - 1) : false);
                    _updatePath();
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

            void DirectoryModel::_updatePath()
            {
                DJV_PRIVATE_PTR();
                const Path path = p.path->get();
                const bool fileSequencesEnabled = p.fileSequencesEnabled->get();
                p.future = std::async(
                    std::launch::async,
                    [path, fileSequencesEnabled]
                {
                    std::pair<std::vector<FileInfo>, std::vector<std::string> > out;
                    DirListOptions options;
                    options.fileSequencesEnabled = fileSequencesEnabled;
                    out.first = FileInfo::dirList(path, options);
                    for (const auto& fileInfo : out.first)
                    {
                        out.second.push_back(fileInfo.getFileName(-1, false));
                    }
                    return out;
                });

                p.futureTimer->start(
                    Time::Timer::getMilliseconds(Time::Timer::Value::Medium),
                    [this](float)
                {
                    DJV_PRIVATE_PTR();
                    if (p.future.valid() &&
                        p.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        p.futureTimer->stop();

                        const auto& out = p.future.get();
                        p.fileInfos->setIfChanged(out.first);
                        p.fileNames->setIfChanged(out.second);
                    }
                });

                p.directoryWatcher->setPath(p.path->get());
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

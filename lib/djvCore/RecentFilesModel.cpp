// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/RecentFilesModel.h>

#include <djvCore/FileInfo.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            namespace
            {
                const size_t filesMaxDefault = 100;

            } // namespace

            struct RecentFilesModel::Private
            {
                std::shared_ptr<ListSubject<FileInfo> > files;
                std::shared_ptr<ValueSubject<size_t> > filesMax;
            };

            void RecentFilesModel::_init()
            {
                DJV_PRIVATE_PTR();
                p.files = ListSubject<FileInfo>::create();
                p.filesMax = ValueSubject<size_t>::create(filesMaxDefault);
            }

            RecentFilesModel::RecentFilesModel() :
                _p(new Private)
            {}

            std::shared_ptr<RecentFilesModel> RecentFilesModel::create()
            {
                auto out = std::shared_ptr<RecentFilesModel>(new RecentFilesModel);
                out->_init();
                return out;
            }

            std::shared_ptr<IListSubject<FileInfo> > RecentFilesModel::observeFiles() const
            {
                return _p->files;
            }

            void RecentFilesModel::setFiles(std::vector<FileInfo> value)
            {
                DJV_PRIVATE_PTR();
                while (value.size() > p.filesMax->get())
                {
                    value.pop_back();
                }
                p.files->setIfChanged(value);
            }

            void RecentFilesModel::addFile(const FileInfo& value)
            {
                DJV_PRIVATE_PTR();
                std::vector<FileInfo> list = p.files->get();
                auto i = std::find(list.begin(), list.end(), value);
                if (i != list.end())
                {
                    list.erase(i);
                }
                list.insert(list.begin(), value);
                while (list.size() > p.filesMax->get())
                {
                    list.pop_back();
                }
                p.files->setIfChanged(list);
            }

            std::shared_ptr<IValueSubject<size_t> > RecentFilesModel::observeFilesMax() const
            {
                return _p->filesMax;
            }

            void RecentFilesModel::setFilesMax(size_t value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.filesMax->get())
                    return;
                p.filesMax->setIfChanged(value);
                if (p.files->getSize() > value)
                {
                    std::vector<FileInfo> list = p.files->get();
                    while (list.size() > value)
                    {
                        list.pop_back();
                    }
                    p.files->setIfChanged(list);
                }
            }

        } // namespace FileSystem
    } // namespace Core
} // namespace djv

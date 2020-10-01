// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvSystem/RecentFilesModel.h>

#include <djvSystem/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace System
    {
        namespace File
        {
            namespace
            {
                const size_t filesMaxDefault = 100;

            } // namespace

            struct RecentFilesModel::Private
            {
                std::shared_ptr<Observer::ListSubject<Info> > files;
                std::shared_ptr<Observer::ValueSubject<size_t> > filesMax;
            };

            void RecentFilesModel::_init()
            {
                DJV_PRIVATE_PTR();
                p.files = Observer::ListSubject<Info>::create();
                p.filesMax = Observer::ValueSubject<size_t>::create(filesMaxDefault);
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

            std::shared_ptr<Observer::IValueSubject<size_t> > RecentFilesModel::observeMax() const
            {
                return _p->filesMax;
            }

            void RecentFilesModel::setMax(size_t value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.filesMax->get())
                    return;
                p.filesMax->setIfChanged(value);
                if (p.files->getSize() > value)
                {
                    std::vector<Info> list = p.files->get();
                    while (list.size() > value)
                    {
                        list.pop_back();
                    }
                    p.files->setIfChanged(list);
                }
            }

            std::shared_ptr<Observer::IListSubject<Info> > RecentFilesModel::observeFiles() const
            {
                return _p->files;
            }

            void RecentFilesModel::setFiles(std::vector<Info> value)
            {
                DJV_PRIVATE_PTR();
                while (value.size() > p.filesMax->get())
                {
                    value.pop_back();
                }
                p.files->setIfChanged(value);
            }

            void RecentFilesModel::addFile(const Info& value)
            {
                DJV_PRIVATE_PTR();
                std::vector<Info> list = p.files->get();
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

        } // namespace File
    } // namespace System
} // namespace djv

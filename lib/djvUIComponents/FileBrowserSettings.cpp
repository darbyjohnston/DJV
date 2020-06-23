// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvAV/ImageData.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/TextSystem.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Settings
        {
            struct FileBrowser::Private
            {
                std::shared_ptr<ListSubject<FileSystem::Path> > shortcuts;
                std::shared_ptr<ListSubject<FileSystem::Path> > recentPaths;
                std::shared_ptr<ValueSubject<ViewType> > viewType;
                std::shared_ptr<ValueSubject<AV::Image::Size> > thumbnailSize;
                std::shared_ptr<ListSubject<float> > listViewHeaderSplit;
                std::shared_ptr<ValueSubject<bool> > fileSequences;
                std::shared_ptr<ValueSubject<bool> > showHidden;
                std::shared_ptr<ValueSubject<FileSystem::DirectoryListSort> > sort;
                std::shared_ptr<ValueSubject<bool> > reverseSort;
                std::shared_ptr<ValueSubject<bool> > sortDirectoriesFirst;
            };

            void FileBrowser::_init(const std::shared_ptr<Context>& context)
            {
                ISettings::_init("djv::UI::Settings::FileBrowser", context);
                
                DJV_PRIVATE_PTR();
                p.shortcuts = ListSubject<FileSystem::Path>::create();
                for (size_t i = 0; i < static_cast<size_t>(OS::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = OS::getPath(static_cast<OS::DirectoryShortcut>(i));
                    p.shortcuts->pushBack(shortcut);
                }
                p.recentPaths = ListSubject<FileSystem::Path>::create();
                p.viewType = ValueSubject<ViewType>::create(ViewType::Tiles);
                p.thumbnailSize = ValueSubject<AV::Image::Size>::create(AV::Image::Size(200, 100));
                p.listViewHeaderSplit = ListSubject<float>::create({ .7F, .8F, 1.F });
                p.fileSequences = ValueSubject<bool>::create(true);
                p.showHidden = ValueSubject<bool>::create(false);
                p.sort = ValueSubject<FileSystem::DirectoryListSort>::create(FileSystem::DirectoryListSort::Name);
                p.reverseSort = ValueSubject<bool>::create(false);
                p.sortDirectoriesFirst = ValueSubject<bool>::create(true);

                _load();
            }

            FileBrowser::FileBrowser() :
                _p(new Private)
            {}

            FileBrowser::~FileBrowser()
            {}

            std::shared_ptr<FileBrowser> FileBrowser::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<FileBrowser>(new FileBrowser);
                out->_init(context);
                return out;
            }

            std::shared_ptr<IListSubject<FileSystem::Path> > FileBrowser::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void FileBrowser::setShortcuts(const std::vector<FileSystem::Path>& value)
            {
                DJV_PRIVATE_PTR();
                p.shortcuts->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<FileSystem::Path> > FileBrowser::observeRecentPaths() const
            {
                return _p->recentPaths;
            }

            void FileBrowser::setRecentPaths(const std::vector<FileSystem::Path>& value)
            {
                DJV_PRIVATE_PTR();
                p.recentPaths->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<ViewType> > FileBrowser::observeViewType() const
            {
                return _p->viewType;
            }

            void FileBrowser::setViewType(ViewType value)
            {
                DJV_PRIVATE_PTR();
                p.viewType->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<AV::Image::Size> > FileBrowser::observeThumbnailSize() const
            {
                return _p->thumbnailSize;
            }

            void FileBrowser::setThumbnailSize(const AV::Image::Size& value)
            {
                DJV_PRIVATE_PTR();
                p.thumbnailSize->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<float> > FileBrowser::observeListViewHeaderSplit() const
            {
                return _p->listViewHeaderSplit;
            }

            void FileBrowser::setListViewHeaderSplit(const std::vector<float> & value)
            {
                DJV_PRIVATE_PTR();
                p.listViewHeaderSplit->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeFileSequences() const
            {
                return _p->fileSequences;
            }

            void FileBrowser::setFileSequences(bool value)
            {
                DJV_PRIVATE_PTR();
                p.fileSequences->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeShowHidden() const
            {
                return _p->showHidden;
            }

            void FileBrowser::setShowHidden(bool value)
            {
                DJV_PRIVATE_PTR();
                p.showHidden->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<FileSystem::DirectoryListSort> > FileBrowser::observeSort() const
            {
                return _p->sort;
            }

            void FileBrowser::setSort(FileSystem::DirectoryListSort value)
            {
                DJV_PRIVATE_PTR();
                p.sort->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeReverseSort() const
            {
                return _p->reverseSort;
            }

            void FileBrowser::setReverseSort(bool value)
            {
                DJV_PRIVATE_PTR();
                p.reverseSort->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeSortDirectoriesFirst() const
            {
                return _p->sortDirectoriesFirst;
            }

            void FileBrowser::setSortDirectoriesFirst(bool value)
            {
                DJV_PRIVATE_PTR();
                p.sortDirectoriesFirst->setIfChanged(value);
            }

            void FileBrowser::load(const rapidjson::Value & value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    read("Shortcuts", value, p.shortcuts);
                    read("RecentPaths", value, p.recentPaths);
                    read("ViewType", value, p.viewType);
                    read("ThumbnailSize", value, p.thumbnailSize);
                    read("ListViewHeaderSplit", value, p.listViewHeaderSplit);
                    read("FileSequences", value, p.fileSequences);
                    read("ShowHidden", value, p.showHidden);
                    read("Sort", value, p.sort);
                    read("ReverseSort", value, p.reverseSort);
                    read("SortDirectoriesFirst", value, p.sortDirectoriesFirst);
                }
            }

            rapidjson::Value FileBrowser::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("Shortcuts", p.shortcuts->get(), out, allocator);
                write("RecentPaths", p.recentPaths->get(), out, allocator);
                write("ViewType", p.viewType->get(), out, allocator);
                write("ThumbnailSize", p.thumbnailSize->get(), out, allocator);
                write("ListViewHeaderSplit", p.listViewHeaderSplit->get(), out, allocator);
                write("FileSequences", p.fileSequences->get(), out, allocator);
                write("ShowHidden", p.showHidden->get(), out, allocator);
                write("Sort", p.sort->get(), out, allocator);
                write("ReverseSort", p.reverseSort->get(), out, allocator);
                write("SortDirectoriesFirst", p.sortDirectoriesFirst->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


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

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvAV/ImageData.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/TextSystem.h>

// These need to be included last on OSX.
#include <djvCore/PicoJSONTemplates.h>
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

            void FileBrowser::_init(Context * context)
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
                p.listViewHeaderSplit = ListSubject<float>::create({ .7f, .8f, 1.f });
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

            std::shared_ptr<FileBrowser> FileBrowser::create(Context * context)
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

            void FileBrowser::load(const picojson::value & value)
            {
                if (value.is<picojson::object>())
                {
                    DJV_PRIVATE_PTR();
                    const auto & object = value.get<picojson::object>();
                    read("Shortcuts", object, p.shortcuts);
                    read("RecentPaths", object, p.recentPaths);
                    read("ViewType", object, p.viewType);
                    read("ThumbnailSize", object, p.thumbnailSize);
                    read("ListViewHeaderSplit", object, p.listViewHeaderSplit);
                    read("FileSequences", object, p.fileSequences);
                    read("ShowHidden", object, p.showHidden);
                    read("Sort", object, p.sort);
                    read("ReverseSort", object, p.reverseSort);
                    read("SortDirectoriesFirst", object, p.sortDirectoriesFirst);
                }
            }

            picojson::value FileBrowser::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto & object = out.get<picojson::object>();
                write("Shortcuts", p.shortcuts->get(), object);
                write("RecentPaths", p.recentPaths->get(), object);
                write("ViewType", p.viewType->get(), object);
                write("ThumbnailSize", p.thumbnailSize->get(), object);
                write("ListViewHeaderSplit", p.listViewHeaderSplit->get(), object);
                write("FileSequences", p.fileSequences->get(), object);
                write("ShowHidden", p.showHidden->get(), object);
                write("Sort", p.sort->get(), object);
                write("ReverseSort", p.reverseSort->get(), object);
                write("SortDirectoriesFirst", p.sortDirectoriesFirst->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


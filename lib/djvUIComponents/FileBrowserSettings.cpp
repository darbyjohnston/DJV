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

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
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
                std::shared_ptr<MapSubject<std::string, bool> > shortcutsBellows;
                std::shared_ptr<ListSubject<float> > shortcutsSplit;
                std::shared_ptr<ValueSubject<ViewType> > viewType;
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
                p.shortcutsBellows = MapSubject<std::string, bool>::create(
                    {
                        { "Shortcuts", true },
                        { "Drives",    true },
                        { "Favorites", true },
                        { "Recent",    true }
                    }
                );
                p.shortcutsSplit = ListSubject<float>::create({ .1f, 1.f });
                p.viewType = ValueSubject<ViewType>::create(ViewType::ThumbnailsSmall);
                p.listViewHeaderSplit = ListSubject<float>::create({ .7f, .8f, 1.f });
                p.fileSequences = ValueSubject<bool>::create(false);
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

            std::shared_ptr<IMapSubject<std::string, bool> > FileBrowser::observeShortcutsBellows() const
            {
                return _p->shortcutsBellows;
            }

            void FileBrowser::setShortcutsBellows(const std::map<std::string, bool> & value)
            {
                DJV_PRIVATE_PTR();
                p.shortcutsBellows->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<float> > FileBrowser::observeShortcutsSplit() const
            {
                return _p->shortcutsSplit;
            }

            void FileBrowser::setShortcutsSplit(const std::vector<float> & value)
            {
                DJV_PRIVATE_PTR();
                p.shortcutsSplit->setIfChanged(value);
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

            void FileBrowser::load(const picojson::value& value)
            {
                if (value.is<picojson::object>())
                {
                    DJV_PRIVATE_PTR();
                    const auto& object = value.get<picojson::object>();
                    read("ShortcutsBellows", object, p.shortcutsBellows);
                    read("ShortcutsSplit", object, p.shortcutsSplit);
                    read("ViewType", object, p.viewType);
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
                auto& object = out.get<picojson::object>();
                write("ShortcutsBellows", p.shortcutsBellows->get(), object);
                write("ShortcutsSplit", p.shortcutsSplit->get(), object);
                write("ViewType", p.viewType->get(), object);
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


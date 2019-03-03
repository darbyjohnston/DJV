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

#include <djvUI/EnumJSON.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/FileInfoJSON.h>
#include <djvCore/TextSystem.h>

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
                std::shared_ptr<ValueSubject<bool> > showShortcuts;
                std::shared_ptr<MapSubject<std::string, bool> > shortcutsBellows;
                std::shared_ptr<ValueSubject<float> > shortcutsSplitter;
                std::shared_ptr<ValueSubject<ViewType> > viewType;
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
                p.showShortcuts = ValueSubject<bool>::create(true);
                p.shortcutsBellows = MapSubject<std::string, bool>::create(
                    {
                        { "Shortcuts", true },
                        { "Drives",    true },
                        { "Favorites", true },
                        { "Recent",    true }
                    }
                );
                p.shortcutsSplitter = ValueSubject<float>::create(.1f);
                p.viewType = ValueSubject<ViewType>::create(ViewType::ThumbnailsSmall);
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

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeShowShortcuts() const
            {
                return _p->showShortcuts;
            }

            void FileBrowser::setShowShortcuts(bool value)
            {
                DJV_PRIVATE_PTR();
                p.showShortcuts->setIfChanged(value);
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

            std::shared_ptr<IValueSubject<float> > FileBrowser::observeShortcutsSplitter() const
            {
                return _p->shortcutsSplitter;
            }

            void FileBrowser::setShortcutsSplitter(float value)
            {
                DJV_PRIVATE_PTR();
                p.shortcutsSplitter->setIfChanged(value);
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
                    _read("ShowShortcuts", object, p.showShortcuts);
                    _read("ShortcutsBellows", object, p.shortcutsBellows);
                    _read("ShortcutsSplitter", object, p.shortcutsSplitter);
                    _read("ViewType", object, p.viewType);
                    _read("FileSequences", object, p.fileSequences);
                    _read("ShowHidden", object, p.showHidden);
                    _read("Sort", object, p.sort);
                    _read("ReverseSort", object, p.reverseSort);
                    _read("SortDirectoriesFirst", object, p.sortDirectoriesFirst);
                }
            }

            picojson::value FileBrowser::save()
            {
                DJV_PRIVATE_PTR();
                picojson::value out(picojson::object_type, true);
                auto& object = out.get<picojson::object>();
                _write("ShowShortcuts", p.showShortcuts->get(), object);
                _write("ShortcutsBellows", p.shortcutsBellows->get(), object);
                _write("ShortcutsSplitter", p.shortcutsSplitter->get(), object);
                _write("ViewType", p.viewType->get(), object);
                _write("FileSequences", p.fileSequences->get(), object);
                _write("ShowHidden", p.showHidden->get(), object);
                _write("Sort", p.sort->get(), object);
                _write("ReverseSort", p.reverseSort->get(), object);
                _write("SortDirectoriesFirst", p.sortDirectoriesFirst->get(), object);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


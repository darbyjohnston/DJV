// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvAV/ImageData.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>
#include <djvCore/OS.h>
#include <djvCore/TextSystem.h>

#if defined(GetObject)
#undef GetObject
#endif // GetObject

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
                std::shared_ptr<ValueSubject<bool> > pathsOpen;
                std::shared_ptr<MapSubject<std::string, bool> > pathsBellowsState;
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
                std::shared_ptr<MapSubject<std::string, ShortcutDataPair> > keyShortcuts;
            };

            void FileBrowser::_init(const std::shared_ptr<Context>& context)
            {
                ISettings::_init("djv::UI::Settings::FileBrowser", context);
                
                DJV_PRIVATE_PTR();
                p.pathsOpen = ValueSubject<bool>::create();
                p.pathsBellowsState = MapSubject<std::string, bool>::create();
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
                p.keyShortcuts = MapSubject<std::string, ShortcutDataPair>::create({
                    { "file_browser_shortcut_back", { ShortcutData(GLFW_KEY_LEFT, ShortcutData::getSystemModifier()) } },
                    { "file_browser_shortcut_forward", { ShortcutData(GLFW_KEY_RIGHT, ShortcutData::getSystemModifier()) } },
                    { "file_browser_shortcut_up", { ShortcutData(GLFW_KEY_UP, ShortcutData::getSystemModifier()) } },
                    { "file_browser_shortcut_paths", { ShortcutData(GLFW_KEY_P) } },
                    { "file_browser_shortcut_tiles", { ShortcutData(GLFW_KEY_T) } },
                    { "file_browser_shortcut_list", { ShortcutData(GLFW_KEY_L) } },
                    { "file_browser_shortcut_increase_thumbnail_size", { ShortcutData(GLFW_KEY_EQUAL) } },
                    { "file_browser_shortcut_decrease_thumbnail_size", { ShortcutData(GLFW_KEY_MINUS) } },
                    { "file_browser_shortcut_file_sequences", { ShortcutData(GLFW_KEY_S) } },
                    { "file_browser_shortcut_show_hidden", { ShortcutData(GLFW_KEY_N) } },
                    { "file_browser_shortcut_sort_by_name", { ShortcutData(GLFW_KEY_3) } },
                    { "file_browser_shortcut_sort_by_size", { ShortcutData(GLFW_KEY_4) } },
                    { "file_browser_shortcut_sort_by_time", { ShortcutData(GLFW_KEY_5) } },
                    { "file_browser_shortcut_reverse_sort", { ShortcutData(GLFW_KEY_R) } },
                    { "file_browser_shortcut_sort_directories_first", { ShortcutData(GLFW_KEY_D) } } });

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

            std::shared_ptr<Core::IValueSubject<bool> > FileBrowser::observePathsOpen() const
            {
                return _p->pathsOpen;
            }

            void FileBrowser::setPathsOpen(bool value)
            {
                _p->pathsOpen->setIfChanged(value);
            }

            std::shared_ptr<Core::IMapSubject<std::string, bool> > FileBrowser::observePathsBellowsState() const
            {
                return _p->pathsBellowsState;
            }

            void FileBrowser::setPathsBellowsState(const std::map<std::string, bool>& value)
            {
                _p->pathsBellowsState->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<FileSystem::Path> > FileBrowser::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void FileBrowser::setShortcuts(const std::vector<FileSystem::Path>& value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<FileSystem::Path> > FileBrowser::observeRecentPaths() const
            {
                return _p->recentPaths;
            }

            void FileBrowser::setRecentPaths(const std::vector<FileSystem::Path>& value)
            {
                _p->recentPaths->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<ViewType> > FileBrowser::observeViewType() const
            {
                return _p->viewType;
            }

            void FileBrowser::setViewType(ViewType value)
            {
                _p->viewType->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<AV::Image::Size> > FileBrowser::observeThumbnailSize() const
            {
                return _p->thumbnailSize;
            }

            void FileBrowser::setThumbnailSize(const AV::Image::Size& value)
            {
                _p->thumbnailSize->setIfChanged(value);
            }

            std::shared_ptr<IListSubject<float> > FileBrowser::observeListViewHeaderSplit() const
            {
                return _p->listViewHeaderSplit;
            }

            void FileBrowser::setListViewHeaderSplit(const std::vector<float> & value)
            {
                _p->listViewHeaderSplit->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeFileSequences() const
            {
                return _p->fileSequences;
            }

            void FileBrowser::setFileSequences(bool value)
            {
                _p->fileSequences->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeShowHidden() const
            {
                return _p->showHidden;
            }

            void FileBrowser::setShowHidden(bool value)
            {
                _p->showHidden->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<FileSystem::DirectoryListSort> > FileBrowser::observeSort() const
            {
                return _p->sort;
            }

            void FileBrowser::setSort(FileSystem::DirectoryListSort value)
            {
                _p->sort->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeReverseSort() const
            {
                return _p->reverseSort;
            }

            void FileBrowser::setReverseSort(bool value)
            {
                _p->reverseSort->setIfChanged(value);
            }

            std::shared_ptr<IValueSubject<bool> > FileBrowser::observeSortDirectoriesFirst() const
            {
                return _p->sortDirectoriesFirst;
            }

            void FileBrowser::setSortDirectoriesFirst(bool value)
            {
                _p->sortDirectoriesFirst->setIfChanged(value);
            }

            std::shared_ptr<MapSubject<std::string, ShortcutDataPair> > FileBrowser::observeKeyShortcuts() const
            {
                return _p->keyShortcuts;
            }

            void FileBrowser::setKeyShortcuts(const ShortcutDataMap& value)
            {
                _p->keyShortcuts->setIfChanged(value);
            }

            void FileBrowser::load(const rapidjson::Value & value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    read("PathsOpen", value, p.pathsOpen);
                    read("PathsBellowsState", value, p.pathsBellowsState);
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
                    read("KeyShortcuts", value, p.keyShortcuts);
                }
            }

            rapidjson::Value FileBrowser::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                write("PathsOpen", p.pathsOpen->get(), out, allocator);
                write("PathsBellowsState", p.pathsBellowsState->get(), out, allocator);
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
                write("KeyShortcuts", p.keyShortcuts->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UI
} // namespace djv


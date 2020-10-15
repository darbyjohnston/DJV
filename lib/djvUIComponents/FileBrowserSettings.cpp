// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserSettings.h>

#include <djvUI/EnumFunc.h>
#include <djvUI/ShortcutDataFunc.h>

#include <djvImage/Info.h>
#include <djvImage/InfoFunc.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfoFunc.h>
#include <djvSystem/PathFunc.h>
#include <djvSystem/TextSystem.h>

#include <djvCore/OSFunc.h>

// These need to be included last on macOS.
#include <djvCore/RapidJSONTemplates.h>
#include <djvUI/ISettingsTemplates.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#pragma optimize("", off)

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct FileBrowser::Private
            {
                std::shared_ptr<Observer::ValueSubject<bool> > pathsOpen;
                std::shared_ptr<Observer::MapSubject<std::string, bool> > pathsBellowsState;
                std::shared_ptr<Observer::ListSubject<System::File::Path> > shortcuts;
                std::shared_ptr<Observer::ListSubject<System::File::Path> > recentPaths;
                std::shared_ptr<Observer::ValueSubject<UI::ViewType> > viewType;
                std::shared_ptr<Observer::ValueSubject<Image::Size> > thumbnailSize;
                std::shared_ptr<Observer::ListSubject<float> > listViewHeaderSplit;
                std::shared_ptr<Observer::ValueSubject<bool> > fileSequences;
                std::shared_ptr<Observer::ValueSubject<bool> > showHidden;
                std::shared_ptr<Observer::ValueSubject<System::File::DirectoryListSort> > sort;
                std::shared_ptr<Observer::ValueSubject<bool> > reverseSort;
                std::shared_ptr<Observer::ValueSubject<bool> > sortDirectoriesFirst;
                std::shared_ptr<Observer::MapSubject<std::string, UI::ShortcutDataPair> > keyShortcuts;
            };

            void FileBrowser::_init(const std::shared_ptr<System::Context>& context)
            {
                ISettings::_init("djv::UI::Settings::FileBrowser", context);
                
                DJV_PRIVATE_PTR();
                p.pathsOpen = Observer::ValueSubject<bool>::create();
                p.pathsBellowsState = Observer::MapSubject<std::string, bool>::create();
                p.shortcuts = Observer::ListSubject<System::File::Path>::create();
                for (size_t i = 0; i < static_cast<size_t>(System::File::DirectoryShortcut::Count); ++i)
                {
                    const auto shortcut = System::File::getPath(static_cast<System::File::DirectoryShortcut>(i));
                    p.shortcuts->pushBack(shortcut);
                }
                p.recentPaths = Observer::ListSubject<System::File::Path>::create();
                p.viewType = Observer::ValueSubject<UI::ViewType>::create(UI::ViewType::Tiles);
                p.thumbnailSize = Observer::ValueSubject<Image::Size>::create(Image::Size(200, 100));
                p.listViewHeaderSplit = Observer::ListSubject<float>::create({ .7F, .8F, 1.F });
                p.fileSequences = Observer::ValueSubject<bool>::create(true);
                p.showHidden = Observer::ValueSubject<bool>::create(false);
                p.sort = Observer::ValueSubject<System::File::DirectoryListSort>::create(System::File::DirectoryListSort::Name);
                p.reverseSort = Observer::ValueSubject<bool>::create(false);
                p.sortDirectoriesFirst = Observer::ValueSubject<bool>::create(true);
                p.keyShortcuts = Observer::MapSubject<std::string, UI::ShortcutDataPair>::create({
                    { "file_browser_shortcut_paths", { UI::ShortcutData(GLFW_KEY_P) } },
                    { "file_browser_shortcut_back", { UI::ShortcutData(GLFW_KEY_LEFT, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_forward", { UI::ShortcutData(GLFW_KEY_RIGHT, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_up", { UI::ShortcutData(GLFW_KEY_UP, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_select_all", { UI::ShortcutData(GLFW_KEY_A, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_select_none", { UI::ShortcutData(GLFW_KEY_N, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_invert_selection", { UI::ShortcutData(GLFW_KEY_I, UI::getSystemModifier()) } },
                    { "file_browser_shortcut_tiles", { UI::ShortcutData(GLFW_KEY_T) } },
                    { "file_browser_shortcut_list", { UI::ShortcutData(GLFW_KEY_L) } },
                    { "file_browser_shortcut_increase_thumbnail_size", { UI::ShortcutData(GLFW_KEY_EQUAL) } },
                    { "file_browser_shortcut_decrease_thumbnail_size", { UI::ShortcutData(GLFW_KEY_MINUS) } },
                    { "file_browser_shortcut_file_sequences", { UI::ShortcutData(GLFW_KEY_S) } },
                    { "file_browser_shortcut_show_hidden", { UI::ShortcutData(GLFW_KEY_N) } },
                    { "file_browser_shortcut_sort_by_name", { UI::ShortcutData(GLFW_KEY_3) } },
                    { "file_browser_shortcut_sort_by_size", { UI::ShortcutData(GLFW_KEY_4) } },
                    { "file_browser_shortcut_sort_by_time", { UI::ShortcutData(GLFW_KEY_5) } },
                    { "file_browser_shortcut_reverse_sort", { UI::ShortcutData(GLFW_KEY_R) } },
                    { "file_browser_shortcut_sort_directories_first", { UI::ShortcutData(GLFW_KEY_D) } } });

                _load();
            }

            FileBrowser::FileBrowser() :
                _p(new Private)
            {}

            FileBrowser::~FileBrowser()
            {}

            std::shared_ptr<FileBrowser> FileBrowser::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<FileBrowser>(new FileBrowser);
                out->_init(context);
                return out;
            }

            std::shared_ptr<Core::Observer::IValueSubject<bool> > FileBrowser::observePathsOpen() const
            {
                return _p->pathsOpen;
            }

            std::shared_ptr<Core::Observer::IMapSubject<std::string, bool> > FileBrowser::observePathsBellowsState() const
            {
                return _p->pathsBellowsState;
            }

            void FileBrowser::setPathsOpen(bool value)
            {
                _p->pathsOpen->setIfChanged(value);
            }

            void FileBrowser::setPathsBellowsState(const std::map<std::string, bool>& value)
            {
                _p->pathsBellowsState->setIfChanged(value);
            }

            std::shared_ptr<Observer::IListSubject<System::File::Path> > FileBrowser::observeShortcuts() const
            {
                return _p->shortcuts;
            }

            void FileBrowser::setShortcuts(const std::vector<System::File::Path>& value)
            {
                _p->shortcuts->setIfChanged(value);
            }

            std::shared_ptr<Observer::IListSubject<System::File::Path> > FileBrowser::observeRecentPaths() const
            {
                return _p->recentPaths;
            }

            void FileBrowser::setRecentPaths(const std::vector<System::File::Path>& value)
            {
                _p->recentPaths->setIfChanged(value);
            }

            std::shared_ptr<Observer::IValueSubject<UI::ViewType> > FileBrowser::observeViewType() const
            {
                return _p->viewType;
            }

            std::shared_ptr<Observer::IValueSubject<Image::Size> > FileBrowser::observeThumbnailSize() const
            {
                return _p->thumbnailSize;
            }

            std::shared_ptr<Observer::IListSubject<float> > FileBrowser::observeListViewHeaderSplit() const
            {
                return _p->listViewHeaderSplit;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > FileBrowser::observeFileSequences() const
            {
                return _p->fileSequences;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > FileBrowser::observeShowHidden() const
            {
                return _p->showHidden;
            }

            std::shared_ptr<Observer::IValueSubject<System::File::DirectoryListSort> > FileBrowser::observeSort() const
            {
                return _p->sort;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > FileBrowser::observeReverseSort() const
            {
                return _p->reverseSort;
            }

            std::shared_ptr<Observer::IValueSubject<bool> > FileBrowser::observeSortDirectoriesFirst() const
            {
                return _p->sortDirectoriesFirst;
            }

            void FileBrowser::setViewType(UI::ViewType value)
            {
                _p->viewType->setIfChanged(value);
            }

            void FileBrowser::setThumbnailSize(const Image::Size& value)
            {
                _p->thumbnailSize->setIfChanged(value);
            }

            void FileBrowser::setListViewHeaderSplit(const std::vector<float>& value)
            {
                _p->listViewHeaderSplit->setIfChanged(value);
            }

            void FileBrowser::setFileSequences(bool value)
            {
                _p->fileSequences->setIfChanged(value);
            }

            void FileBrowser::setShowHidden(bool value)
            {
                _p->showHidden->setIfChanged(value);
            }

            void FileBrowser::setSort(System::File::DirectoryListSort value)
            {
                _p->sort->setIfChanged(value);
            }

            void FileBrowser::setReverseSort(bool value)
            {
                _p->reverseSort->setIfChanged(value);
            }

            void FileBrowser::setSortDirectoriesFirst(bool value)
            {
                _p->sortDirectoriesFirst->setIfChanged(value);
            }

            std::shared_ptr<Observer::MapSubject<std::string, UI::ShortcutDataPair> > FileBrowser::observeKeyShortcuts() const
            {
                return _p->keyShortcuts;
            }

            void FileBrowser::setKeyShortcuts(const UI::ShortcutDataMap& value)
            {
                _p->keyShortcuts->setIfChanged(value);
            }

            void FileBrowser::load(const rapidjson::Value & value)
            {
                if (value.IsObject())
                {
                    DJV_PRIVATE_PTR();
                    UI::Settings::read("PathsOpen", value, p.pathsOpen);
                    UI::Settings::read("PathsBellowsState", value, p.pathsBellowsState);
                    UI::Settings::read("Shortcuts", value, p.shortcuts);
                    UI::Settings::read("RecentPaths", value, p.recentPaths);
                    UI::Settings::read("ViewType", value, p.viewType);
                    UI::Settings::read("ThumbnailSize", value, p.thumbnailSize);
                    UI::Settings::read("ListViewHeaderSplit", value, p.listViewHeaderSplit);
                    UI::Settings::read("FileSequences", value, p.fileSequences);
                    UI::Settings::read("ShowHidden", value, p.showHidden);
                    UI::Settings::read("Sort", value, p.sort);
                    UI::Settings::read("ReverseSort", value, p.reverseSort);
                    UI::Settings::read("SortDirectoriesFirst", value, p.sortDirectoriesFirst);
                    UI::Settings::read("KeyShortcuts", value, p.keyShortcuts);
                }
            }

            rapidjson::Value FileBrowser::save(rapidjson::Document::AllocatorType& allocator)
            {
                DJV_PRIVATE_PTR();
                rapidjson::Value out(rapidjson::kObjectType);
                UI::Settings::write("PathsOpen", p.pathsOpen->get(), out, allocator);
                UI::Settings::write("PathsBellowsState", p.pathsBellowsState->get(), out, allocator);
                UI::Settings::write("Shortcuts", p.shortcuts->get(), out, allocator);
                UI::Settings::write("RecentPaths", p.recentPaths->get(), out, allocator);
                UI::Settings::write("ViewType", p.viewType->get(), out, allocator);
                UI::Settings::write("ThumbnailSize", p.thumbnailSize->get(), out, allocator);
                UI::Settings::write("ListViewHeaderSplit", p.listViewHeaderSplit->get(), out, allocator);
                UI::Settings::write("FileSequences", p.fileSequences->get(), out, allocator);
                UI::Settings::write("ShowHidden", p.showHidden->get(), out, allocator);
                UI::Settings::write("Sort", p.sort->get(), out, allocator);
                UI::Settings::write("ReverseSort", p.reverseSort->get(), out, allocator);
                UI::Settings::write("SortDirectoriesFirst", p.sortDirectoriesFirst->get(), out, allocator);
                UI::Settings::write("KeyShortcuts", p.keyShortcuts->get(), out, allocator);
                return out;
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv


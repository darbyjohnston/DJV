// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/ISettings.h>
#include <djvUI/ShortcutData.h>

#include <djvSystem/FileInfo.h>

#include <djvCore/ListObserver.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Image
    {
        class Size;

    } // namespace Image

    namespace UIComponents
    {
        namespace Settings
        {
            //! This class provides the file browser settings.
            class FileBrowser : public UI::Settings::ISettings
            {
                DJV_NON_COPYABLE(FileBrowser);

            protected:
                void _init(const std::shared_ptr<System::Context>& context);

                FileBrowser();

            public:
                ~FileBrowser() override;

                static std::shared_ptr<FileBrowser> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observePathsOpen() const;

                void setPathsOpen(bool);

                std::shared_ptr<Core::Observer::IMapSubject<std::string, bool> > observePathsBellowsState() const;
                
                void setPathsBellowsState(const std::map<std::string, bool>&);

                std::shared_ptr<Core::Observer::IListSubject<System::File::Path> > observeShortcuts() const;
                
                void setShortcuts(const std::vector<System::File::Path>&);

                std::shared_ptr<Core::Observer::IListSubject<System::File::Path> > observeRecentPaths() const;
                
                void setRecentPaths(const std::vector<System::File::Path>&);

                std::shared_ptr<Core::Observer::IValueSubject<UI::ViewType> > observeViewType() const;
                
                void setViewType(UI::ViewType);

                std::shared_ptr<Core::Observer::IValueSubject<Image::Size> > observeThumbnailSize() const;
                
                void setThumbnailSize(const Image::Size&);

                std::shared_ptr<Core::Observer::IListSubject<float> > observeListViewHeaderSplit() const;
                
                void setListViewHeaderSplit(const std::vector<float> &);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeFileSequences() const;
                
                void setFileSequences(bool);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeShowHidden() const;
                
                void setShowHidden(bool);

                std::shared_ptr<Core::Observer::IValueSubject<System::File::DirectoryListSort> > observeSort() const;
                
                void setSort(System::File::DirectoryListSort);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeReverseSort() const;
                
                void setReverseSort(bool);

                std::shared_ptr<Core::Observer::IValueSubject<bool> > observeSortDirectoriesFirst() const;
                
                void setSortDirectoriesFirst(bool);

                std::shared_ptr<Core::Observer::MapSubject<std::string, UI::ShortcutDataPair> > observeKeyShortcuts() const;
                
                void setKeyShortcuts(const UI::ShortcutDataMap&);

                void load(const rapidjson::Value &) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv

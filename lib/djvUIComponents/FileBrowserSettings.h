// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Enum.h>
#include <djvUI/ISettings.h>
#include <djvUI/ShortcutData.h>

#include <djvCore/FileInfo.h>
#include <djvCore/MapObserver.h>
#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>
#include <djvCore/Vector.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Size;
    
        } // namespace Image
    } // namespace AV

    namespace UI
    {
        namespace Settings
        {
            //! This class provides the file browser settings.
            class FileBrowser : public ISettings
            {
                DJV_NON_COPYABLE(FileBrowser);

            protected:
                void _init(const std::shared_ptr<Core::Context>& context);

                FileBrowser();

            public:
                virtual ~FileBrowser();

                static std::shared_ptr<FileBrowser> create(const std::shared_ptr<Core::Context>&);

                std::shared_ptr<Core::IListSubject<Core::FileSystem::Path> > observeShortcuts() const;
                void setShortcuts(const std::vector<Core::FileSystem::Path>&);

                std::shared_ptr<Core::IListSubject<Core::FileSystem::Path> > observeRecentPaths() const;
                void setRecentPaths(const std::vector<Core::FileSystem::Path>&);

                std::shared_ptr<Core::IValueSubject<ViewType> > observeViewType() const;
                void setViewType(ViewType);

                std::shared_ptr<Core::IValueSubject<AV::Image::Size> > observeThumbnailSize() const;
                void setThumbnailSize(const AV::Image::Size&);

                std::shared_ptr<Core::IListSubject<float> > observeListViewHeaderSplit() const;
                void setListViewHeaderSplit(const std::vector<float> &);

                std::shared_ptr<Core::IValueSubject<bool> > observeFileSequences() const;
                void setFileSequences(bool);

                std::shared_ptr<Core::IValueSubject<bool> > observeShowHidden() const;
                void setShowHidden(bool);

                std::shared_ptr<Core::IValueSubject<Core::FileSystem::DirectoryListSort> > observeSort() const;
                void setSort(Core::FileSystem::DirectoryListSort);

                std::shared_ptr<Core::IValueSubject<bool> > observeReverseSort() const;
                void setReverseSort(bool);

                std::shared_ptr<Core::IValueSubject<bool> > observeSortDirectoriesFirst() const;
                void setSortDirectoriesFirst(bool);

                std::shared_ptr<Core::MapSubject<std::string, std::vector<UI::ShortcutData> > > observeKeyShortcuts() const;
                void setKeyShortcuts(const UI::ShortcutDataMap&);

                void load(const rapidjson::Value &) override;
                rapidjson::Value save(rapidjson::Document::AllocatorType&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Settings
    } // namespace UI
} // namespace djv

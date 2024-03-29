// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/FileBrowser.h>

#include <djvSystem/Path.h>

#include <djvMath/Range.h>

#include <djvCore/ListObserver.h>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class DirectoryModel;
            class DrivesModel;
            class Path;
            class RecentFilesModel;

        } // namespace File
    } // namespace System

    namespace UIComponents
    {
        namespace FileBrowser
        {
            std::string getPathLabel(const System::File::Path&);

            const Math::IntRange thumbnailSizeRange(50, 800);

            //! File browser path widget.
            class PathWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PathWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                PathWidget();

            public:
                ~PathWidget() override;

                static std::shared_ptr<PathWidget> create(const std::shared_ptr<System::Context>&);

                void setPath(const System::File::Path&);
                void setPathCallback(const std::function<void(const System::File::Path&)>&);

                void setHistory(const std::vector<System::File::Path>&);
                void setHistoryIndex(size_t);
                void setHistoryIndexCallback(const std::function<void(size_t)>&);

                void setEdit(bool);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;
                bool _eventFilter(const std::shared_ptr<System::IObject>&, System::Event::Event&) override;

            private:
                static std::string _getLabel(const System::File::Path&);

                DJV_PRIVATE();
            };

            //! File browser shortcuts model.
            class ShortcutsModel : public std::enable_shared_from_this<ShortcutsModel>
            {
                DJV_NON_COPYABLE(ShortcutsModel);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                ShortcutsModel();

            public:
                virtual ~ShortcutsModel();

                static std::shared_ptr<ShortcutsModel> create(const std::shared_ptr<System::Context>&);

                std::shared_ptr<Core::Observer::IListSubject<System::File::Path> > observeShortcuts() const;

                void setShortcuts(const std::vector<System::File::Path>&);
                void addShortcut(const System::File::Path&);
                void removeShortcut(size_t index);

            private:
                DJV_PRIVATE();
            };

            //! File browser shortcuts widget.
            class ShortcutsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ShortcutsWidget);

            protected:
                void _init(
                    const std::shared_ptr<ShortcutsModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);
                ShortcutsWidget();

            public:
                ~ShortcutsWidget() override;

                static std::shared_ptr<ShortcutsWidget> create(
                    const std::shared_ptr<ShortcutsModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);

                void setPath(const System::File::Path&);
                void setCallback(const std::function<void(const System::File::Path&)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

            //! File browser recent paths widget.
            class RecentPathsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(RecentPathsWidget);

            protected:
                void _init(
                    const std::shared_ptr<System::File::RecentFilesModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);
                RecentPathsWidget();

            public:
                ~RecentPathsWidget() override;

                static std::shared_ptr<RecentPathsWidget> create(
                    const std::shared_ptr<System::File::RecentFilesModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);

                void setCallback(const std::function<void(const System::File::Path&)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

            //! File browser drives widget.
            class DrivesWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(DrivesWidget);

            protected:
                void _init(
                    const std::shared_ptr<System::File::DrivesModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);
                DrivesWidget();

            public:
                ~DrivesWidget() override;

                static std::shared_ptr<DrivesWidget> create(
                    const std::shared_ptr<System::File::DrivesModel>&,
                    size_t elide,
                    const std::shared_ptr<System::Context>&);

                void setCallback(const std::function<void(const System::File::Path&)>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

            //! File browser paths widget.
            class PathsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PathsWidget);

            protected:
                void _init(
                    const std::shared_ptr<System::File::DirectoryModel>&,
                    const std::shared_ptr<ShortcutsModel>&,
                    const std::shared_ptr<System::File::RecentFilesModel>&,
                    const std::shared_ptr<System::File::DrivesModel>&,
                    const std::shared_ptr<System::Context>&);
                PathsWidget();

            public:
                ~PathsWidget() override;

                static std::shared_ptr<PathsWidget> create(
                    const std::shared_ptr<System::File::DirectoryModel>&,
                    const std::shared_ptr<ShortcutsModel>&,
                    const std::shared_ptr<System::File::RecentFilesModel>&,
                    const std::shared_ptr<System::File::DrivesModel>&,
                    const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

                void _initEvent(System::Event::Init&) override;

            private:
                void _saveBellowsState();

                DJV_PRIVATE();
            };

            //! File browser thumbnail widget.
            class ThumbnailWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ThumbnailWidget);

            protected:
                void _init(const std::shared_ptr<System::Context>&);
                ThumbnailWidget();

            public:
                ~ThumbnailWidget() override;

                static std::shared_ptr<ThumbnailWidget> create(const std::shared_ptr<System::Context>&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _layoutEvent(System::Event::Layout&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace FileBrowser
    } // namespace UIComponents
} // namespace djv


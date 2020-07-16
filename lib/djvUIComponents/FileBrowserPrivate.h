// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUIComponents/FileBrowser.h>

#include <djvUI/IButton.h>

#include <djvAV/Image.h>

#include <djvCore/ListObserver.h>
#include <djvCore/Path.h>
#include <djvCore/Range.h>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class DrivesModel;
            class Path;
            class RecentFilesModel;

        } // namespace FileSystem
    } // namespace Core

    namespace UI
    {
        namespace FileBrowser
        {
            const Core::IntRange thumbnailSizeRange(50, 800);

            //! This class provides a file browser path widget.
            class PathWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(PathWidget);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                PathWidget();

            public:
                ~PathWidget() override;

                static std::shared_ptr<PathWidget> create(const std::shared_ptr<Core::Context>&);

                void setPath(const Core::FileSystem::Path &);
                void setPathCallback(const std::function<void(const Core::FileSystem::Path &)> &);

                void setHistory(const std::vector<Core::FileSystem::Path> &);
                void setHistoryIndex(size_t);
                void setHistoryIndexCallback(const std::function<void(size_t)> &);

                void setEdit(bool);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init &) override;
                bool _eventFilter(const std::shared_ptr<Core::IObject> &, Core::Event::Event &) override;

            private:
                static std::string _getLabel(const Core::FileSystem::Path&);

                DJV_PRIVATE();
            };

            //! This class provides a file browser shortcuts model.
            class ShortcutsModel : public std::enable_shared_from_this<ShortcutsModel>
            {
                DJV_NON_COPYABLE(ShortcutsModel);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);
                ShortcutsModel();

            public:
                virtual ~ShortcutsModel();

                static std::shared_ptr<ShortcutsModel> create(const std::shared_ptr<Core::Context>&);

                std::shared_ptr<Core::IListSubject<Core::FileSystem::Path> > observeShortcuts() const;
                void setShortcuts(const std::vector<Core::FileSystem::Path> &);
                void addShortcut(const Core::FileSystem::Path &);
                void removeShortcut(size_t index);

            private:
                DJV_PRIVATE();
            };

            //! This class provides a file browser shortcuts widget.
            class ShortcutsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ShortcutsWidget);

            protected:
                void _init(const std::shared_ptr<ShortcutsModel>&, const std::shared_ptr<Core::Context>&);
                ShortcutsWidget();

            public:
                ~ShortcutsWidget() override;

                static std::shared_ptr<ShortcutsWidget> create(const std::shared_ptr<ShortcutsModel>&, const std::shared_ptr<Core::Context>&);

                void setPath(const Core::FileSystem::Path&);
                void setCallback(const std::function<void(const Core::FileSystem::Path&)>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides a file browser recent paths widget.
            class RecentPathsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(RecentPathsWidget);

            protected:
                void _init(const std::shared_ptr<Core::FileSystem::RecentFilesModel>&, const std::shared_ptr<Core::Context>&);
                RecentPathsWidget();

            public:
                ~RecentPathsWidget() override;

                static std::shared_ptr<RecentPathsWidget> create(const std::shared_ptr<Core::FileSystem::RecentFilesModel>&, const std::shared_ptr<Core::Context>&);

                void setCallback(const std::function<void(const Core::FileSystem::Path&)>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides a file browser drives widget.
            class DrivesWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(DrivesWidget);

            protected:
                void _init(const std::shared_ptr<Core::FileSystem::DrivesModel>&, const std::shared_ptr<Core::Context>&);
                DrivesWidget();

            public:
                ~DrivesWidget() override;

                static std::shared_ptr<DrivesWidget> create(const std::shared_ptr<Core::FileSystem::DrivesModel>&, const std::shared_ptr<Core::Context>&);

                void setCallback(const std::function<void(const Core::FileSystem::Path &)> &);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides a file browser sort widget.
            class SortWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(SortWidget);

            protected:
                void _init(
                    const std::map<std::string, std::shared_ptr<Action> >&,
                    const std::shared_ptr<Core::Context>&);
                SortWidget();

            public:
                ~SortWidget() override;

                static std::shared_ptr<SortWidget> create(
                    const std::map<std::string, std::shared_ptr<Action> >&,
                    const std::shared_ptr<Core::Context>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

            //! This class provides a file browser settings widget.
            class SettingsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(
                    const std::map<std::string, std::shared_ptr<Action> >&,
                    const std::shared_ptr<Core::Context>&);
                SettingsWidget();

            public:
                ~SettingsWidget() override;

                static std::shared_ptr<SettingsWidget> create(
                    const std::map<std::string, std::shared_ptr<Action> >&,
                    const std::shared_ptr<Core::Context>&);

                std::map<std::string, bool> getBellowsState() const;
                void setBellowsState(const std::map<std::string, bool>&);

            protected:
                void _preLayoutEvent(Core::Event::PreLayout&) override;
                void _layoutEvent(Core::Event::Layout&) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv


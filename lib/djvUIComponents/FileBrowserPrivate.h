//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
            const Core::IntRange thumbnailSizeRange(50, 400);

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
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;

                void _initEvent(Core::Event::Init &) override;
                bool _eventFilter(const std::shared_ptr<Core::IObject> &, Core::Event::Event &) override;

            private:
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
                void _preLayoutEvent(Core::Event::PreLayout &) override;
                void _layoutEvent(Core::Event::Layout &) override;

                void _initEvent(Core::Event::Init&) override;

            private:
                DJV_PRIVATE();
            };

        } // namespace Layout
    } // namespace UI
} // namespace djv


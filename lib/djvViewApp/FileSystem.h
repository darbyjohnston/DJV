// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewSystem.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace Core
    {
        namespace FileSystem
        {
            class FileInfo;

        } // namespace FileSystem
    } // namespace Core

    namespace ViewApp
    {
        class Media;

        //! This struct provides options for opening files.
        struct OpenOptions
        {
            std::shared_ptr<glm::vec2>            pos;
            std::shared_ptr<float>                spacing;
            std::shared_ptr<Core::Math::Rational> speed;
            std::shared_ptr<Core::Frame::Range>   startEnd;
            std::shared_ptr<std::string>          inPoint;
            std::shared_ptr<std::string>          outPoint;
            std::shared_ptr<std::string>          frame;
        };

        //! This class provides the file system.
        class FileSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(FileSystem);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            FileSystem();

        public:
            ~FileSystem() override;

            static std::shared_ptr<FileSystem> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > observeOpened() const;
            std::shared_ptr<Core::IValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > observeOpened2() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > observeClosed() const;
            std::shared_ptr<Core::IListSubject<std::shared_ptr<Media> > > observeMedia() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > observeCurrentMedia() const;
            std::shared_ptr<Core::IValueSubject<float> > observeCachePercentage() const;

            void open();
            void open(const Core::FileSystem::FileInfo&, const OpenOptions& = OpenOptions());
            void open(const std::vector<std::string>&, OpenOptions = OpenOptions());
            void close(const std::shared_ptr<Media> &);
            void closeAll();
            void setCurrentMedia(const std::shared_ptr<Media> &);

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            MenuData getMenu() const override;

        protected:
            void _actionsUpdate();
            void _cacheUpdate();
            void _showFileBrowserDialog();
            void _showRecentFilesDialog();

            void _closeWidget(const std::string&) override;
            void _textUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


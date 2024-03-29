// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/IViewAppSystem.h>

#include <djvMath/FrameNumber.h>
#include <djvMath/Rational.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

#include <glm/vec2.hpp>

namespace djv
{
    namespace System
    {
        namespace File
        {
            class Info;

        } // namespace File
    } // namespace System

    namespace ViewApp
    {
        class Media;

        //! Options for opening files.
        struct OpenOptions
        {
            std::shared_ptr<Math::IntRational>  speed;
            std::shared_ptr<Math::Frame::Range> startEnd;
            std::shared_ptr<std::string>        inPoint;
            std::shared_ptr<std::string>        outPoint;
            std::shared_ptr<std::string>        frame;
        };

        //! File system.
        class FileSystem : public IViewAppSystem
        {
            DJV_NON_COPYABLE(FileSystem);

        protected:
            void _init(const std::shared_ptr<System::Context>&);
            FileSystem();

        public:
            ~FileSystem() override;

            static std::shared_ptr<FileSystem> create(const std::shared_ptr<System::Context>&);

            //! \name Media
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Media> > > observeOpened() const;
            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Media> > > observeClosed() const;
            std::shared_ptr<Core::Observer::IListSubject<std::shared_ptr<Media> > > observeMedia() const;
            std::shared_ptr<Core::Observer::IValueSubject<std::shared_ptr<Media> > > observeCurrentMedia() const;

            void open();
            void open(const System::File::Info&, const OpenOptions& = OpenOptions());
            void open(const std::vector<std::string>&, OpenOptions = OpenOptions());
            void close(std::shared_ptr<Media>);
            void closeAll();
            void setCurrentMedia(const std::shared_ptr<Media>&);

            ///@}

            //! \name Cache
            ///@{

            std::shared_ptr<Core::Observer::IValueSubject<float> > observeCachePercentage() const;

            ///@}

            int getSortKey() const override;
            std::map<std::string, std::shared_ptr<UI::Action> > getActions() const override;
            std::vector<std::shared_ptr<UI::Menu> > getMenus() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetActions() const override;
            std::vector<std::shared_ptr<UI::Action> > getToolWidgetToolBarActions() const override;
            ToolWidgetData createToolWidget(const std::shared_ptr<UI::Action>&) override;

        protected:
            void _actionsUpdate();
            void _cacheUpdate();
            void _showFileBrowserDialog();
            void _showRecentFilesDialog();

            void _textUpdate() override;
            void _shortcutsUpdate() override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


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
            std::shared_ptr<glm::vec2>           pos;
            std::shared_ptr<float>               spacing;
            std::shared_ptr<Core::Time::Speed>   speed;
            std::shared_ptr<std::string>         inPoint;
            std::shared_ptr<std::string>         outPoint;
            std::shared_ptr<std::string>         frame;
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
            std::vector<Core::FileSystem::FileInfo> _processFileNames(const std::vector<std::string>&);
            
            DJV_PRIVATE();
        };

    } // namespace ViewApp
} // namespace djv


//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/IViewSystem.h>

#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewLib
    {
        class FileSystemSettings;

        class FileSystem : public IViewSystem
        {
            DJV_NON_COPYABLE(FileSystem);

        protected:
            void _init(Core::Context *);
            FileSystem();

        public:
            ~FileSystem() override;

            static std::shared_ptr<FileSystem> create(Core::Context *);

            std::shared_ptr<Core::IValueSubject<std::pair<std::shared_ptr<Media>, glm::vec2> > > observeOpened() const;
            std::shared_ptr<Core::IValueSubject<bool> > observeClose() const;
            void open(const std::string &, const glm::vec2 & = glm::vec2(-1.f, -1.f));
            void showRecentFiles();

            const std::shared_ptr<FileSystemSettings> & getSettings() const;

            std::map<std::string, std::shared_ptr<UI::Action> > getActions() override;
            NewMenu createMenu() override;
            void setCurrentMedia(const std::shared_ptr<Media> &) override;

        protected:
            void _localeEvent(Core::Event::Locale &) override;

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv


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

#include <djvViewLib/Enum.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace ViewLib
    {
        class Media;

        class Workspace : public std::enable_shared_from_this<Workspace>
        {
            DJV_NON_COPYABLE(Workspace);

        protected:
            void _init(Core::Context *);
            Workspace();

        public:
            ~Workspace();

            static std::shared_ptr<Workspace> create(Core::Context *);

            std::shared_ptr<Core::IValueSubject<std::string> > observeWorkspaceName() const;
            std::shared_ptr<Core::IListSubject<std::shared_ptr<Media> > > observeMedia() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > observeMediaOpened() const;
            std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > observeMediaClosed() const;

            void setWorkspaceName(const std::string &);
            void openMedia(const std::string &);
            void closeMedia(const std::shared_ptr<Media> &);

        private:
            DJV_PRIVATE();
        };

    } // namespace ViewLib
} // namespace djv


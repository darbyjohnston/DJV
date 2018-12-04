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

#include <djvAV/Time.h>

#include <djvCore/ListObserver.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace ViewLib
    {
        class Context;

        class Track : public std::enable_shared_from_this<Track>
        {
            DJV_NON_COPYABLE(Track);

        protected:
            void _init(const std::shared_ptr<Context> &);
            Track();

        public:
            ~Track();

            static std::shared_ptr<Track> create(const std::shared_ptr<Context> &);

            std::shared_ptr<Core::IValueSubject<std::string> > getFileName() const;
            std::shared_ptr<Core::IValueSubject<AV::Timestamp> > getOffset() const;
            std::shared_ptr<Core::IValueSubject<AV::Duration> > getDuration() const;

            void setFileName(const std::string &);
            void setOFfset(AV::Timestamp);
            void setDuration(AV::Duration);

            bool operator == (const Track &) const;
            bool operator != (const Track &) const;

        private:
            DJV_PRIVATE();
        };

        class Project : public std::enable_shared_from_this<Project>
        {
            DJV_NON_COPYABLE(Project);

        protected:
            void _init(const std::shared_ptr<Context> &);
            Project();

        public:
            ~Project();

            static std::shared_ptr<Project> create(const std::shared_ptr<Context> &);

            std::shared_ptr<Core::IValueSubject<std::string> > getFileName() const;
            std::shared_ptr<Core::IValueSubject<bool> > getHasChanges() const;
            std::shared_ptr<Core::IListSubject<std::shared_ptr<Track> > > getTracks() const;
            std::shared_ptr<Core::IValueSubject<AV::Duration> > getDuration() const;
            
            void open(const std::string &);
            void close();
            void save();
            void saveAs(const std::string &);

            void addTrack(const std::shared_ptr<Track> &);
            void removeTrack(const std::shared_ptr<Track> &);
            
        private:
            DJV_PRIVATE();

            void _updateDuration();
        };

    } // namespace ViewLib
} // namespace djv


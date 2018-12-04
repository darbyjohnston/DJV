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

#include <djvViewLib/Project.h>

#include <djvViewLib/Context.h>

#include <sstream>

namespace djv
{
    namespace ViewLib
    {
        struct Track::Private
        {
            std::shared_ptr<Core::ValueSubject<std::string> > fileName;
            std::shared_ptr<Core::ValueSubject<AV::Timestamp> > offset;
            std::shared_ptr<Core::ValueSubject<AV::Duration> > duration;
        };

        void Track::_init(const std::shared_ptr<Context> &)
        {
            _p->fileName = Core::ValueSubject<std::string>::create();
            _p->offset = Core::ValueSubject<AV::Timestamp>::create();
            _p->duration = Core::ValueSubject<AV::Duration>::create();
        }

        Track::Track() :
            _p(new Private)
        {}

        Track::~Track()
        {}

        std::shared_ptr<Track> Track::create(const std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<Track>(new Track);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Track::getFileName() const
        {
            return _p->fileName;
        }

        std::shared_ptr<Core::IValueSubject<AV::Timestamp> > Track::getOffset() const
        {
            return _p->offset;
        }

        std::shared_ptr<Core::IValueSubject<AV::Duration> > Track::getDuration() const
        {
            return _p->duration;
        }

        void Track::setFileName(const std::string & value)
        {
            _p->fileName->setIfChanged(value);
        }

        void Track::setOFfset(AV::Timestamp value)
        {
            _p->offset->setIfChanged(value);
        }

        void Track::setDuration(AV::Duration value)
        {
            _p->duration->setIfChanged(value);
        }

        bool Track::operator == (const Track & other) const
        {
            return
                _p->fileName == other._p->fileName &&
                _p->offset == other._p->offset &&
                _p->duration == other._p->duration;
        }

        bool Track::operator != (const Track & other) const
        {
            return !(*this == other);
        }

        namespace
        {
            size_t projectCount = 0;

        } // namespace

        struct Project::Private
        {
            std::shared_ptr<Core::ValueSubject<std::string> > fileName;
            std::shared_ptr<Core::ValueSubject<bool> > changes;
            std::shared_ptr<Core::ListSubject<std::shared_ptr<Track> > > tracks;
            std::shared_ptr<Core::ValueSubject<AV::Duration> > duration;
            std::map<std::shared_ptr<Track>, std::shared_ptr<Core::ValueObserver<AV::Timestamp> > > trackOffsetObserver;
            std::map<std::shared_ptr<Track>, std::shared_ptr<Core::ValueObserver<AV::Duration> > > trackDurationObserver;
        };

        void Project::_init(const std::shared_ptr<Context> &)
        {
            _p->fileName = Core::ValueSubject<std::string>::create();
            _p->changes = Core::ValueSubject<bool>::create();
            _p->tracks = Core::ListSubject<std::shared_ptr<Track> >::create();
            _p->duration = Core::ValueSubject<AV::Duration>::create();

            ++projectCount;
            std::stringstream s;
            s << "Untitled " << projectCount;
            _p->fileName->setIfChanged(s.str());

            _updateDuration();
        }

        Project::Project() :
            _p(new Private)
        {}

        Project::~Project()
        {}

        std::shared_ptr<Project> Project::create(const std::shared_ptr<Context> & context)
        {
            auto out = std::shared_ptr<Project>(new Project);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Project::getFileName() const
        {
            return _p->fileName;
        }

        std::shared_ptr<Core::IValueSubject<bool> >Project::getHasChanges() const
        {
            return _p->changes;
        }

        std::shared_ptr<Core::IListSubject<std::shared_ptr<Track> > > Project::getTracks() const
        {
            return _p->tracks;
        }

        std::shared_ptr<Core::IValueSubject<AV::Duration> >Project::getDuration() const
        {
            return _p->duration;
        }

        void Project::open(const std::string & fileName)
        {
            _p->fileName->setIfChanged(fileName);
        }

        void Project::close()
        {
            _p->fileName->setIfChanged(std::string());
        }

        void Project::save()
        {}

        void Project::saveAs(const std::string & fileName)
        {
            _p->fileName->setIfChanged(fileName);
        }

        void Project::addTrack(const std::shared_ptr<Track> & track)
        {
            _p->tracks->pushBack(track);

            auto weak = std::weak_ptr<Project>(std::dynamic_pointer_cast<Project>(shared_from_this()));
            _p->trackOffsetObserver[track] = Core::ValueObserver<AV::Timestamp>::create(
                track->getOffset(),
                [weak](AV::Timestamp value)
            {
                if (auto project = weak.lock())
                {
                    project->_updateDuration();
                }
            });
            _p->trackDurationObserver[track] = Core::ValueObserver<AV::Timestamp>::create(
                track->getOffset(),
                [weak](AV::Timestamp value)
            {
                if (auto project = weak.lock())
                {
                    project->_updateDuration();
                }
            });
        }

        void Project::removeTrack(const std::shared_ptr<Track> & track)
        {
            const auto i = _p->tracks->indexOf(track);
            if (i != _p->tracks->invalidIndex)
            {
                _p->tracks->removeItem(i);
                const auto i = _p->trackOffsetObserver.find(track);
                if (i != _p->trackOffsetObserver.end())
                {
                    _p->trackOffsetObserver.erase(i);
                }
                const auto j = _p->trackDurationObserver.find(track);
                if (j != _p->trackDurationObserver.end())
                {
                    _p->trackDurationObserver.erase(j);
                }
            }
        }

        void Project::_updateDuration()
        {
            AV::Duration duration = 0;
            for (const auto & i : _p->tracks->get())
            {
                duration = std::max(duration, i->getOffset()->get() + i->getDuration()->get());
            }
            _p->duration->setIfChanged(duration);
        }

    } // namespace ViewLib
} // namespace djv


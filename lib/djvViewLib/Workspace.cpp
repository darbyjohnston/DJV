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

#include <djvViewLib/Workspace.h>

#include <djvViewLib/Media.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct Workspace::Private
        {
            Context * context = nullptr;
            std::shared_ptr<ValueSubject<std::string> > workspaceName;
            std::shared_ptr<ListSubject<std::shared_ptr<Media> > > media;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > mediaOpened;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > mediaClosed;
        };

        void Workspace::_init(Context * context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
            p.workspaceName = ValueSubject<std::string>::create();
            p.media = ListSubject<std::shared_ptr<Media> >::create();
        }

        Workspace::Workspace() :
            _p(new Private)
        {}

        Workspace::~Workspace()
        {}

        std::shared_ptr<Workspace> Workspace::create(Context * context)
        {
            auto out = std::shared_ptr<Workspace>(new Workspace);
            out->_init(context);
            return out;
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Workspace::observeWorkspaceName() const
        {
            return _p->workspaceName;
        }

        std::shared_ptr<Core::IListSubject<std::shared_ptr<Media> > > Workspace::observeMedia() const
        {
            return _p->media;
        }

        void Workspace::setWorkspaceName(const std::string & value)
        {
            _p->workspaceName->setIfChanged(value);
        }

        void Workspace::openMedia(const std::string & fileName)
        {
            auto media = Media::create(fileName, _p->context);
            _p->media->pushBack(media);
            _p->mediaOpened->setAlways(media);
        }

        void Workspace::closeMedia(const std::shared_ptr<Media> & media)
        {
            const size_t index = _p->media->indexOf(media);
            if (index != invalidListIndex)
            {
                _p->media->removeItem(index);
                _p->mediaClosed->setAlways(media);
            }
        }

    } // namespace ViewLib
} // namespace djv


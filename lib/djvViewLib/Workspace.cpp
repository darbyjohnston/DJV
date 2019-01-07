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
        namespace
        {
            size_t workspaceCount = 0;
        
        } // namespace

        struct Workspace::Private
        {
            Context * context = nullptr;
            std::shared_ptr<ValueSubject<std::string> > name;
            std::shared_ptr<ListSubject<std::shared_ptr<Media> > > media;
            std::shared_ptr<ValueSubject<std::shared_ptr<Media> > > currentMedia;
        };

        void Workspace::_init(Context * context)
        {
            DJV_PRIVATE_PTR();
            p.context = context;
            p.name = ValueSubject<std::string>::create();
            p.media = ValueSubject<std::shared_ptr<Media> >::create();
            p.currentMedia = ListSubject<std::shared_ptr<Media> >::create();
        }

        std::shared_ptr<Core::IValueSubject<std::string> > Workspace::getName() const
        {
            return _p->name;
        }

        std::shared_ptr<Core::IListSubject<std::shared_ptr<Media> > > Workspace::getMedia() const
        {
            return _p->media;
        }

        std::shared_ptr<Core::IValueSubject<std::shared_ptr<Media> > > Workspace::getCurrentMedia() const
        {
            return _p->currentMedia;
        }

        void Workspace::setName(const std::string & value)
        {
            _p->name->setIfChanged(value);
        }

        void Workspace::openMedia(const std::string & fileName)
        {
            auto media = Media::create(fileName, _p->context);
            _p->media->pushBack(media);
            _p->currentMedia->setIfChanged(media);
        }

        void Workspace::closeMedia(const std::shared_ptr<Media> & media)
        {
            const size_t index = _p->media->indexOf(media);
            if (index != invalidListIndex)
            {
                _p->media->removeItem(index);
            }
            if (media == _p->currentMedia)
            {
                const size_t size = _p->media->getSize();
                _p->currentMedia->setIfChanged(size > 0 ? _p->media->getItem(size - 1) : nullptr);
            }
        }

        void Workspace::setCurrentMedia(const std::shared_ptr<Media> & value)
        {
            _p->currentMedia->setIfChanged(value);
        }

        void Workspace::nextMedia()
        {
            const size_t size = _p->media->getSize();
            if (size > 1)
            {
                size_t index = _p->media->indexOf(_p->currentMedia->get());
                if (index != invalidListIndex)
                {
                    ++index;
                    if (index >= size)
                    {
                        index = 0;
                    }
                    _p->currentMedia->setIfChanged(_p->media->getItem(index));
                }
            }
        }

        void Workspace::prevMedia()
        {
            const size_t size = _p->media->getSize();
            if (size > 1)
            {
                size_t index = _p->media->indexOf(_p->currentMedia->get());
                if (index != invalidListIndex)
                {
                    if (index > 0)
                    {
                        --index;
                    }
                    else
                    {
                        index = size - 1;
                    }
                    _p->currentMedia->setIfChanged(_p->media->getItem(index));
                }
            }
        }


    } // namespace ViewLib
} // namespace djv


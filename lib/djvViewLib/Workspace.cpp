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

#include <djvViewLib/Context.h>
#include <djvViewLib/Media.h>

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
            std::weak_ptr<Context> context;
            std::string name;
            std::vector<std::shared_ptr<Media> > media;
            std::shared_ptr<Media> currentMedia;
            QMdiArea::ViewMode viewMode = QMdiArea::TabbedView;
            std::map<std::shared_ptr<Media>, Enum::WindowState> windowState;
        };

        Workspace::Workspace(const std::shared_ptr<Context> & context, QObject * parent) :
            QObject(parent),
            _p(new Private)
        {
            _p->context = context;
            ++workspaceCount;
            std::stringstream s;
            s << "Workspace " << workspaceCount;
            _p->name = s.str();
        }

        Workspace::~Workspace()
        {
            for (auto i : _p->media)
            {
                //! \todo Save
            }
        }

        const std::string & Workspace::getName() const
        {
            return _p->name;
        }

        const std::vector<std::shared_ptr<Media> > & Workspace::getMedia() const
        {
            return _p->media;
        }

        const std::shared_ptr<Media> & Workspace::getCurrentMedia() const
        {
            return _p->currentMedia;
        }

        QMdiArea::ViewMode Workspace::getViewMode() const
        {
            return _p->viewMode;
        }

        Enum::WindowState Workspace::getWindowState() const
        {
            const auto i = _p->windowState.find(_p->currentMedia);
            return i != _p->windowState.end() ? i->second : Enum::WindowState::Normal;
        }

        void Workspace::setName(const std::string & value)
        {
            if (value == _p->name)
                return;
            _p->name = value;
            Q_EMIT nameChanged(_p->name);
        }

        void Workspace::openMedia(const std::string & fileName)
        {
            if (auto context = _p->context.lock())
            {
                auto media = Media::create(fileName, context);
                _p->media.push_back(media);
                _p->currentMedia = media;
                Q_EMIT mediaAdded(media);
                Q_EMIT currentMediaChanged(_p->currentMedia);
            }
        }

        void Workspace::closeMedia(const std::shared_ptr<Media> & media)
        {
            const auto i = std::find(_p->media.begin(), _p->media.end(), media);
            if (i != _p->media.end())
            {
                //! \todo Save
                int index = static_cast<int>(i - _p->media.begin());
                auto media = *i;
                _p->media.erase(i);
                const auto k = _p->windowState.find(media);
                if (k != _p->windowState.end())
                {
                    _p->windowState.erase(k);
                }
                Q_EMIT mediaRemoved(media);
                if (media == _p->currentMedia)
                {
                    index = std::min(index, static_cast<int>(_p->media.size()) - 1);
                    setCurrentMedia(index != -1 ? _p->media[index] : nullptr);
                }
            }
        }

        void Workspace::setCurrentMedia(const std::shared_ptr<Media> & media)
        {
            if (media == _p->currentMedia)
                return;
            _p->currentMedia = media;
            Q_EMIT currentMediaChanged(_p->currentMedia);
            auto windowState = Enum::WindowState::Normal;
            const auto i = _p->windowState.find(media);
            if (i != _p->windowState.end())
            {
                windowState = i->second;
            }
            Q_EMIT windowStateChanged(windowState);
        }

        void Workspace::nextMedia()
        {
            auto i = std::find(_p->media.begin(), _p->media.end(), _p->currentMedia);
            if (i != _p->media.end())
            {
                ++i;
                if (i == _p->media.end())
                {
                    i = _p->media.begin();
                }
                setCurrentMedia(*i);
            }
        }

        void Workspace::prevMedia()
        {
            auto i = std::find(_p->media.begin(), _p->media.end(), _p->currentMedia);
            if (i != _p->media.end())
            {
                if (i == _p->media.begin())
                {
                    i = _p->media.end();
                }
                --i;
                setCurrentMedia(*i);
            }
        }

        void Workspace::setViewMode(QMdiArea::ViewMode value)
        {
            if (value == _p->viewMode)
                return;
            _p->viewMode = value;
            Q_EMIT viewModeChanged(_p->viewMode);
        }

        void Workspace::setWindowState(Enum::WindowState value)
        {
            setWindowState(_p->currentMedia, value);
        }

        void Workspace::setWindowState(const std::shared_ptr<Media> & media, Enum::WindowState value)
        {
            const auto i = _p->windowState.find(media);
            if (i == _p->windowState.end())
            {
                _p->windowState[media] = Enum::WindowState::Normal;
            }
            if (value == _p->windowState[media])
                return;
            _p->windowState[media] = value;
            Q_EMIT windowStateChanged(value);
        }

    } // namespace ViewLib
} // namespace djv


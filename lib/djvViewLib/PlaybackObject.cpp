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

#include <djvViewLib/PlaybackObject.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/TimelineWidget.h>

#include <QAction>
#include <QDockWidget>
#include <QMenu>

namespace djv
{
    namespace ViewLib
    {
        struct PlaybackObject::Private
        {
            std::map<QString, QPointer<QAction> > actions;
            std::map<QString, QPointer<QActionGroup> > actionGroups;
            std::shared_ptr<Core::ValueObserver<AV::Duration> > durationObserver;
            std::shared_ptr<Core::ValueObserver<AV::Timestamp> > currentTimeObserver;
            std::shared_ptr<Core::ValueObserver<Enum::Playback> > playbackObserver;
            std::shared_ptr<Media> currentMedia;
            std::vector<QPointer<TimelineWidget> > timelineWidgets;
        };
        
        PlaybackObject::PlaybackObject(const std::shared_ptr<Context> & context, QObject * parent) :
            IViewObject("PlaybackObject", context, parent),
            _p(new Private)
        {
            _p->actions["Stop"] = new QAction("Stop", this);
            _p->actions["Stop"]->setCheckable(true);
            _p->actions["Stop"]->setShortcut(QKeySequence("K"));
            _p->actions["Forward"] = new QAction("Forward", this);
            _p->actions["Forward"]->setCheckable(true);
            _p->actions["Forward"]->setShortcut(QKeySequence("L"));
            _p->actions["Reverse"] = new QAction("Reverse", this);
            _p->actions["Reverse"]->setCheckable(true);
            _p->actions["Reverse"]->setShortcut(QKeySequence("J"));
            _p->actionGroups["Playback"] = new QActionGroup(this);
            _p->actionGroups["Playback"]->setExclusive(true);
            _p->actionGroups["Playback"]->addAction(_p->actions["Stop"]);
            _p->actionGroups["Playback"]->addAction(_p->actions["Forward"]);
            _p->actionGroups["Playback"]->addAction(_p->actions["Reverse"]);

            _p->actions["Timeline"] = new QAction("Timeline", this);
            _p->actions["Timeline"]->setCheckable(true);
            _p->actions["Timeline"]->setChecked(true);

            connect(
                _p->actionGroups["Playback"],
                &QActionGroup::triggered,
                [this](QAction * action)
            {
                if (_p->currentMedia)
                {
                    Enum::Playback playback = Enum::Playback::Stop;
                    if (_p->actions["Forward"] == action)
                    {
                        playback = Enum::Playback::Forward;
                    }
                    else if (_p->actions["Reverse"] == action)
                    {
                        playback = Enum::Playback::Reverse;
                    }
                    _p->currentMedia->setPlayback(playback);
                }
            });
        }
        
        PlaybackObject::~PlaybackObject()
        {}

        std::string PlaybackObject::getMenuSortKey() const
        {
            return "5";
        }
        
        QPointer<QMenu> PlaybackObject::createMenu()
        {
            auto menu = new QMenu("Playback");
            menu->addAction(_p->actions["Stop"]);
            menu->addAction(_p->actions["Forward"]);
            menu->addAction(_p->actions["Reverse"]);
            menu->addSeparator();
            menu->addAction(_p->actions["Timeline"]);
            return menu;
        }

        QPointer<QDockWidget> PlaybackObject::createDockWidget()
        {
            QDockWidget * out = nullptr;
            if (auto context = getContext().lock())
            {
                out = new QDockWidget("Timeline");
                auto timelineWidget = new TimelineWidget(context);
                timelineWidget->setEnabled(_p->currentMedia ? true : false);
                out->setWidget(timelineWidget);
                _p->timelineWidgets.push_back(timelineWidget);

                connect(
                    _p->actions["Timeline"],
                    &QAction::toggled,
                    [out](bool value)
                {
                    out->setVisible(value);
                });

                connect(
                    out,
                    &QDockWidget::visibilityChanged,
                    [this](bool value)
                {
                    _p->actions["Timeline"]->setChecked(value);
                });

                connect(
                    timelineWidget,
                    &TimelineWidget::currentTimeChanged,
                    [this](AV::Timestamp value)
                {
                    if (_p->currentMedia)
                    {
                        _p->currentMedia->setCurrentTime(value);
                    }
                });
                connect(
                    timelineWidget,
                    &TimelineWidget::playbackChanged,
                    [this](Enum::Playback value)
                {
                    if (_p->currentMedia)
                    {
                        _p->currentMedia->setPlayback(value);
                    }
                });
            }
            return out;
        }

        Qt::DockWidgetArea PlaybackObject::getDockWidgetArea() const
        {
            return Qt::DockWidgetArea::BottomDockWidgetArea;
        }

        bool PlaybackObject::isDockWidgetVisible() const
        {
            return true;
        }

        void PlaybackObject::setCurrentMedia(const std::shared_ptr<Media> & media)
        {
            _p->actionGroups["Playback"]->setEnabled(media ? true : false);
            for (auto & i : _p->timelineWidgets)
            {
                i->setEnabled(media ? true : false);
            }

            if (media)
            {
                _p->durationObserver = Core::ValueObserver<AV::Duration>::create(
                    media->getDuration(),
                    [this](AV::Duration value)
                {
                    for (auto & i : _p->timelineWidgets)
                    {
                        i->setDuration(value);
                    }
                });
                _p->currentTimeObserver = Core::ValueObserver<AV::Timestamp>::create(
                    media->getCurrentTime(),
                    [this](AV::Timestamp value)
                {
                    for (auto & i : _p->timelineWidgets)
                    {
                        i->setCurrentTime(value);
                    }
                });
                _p->playbackObserver = Core::ValueObserver<Enum::Playback>::create(
                    media->getPlayback(),
                    [this](Enum::Playback value)
                {
                    switch (value)
                    {
                    case Enum::Playback::Stop: _p->actions["Stop"]->setChecked(true); break;
                    case Enum::Playback::Forward: _p->actions["Forward"]->setChecked(true); break;
                    case Enum::Playback::Reverse: _p->actions["Reverse"]->setChecked(true); break;
                    default: break;
                    }
                    for (auto & i : _p->timelineWidgets)
                    {
                        i->setPlayback(value);
                    }
                });
            }
            else
            {
                _p->currentTimeObserver = nullptr;
                _p->playbackObserver = nullptr;
                for (auto & i : _p->timelineWidgets)
                {
                    i->setDuration(0);
                    i->setCurrentTime(0);
                    i->setPlayback(Enum::Playback::Stop);
                }
            }
            _p->currentMedia = media;
        }

    } // namespace ViewLib
} // namespace djv

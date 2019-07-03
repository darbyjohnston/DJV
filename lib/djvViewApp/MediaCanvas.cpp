//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/MediaCanvas.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/MDICanvas.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MediaCanvas::Private
        {
            std::shared_ptr<UI::MDI::Canvas> canvas;
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaWidget> > mediaWidgets;
            std::function<void(const std::shared_ptr<MediaWidget>&)> activeCallback;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened2Observer;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > closedObserver;
        };

        void MediaCanvas::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaCanvas");

            p.canvas = UI::MDI::Canvas::create(context);
            addChild(p.canvas);

            auto weak = std::weak_ptr<MediaCanvas>(std::dynamic_pointer_cast<MediaCanvas>(shared_from_this()));
            p.canvas->setActiveCallback(
                [weak, context](const std::shared_ptr<UI::MDI::IWidget> & value)
            {
                if (auto widget = weak.lock())
                {
                    auto mediaWidget = std::dynamic_pointer_cast<MediaWidget>(value);
                    if (mediaWidget)
                    {
                        auto fileSystem = context->getSystemT<FileSystem>();
                        fileSystem->setCurrentMedia(mediaWidget->getMedia());
                    }
                    if (widget->_p->activeCallback)
                    {
                        widget->_p->activeCallback(mediaWidget);
                    }
                }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.canvas->setMaximizedCallback(
                    [windowSystem](bool value)
                {
                    windowSystem->setMaximized(value);
                });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->mediaWidgets.find(value);
                        if (i != widget->_p->mediaWidgets.end())
                        {
                            i->second->moveToFront();
                        }
                    }
                });

                p.openedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeOpened(),
                    [weak, context](const std::shared_ptr<Media>& value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            auto mediaWidget = MediaWidget::create(value, context);
                            widget->_p->canvas->addChild(mediaWidget);
                            widget->_p->mediaWidgets[value] = mediaWidget;
                        }
                    }
                });

                p.opened2Observer = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                    fileSystem->observeOpened2(),
                    [weak, context](const std::pair<std::shared_ptr<Media>, glm::vec2>& value)
                {
                    if (value.first)
                    {
                        if (auto widget = weak.lock())
                        {
                            auto mediaWidget = MediaWidget::create(value.first, context);
                            widget->_p->canvas->addChild(mediaWidget);
                            widget->_p->canvas->setWidgetGeometry(mediaWidget, BBox2f(value.second, value.second));
                            widget->_p->mediaWidgets[value.first] = mediaWidget;
                        }
                    }
                });

                p.closedObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeClosed(),
                    [weak](const std::shared_ptr<Media> & value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            const auto i = widget->_p->mediaWidgets.find(value);
                            if (i != widget->_p->mediaWidgets.end())
                            {
                                widget->_p->canvas->removeChild(i->second);
                                widget->_p->mediaWidgets.erase(i);
                            }
                        }
                    }
                });
            }
        }

        MediaCanvas::MediaCanvas() :
            _p(new Private)
        {}

        MediaCanvas::~MediaCanvas()
        {}

        std::shared_ptr<MediaCanvas> MediaCanvas::create(Context* context)
        {
            auto out = std::shared_ptr<MediaCanvas>(new MediaCanvas);
            out->_init(context);
            return out;
        }

        std::shared_ptr<MediaWidget> MediaCanvas::getActiveWidget() const
        {
            return std::dynamic_pointer_cast<MediaWidget>(_p->canvas->getActiveWidget());
        }

        void MediaCanvas::setActiveCallback(const std::function<void(const std::shared_ptr<MediaWidget>&)>& value)
        {
            _p->activeCallback = value;
        }

        void MediaCanvas::setMaximized(bool value)
        {
            _p->canvas->setMaximized(value);
        }

        void MediaCanvas::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->canvas->getMinimumSize());
        }

        void MediaCanvas::_layoutEvent(Event::Layout&)
        {
            _p->canvas->setGeometry(getGeometry());
        }

        void MediaCanvas::_localeEvent(Event::Locale& event)
        {}

    } // namespace ViewApp
} // namespace djv


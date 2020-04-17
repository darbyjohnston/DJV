// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MediaCanvas.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MediaCanvas::Private
        {
            std::map<std::shared_ptr<Media>, std::shared_ptr<MediaWidget> > mediaWidgets;
            std::function<void(const std::shared_ptr<MediaWidget>&)> activeCallback;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > openedObserver;
            std::shared_ptr<ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> > > opened2Observer;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > closedObserver;
        };

        void MediaCanvas::_init(const std::shared_ptr<Context>& context)
        {
            Canvas::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MediaCanvas");

            auto weak = std::weak_ptr<MediaCanvas>(std::dynamic_pointer_cast<MediaCanvas>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            Canvas::setActiveCallback(
                [weak, contextWeak](const std::shared_ptr<UI::MDI::IWidget> & value)
            {
                    if (auto context = contextWeak.lock())
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
                    }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                setMaximizeCallback(
                    [windowSystem](bool value)
                {
                    windowSystem->setMaximize(value);
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
                    [weak, contextWeak](const std::shared_ptr<Media>& value)
                {
                    if (value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto mediaWidget = MediaWidget::create(value, context);
                                widget->addChild(mediaWidget);
                                widget->_p->mediaWidgets[value] = mediaWidget;
                            }
                        }
                    }
                });

                p.opened2Observer = ValueObserver<std::pair<std::shared_ptr<Media>, glm::vec2> >::create(
                    fileSystem->observeOpened2(),
                    [weak, contextWeak](const std::pair<std::shared_ptr<Media>, glm::vec2>& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (value.first)
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto mediaWidget = MediaWidget::create(value.first, context);
                                    widget->addChild(mediaWidget);
                                    widget->setWidgetGeometry(mediaWidget, BBox2f(value.second, value.second));
                                    widget->_p->mediaWidgets[value.first] = mediaWidget;
                                }
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
                                widget->removeChild(i->second);
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

        std::shared_ptr<MediaCanvas> MediaCanvas::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<MediaCanvas>(new MediaCanvas);
            out->_init(context);
            return out;
        }

        std::shared_ptr<MediaWidget> MediaCanvas::getActiveWidget() const
        {
            return std::dynamic_pointer_cast<MediaWidget>(Canvas::getActiveWidget());
        }

        void MediaCanvas::setActiveCallback(const std::function<void(const std::shared_ptr<MediaWidget>&)>& value)
        {
            _p->activeCallback = value;
        }

    } // namespace ViewApp
} // namespace djv


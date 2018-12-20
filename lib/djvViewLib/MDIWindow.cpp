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

#include <djvViewLib/MDIWindow.h>

#include <djvViewLib/Media.h>
#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/Button.h>
#include <djvUI/Icon.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvCore/Memory.h>
#include <djvCore/UID.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MDIWindow::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<UI::ImageWidget> imageWidget;
            std::shared_ptr<PlaybackWidget> playbackWidget;
            std::shared_ptr<TimelineSlider> timelineSlider;
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<UI::Icon> resizeHandle;
            std::shared_ptr<UI::HorizontalLayout> bottomBar;
            std::shared_ptr<UI::StackLayout> layout;
            std::function<void(void)> closedCallback;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<AV::Duration> > durationObserver;
            std::shared_ptr<ValueObserver<AV::Timestamp> > currentTimeObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver;
            std::shared_ptr<ValueObserver<Playback> > playbackObserver2;
            std::shared_ptr<ValueObserver<AV::Timestamp> > currentTimeObserver2;
        };
        
        void MDIWindow::_init(const std::shared_ptr<Media> & media, Context * context)
        {
            IWindow::_init(context);

            DJV_PRIVATE_PTR();
            p.media = media;

            p.imageWidget = UI::ImageWidget::create(context);
            p.playbackWidget = PlaybackWidget::create(context);
            p.timelineSlider = TimelineSlider::create(context);

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);

            auto closeButton = UI::ToolButton::create(context);
            closeButton->setIcon(context->getPath(ResourcePath::IconsDirectory, "djvIconClose90DPI.png"));

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setClassName("djv::UI::MDI::TitleBar");
            p.titleBar->setBackgroundRole(UI::ColorRole::Overlay);
            p.titleBar->addWidget(p.titleLabel);
            p.titleBar->addExpander();
            p.titleBar->addWidget(closeButton);

            p.resizeHandle = UI::Icon::create(context);
            p.resizeHandle->setPointerEnabled(true);
            p.resizeHandle->setIcon(context->getPath(ResourcePath::IconsDirectory, "djvIconWindowResizeHandle90DPI.png"));
            p.resizeHandle->setHAlign(UI::HAlign::Right);
            p.resizeHandle->setVAlign(UI::VAlign::Bottom);

            p.bottomBar = UI::HorizontalLayout::create(context);
            p.bottomBar->setBackgroundRole(UI::ColorRole::Overlay);
            p.bottomBar->addWidget(p.playbackWidget);
            p.bottomBar->addWidget(p.timelineSlider, UI::RowLayoutStretch::Expand);
            p.bottomBar->addWidget(p.resizeHandle);

            p.layout = UI::StackLayout::create(context);
            p.layout->addWidget(p.imageWidget);

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(p.titleBar);
            vLayout->addExpander();
            vLayout->addWidget(p.bottomBar);
            p.layout->addWidget(vLayout);
            IContainerWidget::addWidget(p.layout);

            auto weak = std::weak_ptr<MDIWindow>(std::dynamic_pointer_cast<MDIWindow>(shared_from_this()));
            closeButton->setClickedCallback(
                [weak]
            {
                if (auto window = weak.lock())
                {
                    if (window->_p->closedCallback)
                    {
                        window->_p->closedCallback();
                    }
                }
            });

            p.imageObserver = ValueObserver<std::shared_ptr<AV::Image> >::create(
                media->getCurrentImage(),
                [weak](const std::shared_ptr<AV::Image> & image)
            {
                if (auto window = weak.lock())
                {
                    window->_p->imageWidget->setImage(image, createUID());
                }
            });

            p.durationObserver = ValueObserver<AV::Duration>::create(
                media->getDuration(),
                [weak](AV::Duration value)
            {
                if (auto window = weak.lock())
                {
                    window->_p->timelineSlider->setDuration(value);
                }
            });

            p.currentTimeObserver = ValueObserver<AV::Timestamp>::create(
                media->getCurrentTime(),
                [weak](AV::Timestamp value)
            {
                if (auto window = weak.lock())
                {
                    window->_p->timelineSlider->setCurrentTime(value);
                }
            });

            p.playbackObserver = ValueObserver<Playback>::create(
                media->getPlayback(),
                [weak](Playback value)
            {
                if (auto window = weak.lock())
                {
                    window->_p->playbackWidget->setPlayback(value);
                }
            });

            p.playbackObserver2 = ValueObserver<Playback>::create(
                p.playbackWidget->getPlayback(),
                [weak](Playback value)
            {
                if (auto window = weak.lock())
                {
                    window->_p->media->setPlayback(value);
                }
            });

            p.currentTimeObserver2 = ValueObserver<AV::Timestamp>::create(
                p.timelineSlider->getCurrentTime(),
                [weak](AV::Timestamp value)
            {
                if (auto window = weak.lock())
                {
                    window->_p->media->setCurrentTime(value);
                }
            });
        }

        MDIWindow::MDIWindow() :
            _p(new Private)
        {}

        MDIWindow::~MDIWindow()
        {}

        std::shared_ptr<MDIWindow> MDIWindow::create(const std::shared_ptr<Media> & media, Context * context)
        {
            auto out = std::shared_ptr<MDIWindow>(new MDIWindow);
            out->_init(media, context);
            return out;
        }

        const std::string & MDIWindow::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void MDIWindow::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void MDIWindow::setClosedCallback(const std::function<void(void)> & value)
        {
            _p->closedCallback = value;
        }

        std::shared_ptr<UI::Widget> MDIWindow::getMoveHandle()
        {
            return std::dynamic_pointer_cast<Widget>(shared_from_this());
        }

        std::shared_ptr<UI::Widget> MDIWindow::getResizeHandle()
        {
            return _p->resizeHandle;
        }

        void MDIWindow::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MDIWindow::layoutEvent(LayoutEvent&)
        {
            _p->layout->setGeometry(getGeometry());
        }
        
    } // namespace ViewLib
} // namespace djv


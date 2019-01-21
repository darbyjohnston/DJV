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

#include <djvViewLib/MediaWidget.h>

#include <djvViewLib/ImageView.h>
#include <djvViewLib/Media.h>
#include <djvViewLib/PlaybackWidget.h>
#include <djvViewLib/TimelineSlider.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Memory.h>
#include <djvCore/UID.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct MediaWidget::Private
        {
            std::shared_ptr<ImageView> imageView;
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::Layout::Horizontal> titleBar;
            std::shared_ptr<UI::Icon> resizeHandle;
            std::shared_ptr<UI::Layout::Horizontal> bottomBar;
            std::shared_ptr<UI::Layout::Stack> layout;
            std::function<void(void)> maximizeCallback;
            std::function<void(void)> closedCallback;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
        };
        
        void MediaWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.imageView = ImageView::create(context);

            p.layout = UI::Layout::Stack::create(context);
            p.layout->addWidget(p.imageView);
            p.layout->setParent(shared_from_this());
        }

        MediaWidget::MediaWidget() :
            _p(new Private)
        {}

        MediaWidget::~MediaWidget()
        {}

        std::shared_ptr<MediaWidget> MediaWidget::create(Context * context)
        {
            auto out = std::shared_ptr<MediaWidget>(new MediaWidget);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<Media> & MediaWidget::getMedia() const
        {
            return _p->imageView->getMedia();
        }

        void MediaWidget::setMedia(const std::shared_ptr<Media> & value)
        {
            _p->imageView->setMedia(value);
        }

        void MediaWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MediaWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }
        
    } // namespace ViewLib
} // namespace djv


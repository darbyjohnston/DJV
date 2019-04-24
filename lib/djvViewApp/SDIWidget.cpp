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

#include <djvViewApp/SDIWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SDIWidget::Private
        {
            std::shared_ptr<MediaWidget> mediaWidget;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
        };

        void SDIWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SDIWidget");

            p.mediaWidget = MediaWidget::create(context);
            addChild(p.mediaWidget);

            auto fileSystem = context->getSystemT<FileSystem>();
            auto weak = std::weak_ptr<SDIWidget>(std::dynamic_pointer_cast<SDIWidget>(shared_from_this()));
            p.currentMediaObserver = ValueObserver<std::shared_ptr<Media>>::create(
                fileSystem->observeCurrentMedia(),
                [weak](const std::shared_ptr<Media> & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->mediaWidget->setMedia(value);
                }
            });
        }

        SDIWidget::SDIWidget() :
            _p(new Private)
        {}

        SDIWidget::~SDIWidget()
        {}

        std::shared_ptr<SDIWidget> SDIWidget::create(Context * context)
        {
            auto out = std::shared_ptr<SDIWidget>(new SDIWidget);
            out->_init(context);
            return out;
        }

        void SDIWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->mediaWidget->getMinimumSize());
        }

        void SDIWidget::_layoutEvent(Event::Layout&)
        {
            _p->mediaWidget->setGeometry(getGeometry());
        }

        void SDIWidget::_localeEvent(Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
        }

    } // namespace ViewApp
} // namespace djv


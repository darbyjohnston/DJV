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

#include <djvViewLib/ImageView.h>

#include <djvViewLib/Media.h>

#include <djvAV/Render2DSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ImageView::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<AV::Image::Image> image;
            UID imageUID = 0;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
        };

        void ImageView::_init(Context * context)
        {
            Widget::_init(context);
        }

        ImageView::ImageView() :
            _p(new Private)
        {}

        ImageView::~ImageView()
        {}

        std::shared_ptr<ImageView> ImageView::create(Context * context)
        {
            auto out = std::shared_ptr<ImageView>(new ImageView);
            out->_init(context);
            return out;
        }

        void ImageView::setMedia(const std::shared_ptr<Media> & media)
        {
            DJV_PRIVATE_PTR();
            auto weak = std::weak_ptr<ImageView>(std::dynamic_pointer_cast<ImageView>(shared_from_this()));
            p.imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                media->getCurrentImage(),
                [weak](const std::shared_ptr<AV::Image::Image> & image)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->image = image;
                    widget->_p->imageUID = createUID();
                    widget->_redraw();
                }
            });
        }

        void ImageView::_preLayoutEvent(Event::PreLayout& event)
        {
        }

        void ImageView::_layoutEvent(Event::Layout&)
        {
        }

        void ImageView::_paintEvent(Core::Event::Paint&)
        {
            if (auto render = _getRenderSystem().lock())
            {
                if (auto style = _getStyle().lock())
                {
                    if (_p->image)
                    {
                        const BBox2f& g = getMargin().bbox(getGeometry(), style);
                        const glm::vec2 c = g.getCenter();
                        const glm::vec2& size = _p->image->getSize();
                        glm::vec2 pos = glm::vec2(0.f, 0.f);
                        render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f));
                        render->drawImage(_p->image, pos, AV::Render::Render2DSystem::ImageType::Dynamic, _p->imageUID);
                    }
                }
            }
        }

    } // namespace ViewLib
} // namespace djv


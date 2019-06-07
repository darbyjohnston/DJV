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

#include <djvViewApp/ImageView.h>

#include <djvViewApp/ImageViewSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/Action.h>

#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageView::Private
        {
            std::shared_ptr<Media> media;
            std::shared_ptr<AV::Image::Image> image;
            glm::vec2 imagePos = glm::vec2(0.f, 0.f);
            float imageZoom = 1.f;
            AV::Image::Color backgroundColor = AV::Image::Color(0.f, 0.f, 0.f);
            glm::vec2 pressedImagePos = glm::vec2(0.f, 0.f);
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > clickedObservers;
        };

        void ImageView::_init(Context * context)
        {
            Widget::_init(context);
            setClassName("djv::ViewApp::ImageView");
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

        const std::shared_ptr<Media> & ImageView::getMedia() const
        {
            return _p->media;
        }

        void ImageView::setMedia(const std::shared_ptr<Media> & media)
        {
            DJV_PRIVATE_PTR();
            if (media == p.media)
                return;
            p.media = media;
            if (p.media)
            {
                auto weak = std::weak_ptr<ImageView>(std::dynamic_pointer_cast<ImageView>(shared_from_this()));
                p.imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                    p.media->observeCurrentImage(),
                    [weak](const std::shared_ptr<AV::Image::Image> & image)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->image = image;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_redraw();
                        }
                    }
                });
                auto imageViewSystem = getContext()->getSystemT<ImageViewSystem>();
                p.clickedObservers["Left"] = ValueObserver<bool>::create(
                    imageViewSystem->getActions()["Left"]->observeClicked(),
                    [weak](bool value)
                {
                    if (value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setImagePos(widget->getImagePos() + glm::vec2(-10.f, 0.f));
                        }
                    }
                });
            }
            else
            {
                p.image.reset();
                p.imageObserver.reset();
                p.clickedObservers["Left"].reset();
                if (isVisible() && !isClipped())
                {
                    _redraw();
                }
            }
        }

        const glm::vec2& ImageView::getImagePos() const
        {
            return _p->imagePos;
        }

        float ImageView::getImageZoom() const
        {
            return _p->imageZoom;
        }

        void ImageView::setImagePos(const glm::vec2& value)
        {
            if (value == _p->imagePos)
                return;
            _p->imagePos = value;
            _redraw();
        }

        void ImageView::setImageZoom(float value)
        {
            if (value == _p->imageZoom)
                return;
            _p->imageZoom = value;
            _redraw();
        }

        const AV::Image::Color& ImageView::getBackgroundColor() const
        {
            return _p->backgroundColor;
        }

        void ImageView::setBackgroundColor(const AV::Image::Color& value)
        {
            if (value == _p->backgroundColor)
                return;
            _p->backgroundColor = value;
            _redraw();
        }

        void ImageView::_preLayoutEvent(Event::PreLayout & event)
        {
            auto style = _getStyle();
            const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
            _setMinimumSize(glm::vec2(sa, sa));
        }

        void ImageView::_layoutEvent(Event::Layout &)
        {}

        void ImageView::_paintEvent(Event::Paint &)
        {
            DJV_PRIVATE_PTR();
            if (_p->image)
            {
                auto style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();
                render->setFillColor(p.backgroundColor);
                render->drawRect(g);
                render->setFillColor(AV::Image::Color(1.f, 1.f, 1.f));
                const BBox2f imageGeometry(
                    g.min.x + p.imagePos.x * p.imageZoom,
                    g.min.y + p.imagePos.y * p.imageZoom,
                    _p->image->getWidth() * p.imageZoom,
                    _p->image->getHeight() * p.imageZoom);
                render->drawImage(_p->image, imageGeometry, AV::Render::ImageCache::Dynamic);
            }
        }

    } // namespace ViewApp
} // namespace djv


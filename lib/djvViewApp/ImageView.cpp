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

#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ViewSettings.h>

#include <djvUI/Action.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Image.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        GridOptions::GridOptions()
        {}

        bool GridOptions::operator == (const GridOptions& other) const
        {
            return enabled == other.enabled &&
                current == other.current &&
                size == other.size &&
                color == other.color;
        }

        struct ImageView::Private
        {
            std::shared_ptr<AV::Image::Image> image;
            std::shared_ptr<ValueSubject<AV::Render::ImageOptions> > imageOptions;
            AV::OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<ValueSubject<glm::vec2> > imagePos;
            std::shared_ptr<ValueSubject<float> > imageZoom;
            std::shared_ptr<ValueSubject<ImageRotate> > imageRotate;
            std::shared_ptr<ValueSubject<ImageAspectRatio> > imageAspectRatio;
            ImageViewLock lock = ImageViewLock::None;
            BBox2f lockFrame = BBox2f(0.F, 0.F, 0.F, 0.F);
            std::shared_ptr<ValueSubject<GridOptions> > gridOptions;
            std::vector<float> gridList;
            AV::Image::Color backgroundColor = AV::Image::Color(0.F, 0.F, 0.F);
            glm::vec2 pressedImagePos = glm::vec2(0.F, 0.F);
            bool viewInit = true;
            std::shared_ptr<ValueObserver<ImageViewLock> > lockObserver;
            std::shared_ptr<ListObserver<float> > gridListObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > ocioConfigObserver;
        };

        void ImageView::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);

            setClassName("djv::ViewApp::ImageView");

            DJV_PRIVATE_PTR();
            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            AV::Render::ImageOptions imageOptions;
            imageOptions.alphaBlend = avSystem->observeAlphaBlend()->get();
            p.imageOptions = ValueSubject<AV::Render::ImageOptions>::create(imageOptions);
            p.imagePos = ValueSubject<glm::vec2>::create();
            p.imageZoom = ValueSubject<float>::create();
            p.imageRotate = ValueSubject<ImageRotate>::create(imageSettings->observeRotate()->get());
            p.imageAspectRatio = ValueSubject<ImageAspectRatio>::create(imageSettings->observeAspectRatio()->get());
            p.gridOptions = ValueSubject<GridOptions>::create(viewSettings->observeGridOptions()->get());

            auto weak = std::weak_ptr<ImageView>(std::dynamic_pointer_cast<ImageView>(shared_from_this()));
            p.lockObserver = ValueObserver<ImageViewLock>::create(
                viewSettings->observeLock(),
                [weak](ImageViewLock value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->lock = value;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_resize();
                        }
                    }
                });

            p.gridListObserver = ListObserver<float>::create(
                viewSettings->observeGridList(),
                [weak](const std::vector<float>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridList = value;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_redraw();
                        }
                    }
                });

            p.backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                viewSettings->observeBackgroundColor(),
                [weak](const AV::Image::Color& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundColor = value;
                        if (widget->isVisible() && !widget->isClipped())
                        {
                            widget->_redraw();
                        }
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            auto contextWeak = std::weak_ptr<Context>(context);
            p.ocioConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](const AV::OCIO::Config& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                            widget->_p->ocioConfig = value;
                            widget->_p->outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                            widget->_redraw();
                        }
                    }
                });
        }

        ImageView::ImageView() :
            _p(new Private)
        {}

        ImageView::~ImageView()
        {}

        std::shared_ptr<ImageView> ImageView::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ImageView>(new ImageView);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<AV::Image::Image> & ImageView::getImage() const
        {
            return _p->image;
        }

        void ImageView::setImage(const std::shared_ptr<AV::Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.image)
                return;
            p.image = value;
            if (isVisible() && !isClipped())
            {
                _resize();
            }
        }

        std::shared_ptr<IValueSubject<AV::Render::ImageOptions> > ImageView::observeImageOptions() const
        {
            return _p->imageOptions;
        }

        void ImageView::setImageOptions(const AV::Render::ImageOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageOptions->setIfChanged(value))
            {
                if (isVisible() && !isClipped())
                {
                    _resize();
                }
            }
        }

        std::shared_ptr<IValueSubject<glm::vec2> > ImageView::observeImagePos() const
        {
            return _p->imagePos;
        }

        std::shared_ptr<IValueSubject<float> > ImageView::observeImageZoom() const
        {
            return _p->imageZoom;
        }

        std::shared_ptr<IValueSubject<ImageRotate> > ImageView::observeImageRotate() const
        {
            return _p->imageRotate;
        }

        std::shared_ptr<IValueSubject<ImageAspectRatio> > ImageView::observeImageAspectRatio() const
        {
            return _p->imageAspectRatio;
        }

        BBox2f ImageView::getImageBBox() const
        {
            return _getBBox(_getImagePoints());
        }

        float ImageView::getPixelAspectRatio() const
        {
            DJV_PRIVATE_PTR();
            float out = 1.F;
            switch (p.imageAspectRatio->get())
            {
            case ImageAspectRatio::Default:
                out = p.image ? p.image->getInfo().pixelAspectRatio : 1.F;
                break;
            default: break;
            }
            return out;
        }

        float ImageView::getAspectRatioScale() const
        {
            DJV_PRIVATE_PTR();
            float out = 1.F;
            switch (p.imageAspectRatio->get())
            {
            case ImageAspectRatio::_16_9:
            case ImageAspectRatio::_1_85:
            case ImageAspectRatio::_2_35:
                out = (p.image ? p.image->getAspectRatio() : 1.F) / getImageAspectRatio(p.imageAspectRatio->get());
                break;
            default: break;
            }
            return out;
        }

        void ImageView::setImagePos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (p.imagePos->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageZoom(float value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageZoom->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageZoomFocus(float value, const glm::vec2& mouse)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(
                mouse + (p.imagePos->get() - mouse) * (value / p.imageZoom->get()),
                value);
        }

        void ImageView::setImagePosAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            const bool posChanged = p.imagePos->setIfChanged(pos);
            const bool zoomChanged = p.imageZoom->setIfChanged(zoom);
            if (posChanged || zoomChanged)
            {
                _resize();
            }
        }

        void ImageView::setImageRotate(ImageRotate value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageRotate->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::setImageAspectRatio(ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageAspectRatio->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::imageFill()
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const BBox2f& g = getGeometry();
                const auto pts = _getImagePoints();
                const glm::vec2 c = _getCenter(pts);
                const BBox2f bbox = _getBBox(pts);
                float zoom = g.w() / static_cast<float>(bbox.w());
                if (zoom * bbox.h() > g.h())
                {
                    zoom = g.h() / static_cast<float>(bbox.h());
                }
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.F - c.x * zoom,
                        g.h() / 2.F - c.y * zoom),
                    zoom);
            }
        }

        void ImageView::setImageFrame(const BBox2f& value)
        {
            _p->lockFrame = value;
        }

        void ImageView::imageFrame()
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const BBox2f& g = getGeometry();
                const auto pts = _getImagePoints();
                const glm::vec2 c = _getCenter(pts);
                const BBox2f bbox = _getBBox(pts);
                float zoom = p.lockFrame.w() / static_cast<float>(bbox.w());
                if (zoom * bbox.h() > p.lockFrame.h())
                {
                    zoom = p.lockFrame.h() / static_cast<float>(bbox.h());
                }
                setImagePosAndZoom(
                    glm::vec2(
                        (p.lockFrame.min.x - g.min.x) + p.lockFrame.w() / 2.F - c.x * zoom,
                        (p.lockFrame.min.y - g.min.y) + p.lockFrame.h() / 2.F - c.y * zoom),
                    zoom);
            }
        }

        void ImageView::imageCenter()
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const BBox2f& g = getGeometry();
                const glm::vec2 c = _getCenter(_getImagePoints());
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.F - c.x,
                        g.h() / 2.F - c.y),
                    1.F);
            }
        }

        std::shared_ptr<Core::IValueSubject<GridOptions> > ImageView::observeGridOptions() const
        {
            return _p->gridOptions;
        }

        void ImageView::setGridOptions(const GridOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.gridOptions->setIfChanged(value))
            {
                _redraw();
            }
        }

        void ImageView::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
            _setMinimumSize(glm::vec2(sa, sa));
        }

        void ImageView::_layoutEvent(Event::Layout &)
        {
            DJV_PRIVATE_PTR();
            switch (p.lock)
            {
            case ImageViewLock::Fill:   imageFill();   break;
            case ImageViewLock::Frame:  imageFrame();  break;
            case ImageViewLock::Center: imageCenter(); break;
            default:
                if (p.image && p.viewInit)
                {
                    p.viewInit = false;
                    imageFill();
                }
                break;
            }
        }

        void ImageView::_paintEvent(Event::Paint &)
        {
            DJV_PRIVATE_PTR();
            if (p.image)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                auto render = _getRender();
                render->setFillColor(p.backgroundColor);
                render->drawRect(g);
                render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));

                glm::mat3x3 m(1.F);
                m = glm::translate(m, g.min + p.imagePos->get());
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(
                    p.imageZoom->get() * getPixelAspectRatio(),
                    p.imageZoom->get() * getAspectRatioScale()));
                render->pushTransform(m);
                AV::Render::ImageOptions options(p.imageOptions->get());
                auto i = p.ocioConfig.colorSpaces.find(p.image->getPluginName());
                if (i != p.ocioConfig.colorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.colorSpaces.find(std::string());
                    if (i != p.ocioConfig.colorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
                options.colorSpace.output = p.outputColorSpace;
                options.cache = AV::Render::ImageCache::Dynamic;
                render->drawImage(p.image, glm::vec2(0.F, 0.F), options);
                render->popTransform();
            }
            const auto& gridOptions = p.gridOptions->get();
            if (gridOptions.enabled && gridOptions.current >= 0 && gridOptions.current < static_cast<int>(p.gridList.size()))
            {
                _drawGrid(p.gridList[gridOptions.current]);
            }
        }

        std::vector<glm::vec3> ImageView::_getImagePoints() const
        {
            DJV_PRIVATE_PTR();
            std::vector<glm::vec3> out;
            if (p.image)
            {
                const AV::Image::Size& imageSize = p.image->getSize();
                glm::mat3x3 m(1.F);
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(getPixelAspectRatio(), getAspectRatioScale()));
                out.resize(4);
                out[0].x = 0.F;
                out[0].y = 0.F;
                out[0].z = 1.F;
                out[1].x = 0.F + imageSize.w;
                out[1].y = 0.F;
                out[1].z = 1.F;
                out[2].x = 0.F + imageSize.w;
                out[2].y = 0.F + imageSize.h;
                out[2].z = 1.F;
                out[3].x = 0.F;
                out[3].y = 0.F + imageSize.h;
                out[3].z = 1.F;
                for (auto& i : out)
                {
                    i = m * i;
                }
            }
            return out;
        }

        glm::vec2 ImageView::_getCenter(const std::vector<glm::vec3>& value)
        {
            glm::vec2 out(0.F, 0.F);
            if (value.size())
            {
                for (const auto& i : value)
                {
                    out.x += i.x;
                    out.y += i.y;
                }
                out.x /= static_cast<float>(value.size());
                out.y /= static_cast<float>(value.size());
            }
            return out;
        }
        
        BBox2f ImageView::_getBBox(const std::vector<glm::vec3>& value)
        {
            BBox2f out(0.F, 0.F, 0.F, 0.F);
            if (value.size())
            {
                out.min = out.max = value[0];
                for (size_t i = 1; i < 4; ++i)
                {
                    out.min.x = std::min(out.min.x, value[i].x);
                    out.max.x = std::max(out.max.x, value[i].x);
                    out.min.y = std::min(out.min.y, value[i].y);
                    out.max.y = std::max(out.max.y, value[i].y);
                }
            }
            return out;
        }

        void ImageView::_drawGrid(float gridSize)
        {
            DJV_PRIVATE_PTR();
            const float imageZoom = p.imageZoom->get();
            if (gridSize * imageZoom <= 2.f)
            {
                return;
            }
            const glm::vec2& imagePos = p.imagePos->get();

            const auto& style = _getStyle();
            const float b = style->getMetric(UI::MetricsRole::Border);
            const BBox2f& g = getMargin().bbox(getGeometry(), style);
            const BBox2f area(
                (floorf(-imagePos.x / imageZoom / gridSize) - 1.F) * gridSize,
                (floorf(-imagePos.y / imageZoom / gridSize) - 1.F) * gridSize,
                (ceilf(g.w() / imageZoom / gridSize) + 2.F) * gridSize,
                (ceilf(g.h() / imageZoom / gridSize) + 2.F) * gridSize);

            auto render = _getRender();
            const auto& color = p.gridOptions->get().color;
            render->setFillColor(color);
            for (float y = 0; y < area.h(); y += gridSize)
            {
                render->drawRect(BBox2f(
                    g.min.x + floorf(area.min.x * imageZoom + imagePos.x),
                    g.min.y + floorf((area.min.y + y) * imageZoom + imagePos.y),
                    ceilf(area.w() * imageZoom),
                    b));
            }
            for (float x = 0; x < area.w(); x += gridSize)
            {
                render->drawRect(BBox2f(
                    g.min.x + floorf((area.min.x + x) * imageZoom + imagePos.x),
                    g.min.y + floorf(area.min.y * imageZoom + imagePos.y),
                    b,
                    ceilf(area.h() * imageZoom)));
            }
        }

    } // namespace ViewApp

    picojson::value toJSON(const ViewApp::GridOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Enabled"] = toJSON(value.enabled);
        out.get<picojson::object>()["Current"] = toJSON(value.current);
        out.get<picojson::object>()["Size"] = toJSON(value.size);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        return out;
    }

    void fromJSON(const picojson::value& value, ViewApp::GridOptions& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
                if ("Enabled" == i.first)
                {
                    fromJSON(i.second, out.enabled);
                }
                else if ("Current" == i.first)
                {
                    fromJSON(i.second, out.current);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv


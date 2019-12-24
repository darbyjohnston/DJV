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

#include <djvViewApp/Annotate.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/ViewSettings.h>

#include <djvUI/Action.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Style.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Image.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t zoomAnimation = 200;
            
        } // namespace
        
        GridOptions::GridOptions()
        {}

        bool GridOptions::operator == (const GridOptions& other) const
        {
            return enabled == other.enabled &&
                size == other.size &&
                color == other.color &&
                labels == other.labels;
        }

        struct ImageView::Private
        {
            std::shared_ptr<AV::Font::System> fontSystem;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > image;
            std::shared_ptr<ValueSubject<AV::Render::ImageOptions> > imageOptions;
            AV::OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<ValueSubject<glm::vec2> > imagePos;
            std::shared_ptr<ValueSubject<float> > imageZoom;
            std::shared_ptr<ValueSubject<ImageRotate> > imageRotate;
            std::shared_ptr<ValueSubject<UI::ImageAspectRatio> > imageAspectRatio;
            ImageViewLock lock = ImageViewLock::None;
            BBox2f lockFrame = BBox2f(0.F, 0.F, 0.F, 0.F);
            std::shared_ptr<ValueSubject<GridOptions> > gridOptions;
            std::shared_ptr<ValueSubject<AV::Image::Color> > backgroundColor;
            std::vector<std::shared_ptr<AnnotatePrimitive> > annotations;
            glm::vec2 pressedImagePos = glm::vec2(0.F, 0.F);
            bool viewInit = true;
            AV::Font::Metrics fontMetrics;
            std::future<AV::Font::Metrics> fontMetricsFuture;
            struct Text
            {
                std::string text;
                glm::vec2 size = glm::vec2(0.F, 0.F);
                std::vector<std::shared_ptr<AV::Font::Glyph> > glyphs;
            };
            std::map<UI::Orientation, std::vector<Text> > text;
            std::map<UI::Orientation, std::vector<std::pair<size_t, std::future<glm::vec2> > > > textSizeFutures;
            std::map<UI::Orientation, std::vector<std::pair<size_t, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > > > textGlyphsFutures;
            float textWidthMax = 0.F;
            std::shared_ptr<ValueObserver<ImageViewLock> > lockObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > ocioConfigObserver;
            std::shared_ptr<Animation::Animation> zoomAnimation;
        };

        void ImageView::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageView");

            p.fontSystem = context->getSystemT<AV::Font::System>();

            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.image = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            AV::Render::ImageOptions imageOptions;
            imageOptions.alphaBlend = avSystem->observeAlphaBlend()->get();
            p.imageOptions = ValueSubject<AV::Render::ImageOptions>::create(imageOptions);
            p.imagePos = ValueSubject<glm::vec2>::create();
            p.imageZoom = ValueSubject<float>::create(1.F);
            p.imageRotate = ValueSubject<ImageRotate>::create(imageSettings->observeRotate()->get());
            p.imageAspectRatio = ValueSubject<UI::ImageAspectRatio>::create(imageSettings->observeAspectRatio()->get());
            p.gridOptions = ValueSubject<GridOptions>::create(viewSettings->observeGridOptions()->get());
            p.backgroundColor = ValueSubject<AV::Image::Color>::create(viewSettings->observeBackgroundColor()->get());

            _textUpdate();

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
                
            p.zoomAnimation = Animation::Animation::create(context);
            p.zoomAnimation->setType(Animation::Type::SmoothStep);
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

        std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image::Image> > > ImageView::observeImage() const
        {
            return _p->image;
        }

        void ImageView::setImage(const std::shared_ptr<AV::Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (p.image->setIfChanged(value))
            {
                _textUpdate();
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

        std::shared_ptr<IValueSubject<UI::ImageAspectRatio> > ImageView::observeImageAspectRatio() const
        {
            return _p->imageAspectRatio;
        }

        BBox2f ImageView::getImageBBox() const
        {
            return _getBBox(_getImagePoints());
        }

        void ImageView::setImagePos(const glm::vec2& value, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(value, p.imageZoom->get(), animate);
        }

        void ImageView::setImageZoom(float value, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(p.imagePos->get(), value, animate);
        }

        void ImageView::setImageZoomFocus(float value, const glm::vec2& mouse, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(
                mouse + (p.imagePos->get() - mouse) * (value / p.imageZoom->get()),
                value,
                animate);
        }

        void ImageView::setImageZoomFocus(float value, bool animate)
        {
            const BBox2f& g = getGeometry();
            const glm::vec2& c = g.getCenter();
            setImageZoomFocus(value, c, animate);
        }

        void ImageView::setImagePosAndZoom(const glm::vec2& pos, float zoom, bool animate)
        {
            if (animate)
            {
                _animate(pos, zoom);
            }
            else
            {
                _posAndZoom(pos, zoom);
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

        void ImageView::setImageAspectRatio(UI::ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageAspectRatio->setIfChanged(value))
            {
                _resize();
            }
        }

        void ImageView::imageFill(bool animate)
        {
            DJV_PRIVATE_PTR();
            if (p.image->get())
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
                    zoom,
                    animate);
            }
        }

        void ImageView::setImageFrame(const BBox2f& value)
        {
            _p->lockFrame = value;
        }

        void ImageView::imageFrame(bool animate)
        {
            DJV_PRIVATE_PTR();
            if (p.image->get())
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
                    zoom,
                    animate);
            }
        }

        void ImageView::imageCenter(bool animate)
        {
            DJV_PRIVATE_PTR();
            if (p.image->get())
            {
                const BBox2f& g = getGeometry();
                const glm::vec2 c = _getCenter(_getImagePoints());
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.F - c.x,
                        g.h() / 2.F - c.y),
                    1.F,
                    animate);
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
                _textUpdate();
            }
        }

        std::shared_ptr<Core::IValueSubject<AV::Image::Color> > ImageView::observeBackgroundColor() const
        {
            return _p->backgroundColor;
        }

        void ImageView::setBackgroundColor(const AV::Image::Color& value)
        {
            DJV_PRIVATE_PTR();
            if (p.backgroundColor->setIfChanged(value))
            {
                _redraw();
            }
        }
        
        void ImageView::setAnnotations(const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
        {
            _p->annotations = value;
            _redraw();
        }

        void ImageView::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
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
                    imageFill();
                }
                break;
            }
            if (p.image && p.viewInit)
            {
                p.viewInit = false;
            }
        }

        void ImageView::_paintEvent(Event::Paint &)
        {
            DJV_PRIVATE_PTR();

            const auto& style = _getStyle();
            const BBox2f & g = getMargin().bbox(getGeometry(), style);
            auto render = _getRender();
            render->setFillColor(p.backgroundColor->get());
            render->drawRect(g);

            const float zoom = p.imageZoom->get();
            const glm::vec2& pos = p.imagePos->get();
            if (auto image = p.image->get())
            {
                render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));

                glm::mat3x3 m(1.F);
                m = glm::translate(m, g.min + pos);
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(
                    zoom * UI::getPixelAspectRatio(p.imageAspectRatio->get(), image->getInfo().pixelAspectRatio),
                    zoom * UI::getAspectRatioScale(p.imageAspectRatio->get(), image->getAspectRatio())));
                render->pushTransform(m);
                AV::Render::ImageOptions options(p.imageOptions->get());
                auto i = p.ocioConfig.fileColorSpaces.find(image->getPluginName());
                if (i != p.ocioConfig.fileColorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.fileColorSpaces.find(std::string());
                    if (i != p.ocioConfig.fileColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
                options.colorSpace.output = p.outputColorSpace;
                options.cache = AV::Render::ImageCache::Dynamic;
                render->drawImage(image, glm::vec2(0.F, 0.F), options);
                render->popTransform();
            }
            
            const auto& gridOptions = p.gridOptions->get();
            if (gridOptions.enabled)
            {
                _drawGrid(gridOptions.size);
            }

            if (p.annotations.size())
            {
                glm::mat3x3 m(1.F);
                m = glm::translate(m, g.min + pos);
                m = glm::scale(m, glm::vec2(zoom, zoom));
                render->pushTransform(m);
                for (const auto& i : p.annotations)
                {
                    i->draw(render);
                }
                render->popTransform();
            }
        }

        void ImageView::_initEvent(Event::Init& event)
        {
            Widget::_initEvent(event);
            _textUpdate();
        }

        void ImageView::_updateEvent(Event::Update& event)
        {
            Widget::_updateEvent(event);
            DJV_PRIVATE_PTR();
            if (p.fontMetricsFuture.valid() &&
                p.fontMetricsFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                try
                {
                    p.fontMetrics = p.fontMetricsFuture.get();
                    _redraw();
                }
                catch (const std::exception & e)
                {
                    _log(e.what(), LogLevel::Error);
                }
            }
            for (auto& i : p.textSizeFutures)
            {
                auto j = i.second.begin();
                while (j != i.second.end())
                {
                    if (j->second.valid() &&
                        j->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            const glm::vec2 size = j->second.get();
                            p.text[i.first][j->first].size = size;
                            p.textWidthMax = std::max(p.textWidthMax, size.x);
                            _redraw();
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        j = i.second.erase(j);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
            for (auto& i : p.textGlyphsFutures)
            {
                auto j = i.second.begin();
                while (j != i.second.end())
                {
                    if (j->second.valid() &&
                        j->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        try
                        {
                            p.text[i.first][j->first].glyphs = j->second.get();
                            _redraw();
                        }
                        catch (const std::exception & e)
                        {
                            _log(e.what(), LogLevel::Error);
                        }
                        j = i.second.erase(j);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
        }

        std::vector<glm::vec3> ImageView::_getImagePoints() const
        {
            DJV_PRIVATE_PTR();
            std::vector<glm::vec3> out;
            if (auto image = p.image->get())
            {
                const AV::Image::Size& imageSize = image->getSize();
                glm::mat3x3 m(1.F);
                m = glm::rotate(m, Math::deg2rad(getImageRotate(p.imageRotate->get())));
                m = glm::scale(m, glm::vec2(
                    getPixelAspectRatio(p.imageAspectRatio->get(), image->getInfo().pixelAspectRatio),
                    getAspectRatioScale(p.imageAspectRatio->get(), image->getAspectRatio())));
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

        void ImageView::_animate(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            const glm::vec2 posPrev = p.imagePos->get();
            const float zoomPrev = p.imageZoom->get();
            auto weak = std::weak_ptr<ImageView>(std::dynamic_pointer_cast<ImageView>(shared_from_this()));
            p.zoomAnimation->start(
                0.F,
                1.F,
                std::chrono::milliseconds(zoomAnimation),
                [weak, pos, zoom, posPrev, zoomPrev](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        const glm::vec2 posTmp(
                            Math::lerp(value, posPrev.x, pos.x),
                            Math::lerp(value, posPrev.y, pos.y));
                        const float zoomTmp = Math::lerp(value, zoomPrev, zoom);
                        widget->_posAndZoom(posTmp, zoomTmp);
                    }
                },
                [weak, pos, zoom](float)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_posAndZoom(pos, zoom);
                    }
                });
        }

        void ImageView::_posAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            if (p.imagePos->setIfChanged(pos))
            {
                _resize();
            }
            if (p.imageZoom->setIfChanged(std::max(0.F, zoom)))
            {
                _textUpdate();
            }
        }

        void ImageView::_drawGrid(float gridSize)
        {
            DJV_PRIVATE_PTR();
            const float zoom = _p->imageZoom->get();
            const float gridSizeZoom = gridSize * zoom;
            if (gridSizeZoom > 2.f)
            {
                const glm::vec2& imagePos = p.imagePos->get();

                const auto& style = _getStyle();
                const float m = style->getMetric(UI::MetricsRole::MarginSmall);
                const float b = style->getMetric(UI::MetricsRole::Border);
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const BBox2f area(
                    (floorf(-imagePos.x / zoom / gridSize) - 1.F) * gridSize,
                    (floorf(-imagePos.y / zoom / gridSize) - 1.F) * gridSize,
                    (ceilf(g.w() / zoom / gridSize) + 2.F) * gridSize,
                    (ceilf(g.h() / zoom / gridSize) + 2.F) * gridSize);

                auto render = _getRender();
                const auto& gridOptions = p.gridOptions->get();
                const float opacity = Math::clamp((gridSizeZoom - 2.F) / 10.F, 0.F, 1.F);
                auto gridColor = gridOptions.color.convert(AV::Image::Type::RGBA_F32);
                gridColor.setF32(gridColor.getF32(3) * opacity, 3);
                render->setFillColor(gridColor);
                for (float y = 0; y < area.h(); y += gridSize)
                {
                    render->drawRect(BBox2f(
                        g.min.x + floorf(area.min.x * zoom + imagePos.x),
                        g.min.y + floorf((area.min.y + y) * zoom + imagePos.y),
                        ceilf(area.w() * zoom),
                        b));
                }
                for (float x = 0; x < area.w(); x += gridSize)
                {
                    render->drawRect(BBox2f(
                        g.min.x + floorf((area.min.x + x) * zoom + imagePos.x),
                        g.min.y + floorf(area.min.y * zoom + imagePos.y),
                        b,
                        ceilf(area.h() * zoom)));
                }

                if (gridOptions.labels && (p.textWidthMax + m * 2.F) < gridSizeZoom)
                {
                    render->setFillColor(style->getColor(UI::ColorRole::OverlayLight));
                    float x = g.min.x + imagePos.x;
                    float y = p.lockFrame.min.y;
                    for (size_t i = 0; i < p.text[UI::Orientation::Horizontal].size(); ++i)
                    {
                        render->drawRect(BBox2f(x, y, p.text[UI::Orientation::Horizontal][i].size.x + m * 2.F, p.fontMetrics.lineHeight + m * 2.f));
                        x += gridSizeZoom;
                    }
                    x = p.lockFrame.min.x;
                    y = g.min.y + imagePos.y;
                    for (size_t i = 0; i < p.text[UI::Orientation::Vertical].size(); ++i)
                    {
                        render->drawRect(BBox2f(x, y, p.text[UI::Orientation::Vertical][i].size.x + m * 2.F, p.fontMetrics.lineHeight + m * 2.f));
                        y += gridSizeZoom;
                    }

                    render->setFillColor(style->getColor(UI::ColorRole::Foreground));
                    render->setCurrentFont(style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall));
                    x = g.min.x + imagePos.x + m;
                    y = p.lockFrame.min.y + m;
                    for (size_t i = 0; i < p.text[UI::Orientation::Horizontal].size(); ++i)
                    {
                        render->drawText(p.text[UI::Orientation::Horizontal][i].glyphs, glm::vec2(floorf(x), floorf(y + p.fontMetrics.ascender - 1.F)));
                        x += gridSizeZoom;
                    }
                    x = p.lockFrame.min.x + m;
                    y = g.min.y + imagePos.y + m;
                    for (size_t i = 0; i < p.text[UI::Orientation::Vertical].size(); ++i)
                    {
                        render->drawText(p.text[UI::Orientation::Vertical][i].glyphs, glm::vec2(floorf(x), floorf(y + p.fontMetrics.ascender - 1.F)));
                        y += gridSizeZoom;
                    }
                }
            }
        }

        namespace
        {
            std::string getColumnLabel(size_t value)
            {
                std::stringstream ss;
                ss << "X" << value;
                return ss.str();
            }

            std::string getRowLabel(size_t value)
            {
                std::stringstream ss;
                ss << "Y" << value;
                return ss.str();
            }

        } // namespace

        void ImageView::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.textSizeFutures.clear();
            p.textGlyphsFutures.clear();
            if (auto image = p.image->get())
            {
                const auto& gridOptions = p.gridOptions->get();
                const float gridSize = gridOptions.size;
                if (gridOptions.labels && (gridSize * _p->imageZoom->get()) > 2.f)
                {
                    for (auto i : UI::getOrientationEnums())
                    {
                        p.textSizeFutures[i] = std::vector<std::pair<size_t, std::future<glm::vec2> > >();
                        p.textGlyphsFutures[i] = std::vector< std::pair<size_t, std::future<std::vector<std::shared_ptr<AV::Font::Glyph> > > > >();
                    }
                    const auto& style = _getStyle();
                    const auto fontInfo = style->getFontInfo(AV::Font::familyMono, AV::Font::faceDefault, UI::MetricsRole::FontSmall);
                    p.fontMetricsFuture = p.fontSystem->getMetrics(fontInfo);
                    const AV::Image::Size& imageSize = image->getSize();
                    size_t cells = static_cast<size_t>(imageSize.w / gridSize + 1);
                    p.text[UI::Orientation::Horizontal].resize(cells);
                    for (size_t i = 0; i < cells; ++i)
                    {
                        const std::string label = getColumnLabel(i);
                        p.text[UI::Orientation::Horizontal][i].text = label;
                        p.textSizeFutures[UI::Orientation::Horizontal].push_back(std::make_pair(i, p.fontSystem->measure(label, fontInfo)));
                        p.textGlyphsFutures[UI::Orientation::Horizontal].push_back(std::make_pair(i, p.fontSystem->getGlyphs(label, fontInfo)));
                    }
                    cells = static_cast<size_t>(imageSize.h / gridSize + 1);
                    p.text[UI::Orientation::Vertical].resize(cells);
                    for (size_t i = 0; i < cells; ++i)
                    {
                        const std::string label = getRowLabel(i);
                        p.text[UI::Orientation::Vertical][i].text = label;
                        p.textSizeFutures[UI::Orientation::Vertical].push_back(std::make_pair(i, p.fontSystem->measure(label, fontInfo)));
                        p.textGlyphsFutures[UI::Orientation::Vertical].push_back(std::make_pair(i, p.fontSystem->getGlyphs(label, fontInfo)));
                    }
                }
            }
        }

    } // namespace ViewApp

    picojson::value toJSON(const ViewApp::GridOptions& value)
    {
        picojson::value out(picojson::object_type, true);
        out.get<picojson::object>()["Enabled"] = toJSON(value.enabled);
        out.get<picojson::object>()["Size"] = toJSON(value.size);
        out.get<picojson::object>()["Color"] = toJSON(value.color);
        out.get<picojson::object>()["Labels"] = toJSON(value.labels);
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
                else if ("Size" == i.first)
                {
                    fromJSON(i.second, out.size);
                }
                else if ("Color" == i.first)
                {
                    fromJSON(i.second, out.color);
                }
                else if ("Labels" == i.first)
                {
                    fromJSON(i.second, out.labels);
                }
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv


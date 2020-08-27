// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewWidget.h>

#include <djvViewApp/Annotate.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/View.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidgetPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Image.h>
#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

#include <djvCore/Animation.h>
#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

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

        struct ViewWidget::Private
        {
            Time::Units timeUnits = Time::Units::First;
            std::shared_ptr<Media> media;
            std::shared_ptr<ValueSubject<std::shared_ptr<AV::Image::Image> > > image;
            std::shared_ptr<ValueSubject<AV::Render2D::ImageOptions> > imageOptions;
            AV::OCIO::Config ocioConfig;
            std::string outputColorSpace;
            std::shared_ptr<ValueSubject<glm::vec2> > imagePos;
            std::shared_ptr<ValueSubject<float> > imageZoom;
            std::shared_ptr<ValueSubject<UI::ImageRotate> > imageRotate;
            std::shared_ptr<ValueSubject<UI::ImageAspectRatio> > imageAspectRatio;
            ViewLock lock = ViewLock::None;
            std::shared_ptr<ValueSubject<GridOptions> > gridOptions;
            std::shared_ptr<ValueSubject<HUDOptions> > hudOptions;
            std::shared_ptr<ValueSubject<ViewBackgroundOptions> > backgroundOptions;
            Math::Rational speed;
            float realSpeed = 0.F;
            Frame::Sequence sequence;
            Frame::Index currentFrame = Frame::invalidIndex;
            std::vector<std::shared_ptr<AnnotatePrimitive> > annotations;
            glm::vec2 pressedImagePos = glm::vec2(0.F, 0.F);
            bool viewInit = true;

            std::shared_ptr<GridOverlay> gridOverlay;
            std::shared_ptr<HUDOverlay> hudOverlay;
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<ValueObserver<Time::Units> > timeUnitsObserver;
            std::shared_ptr<ValueObserver<std::pair<std::vector<AV::Image::Info>, int> > > layersObserver;
            std::shared_ptr<ValueObserver<ViewLock> > lockObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::Config> > ocioConfigObserver;
            std::shared_ptr<ValueObserver<Math::Rational> > speedObserver;
            std::shared_ptr<ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<ValueObserver<Frame::Sequence> > sequenceObserver;
            std::shared_ptr<ValueObserver<Frame::Index> > currentFrameObserver;

            std::shared_ptr<Animation::Animation> zoomAnimation;
        };

        void ViewWidget::_init(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ViewWidget");

            p.media = media;
            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.image = ValueSubject<std::shared_ptr<AV::Image::Image> >::create();
            AV::Render2D::ImageOptions imageOptions;
            imageOptions.alphaBlend = avSystem->observeAlphaBlend()->get();
            imageOptions.colorEnabled = true;
            imageOptions.levelsEnabled = true;
            imageOptions.softClipEnabled = true;
            p.imageOptions = ValueSubject<AV::Render2D::ImageOptions>::create(imageOptions);
            p.imagePos = ValueSubject<glm::vec2>::create();
            p.imageZoom = ValueSubject<float>::create(1.F);
            p.imageRotate = ValueSubject<UI::ImageRotate>::create(imageSettings->observeRotate()->get());
            p.imageAspectRatio = ValueSubject<UI::ImageAspectRatio>::create(imageSettings->observeAspectRatio()->get());
            p.gridOptions = ValueSubject<GridOptions>::create(viewSettings->observeGridOptions()->get());
            p.hudOptions = ValueSubject<HUDOptions>::create(viewSettings->observeHUDOptions()->get());
            p.backgroundOptions = ValueSubject<ViewBackgroundOptions>::create(viewSettings->observeBackgroundOptions()->get());

            p.gridOverlay = GridOverlay::create(context);
            p.gridOverlay->setOptions(p.gridOptions->get());
            p.gridOverlay->setImagePosAndZoom(p.imagePos->get(), p.imageZoom->get());
            p.gridOverlay->setImageRotate(p.imageRotate->get());
            p.gridOverlay->setImageAspectRatio(p.imageAspectRatio->get(), 1.F, 1.F);
            p.gridOverlay->setImageBBox(getImageBBox());

            p.hudOverlay = HUDOverlay::create(context);
            p.hudOverlay->setHUDOptions(p.hudOptions->get());

            p.layout = UI::StackLayout::create(context);
            p.layout->addChild(p.gridOverlay);
            p.layout->addChild(p.hudOverlay);
            addChild(p.layout);

            _hudUpdate();

            auto weak = std::weak_ptr<ViewWidget>(std::dynamic_pointer_cast<ViewWidget>(shared_from_this()));
            p.timeUnitsObserver = ValueObserver<Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_hudUpdate();
                    }
                });

            p.layersObserver = ValueObserver<std::pair<std::vector<AV::Image::Info>, int> >::create(
                p.media->observeLayers(),
                [weak](const std::pair<std::vector<AV::Image::Info>, int>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_hudUpdate();
                    }
                });

            p.lockObserver = ValueObserver<ViewLock>::create(
                viewSettings->observeLock(),
                [weak](ViewLock value)
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

            p.speedObserver = ValueObserver<Math::Rational>::create(
                p.media->observeSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_hudUpdate();
                    }
                });

            p.realSpeedObserver = ValueObserver<float>::create(
                p.media->observeRealSpeed(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->realSpeed = value;
                        widget->_hudUpdate();
                    }
                });

            p.sequenceObserver = ValueObserver<Frame::Sequence>::create(
                p.media->observeSequence(),
                [weak](const Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->sequence = value;
                        widget->_hudUpdate();
                    }
                });

            p.currentFrameObserver = ValueObserver<Frame::Index>::create(
                p.media->observeCurrentFrame(),
                [weak](Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_hudUpdate();
                    }
                });
                
            p.zoomAnimation = Animation::Animation::create(context);
            p.zoomAnimation->setType(Animation::Type::SmoothStep);
        }

        ViewWidget::ViewWidget() :
            _p(new Private)
        {}

        ViewWidget::~ViewWidget()
        {}

        std::shared_ptr<ViewWidget> ViewWidget::create(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ViewWidget>(new ViewWidget);
            out->_init(media, context);
            return out;
        }

        std::shared_ptr<IValueSubject<std::shared_ptr<AV::Image::Image> > > ViewWidget::observeImage() const
        {
            return _p->image;
        }

        void ViewWidget::setImage(const std::shared_ptr<AV::Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (p.image->setIfChanged(value))
            {
                auto image = p.image->get();
                p.gridOverlay->setImageAspectRatio(
                    p.imageAspectRatio->get(),
                    image ? image->getAspectRatio() : 1.F,
                    image ? image->getInfo().pixelAspectRatio : 1.F);
                p.gridOverlay->setImageBBox(getImageBBox());
                _resize();
            }
        }

        std::shared_ptr<IValueSubject<AV::Render2D::ImageOptions> > ViewWidget::observeImageOptions() const
        {
            return _p->imageOptions;
        }

        void ViewWidget::setImageOptions(const AV::Render2D::ImageOptions& value)
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

        std::shared_ptr<IValueSubject<glm::vec2> > ViewWidget::observeImagePos() const
        {
            return _p->imagePos;
        }

        std::shared_ptr<IValueSubject<float> > ViewWidget::observeImageZoom() const
        {
            return _p->imageZoom;
        }

        std::shared_ptr<IValueSubject<UI::ImageRotate> > ViewWidget::observeImageRotate() const
        {
            return _p->imageRotate;
        }

        std::shared_ptr<IValueSubject<UI::ImageAspectRatio> > ViewWidget::observeImageAspectRatio() const
        {
            return _p->imageAspectRatio;
        }

        BBox2f ViewWidget::getImageBBox() const
        {
            return _getBBox(_getImagePoints());
        }

        void ViewWidget::setImagePos(const glm::vec2& value, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(value, p.imageZoom->get(), animate);
        }

        void ViewWidget::setImageZoom(float value, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(p.imagePos->get(), value, animate);
        }

        void ViewWidget::setImageZoomFocus(float value, const glm::vec2& mouse, bool animate)
        {
            DJV_PRIVATE_PTR();
            setImagePosAndZoom(
                mouse + (p.imagePos->get() - mouse) * (value / p.imageZoom->get()),
                value,
                animate);
        }

        void ViewWidget::setImageZoomFocus(float value, bool animate)
        {
            const BBox2f& g = getGeometry();
            const glm::vec2& c = g.getCenter();
            setImageZoomFocus(value, c, animate);
        }

        void ViewWidget::setImagePosAndZoom(const glm::vec2& pos, float zoom, bool animate)
        {
            if (animate)
            {
                _animatePosAndZoom(pos, zoom);
            }
            else
            {
                _setPosAndZoom(pos, zoom);
            }
        }

        void ViewWidget::setImageRotate(UI::ImageRotate value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageRotate->setIfChanged(value))
            {
                p.gridOverlay->setImageRotate(value);
                p.gridOverlay->setImageBBox(getImageBBox());
                _resize();
            }
        }

        void ViewWidget::setImageAspectRatio(UI::ImageAspectRatio value)
        {
            DJV_PRIVATE_PTR();
            if (p.imageAspectRatio->setIfChanged(value))
            {
                p.gridOverlay->setImageAspectRatio(
                    p.imageAspectRatio->get(),
                    p.image ? p.image->get()->getAspectRatio() : 1.F,
                    p.image ? p.image->get()->getInfo().pixelAspectRatio : 1.F);
                p.gridOverlay->setImageBBox(getImageBBox());
                _resize();
            }
        }

        void ViewWidget::imageFrame(bool animate)
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

        void ViewWidget::imageCenter(bool animate)
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

        std::shared_ptr<IValueSubject<GridOptions> > ViewWidget::observeGridOptions() const
        {
            return _p->gridOptions;
        }

        void ViewWidget::setGridOptions(const GridOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.gridOptions->setIfChanged(value))
            {
                p.gridOverlay->setOptions(value);
            }
        }

        std::shared_ptr<Core::IValueSubject<HUDOptions> > ViewWidget::observeHUDOptions() const
        {
            return _p->hudOptions;
        }

        void ViewWidget::setHUDOptions(const HUDOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.hudOptions->setIfChanged(value))
            {
                p.hudOverlay->setHUDOptions(value);
            }
        }

        std::shared_ptr<IValueSubject<ViewBackgroundOptions> > ViewWidget::observeBackgroundOptions() const
        {
            return _p->backgroundOptions;
        }

        void ViewWidget::setBackgroundOptions(const ViewBackgroundOptions& value)
        {
            DJV_PRIVATE_PTR();
            if (p.backgroundOptions->setIfChanged(value))
            {
                _redraw();
            }
        }
        
        void ViewWidget::setAnnotations(const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
        {
            _p->annotations = value;
            _redraw();
        }

        void ViewWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
            glm::vec2 size = _p->layout->getMinimumSize();
            size.x = std::max(size.x, sa);
            size.y = std::max(size.x, sa);
            _setMinimumSize(size);
        }

        void ViewWidget::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            switch (p.lock)
            {
            case ViewLock::Frame:  imageFrame();  break;
            case ViewLock::Center: imageCenter(); break;
            default:
                if (p.image && p.viewInit)
                {
                    imageFrame();
                }
                break;
            }
            if (p.image && p.viewInit)
            {
                p.viewInit = false;
            }
            const BBox2f& g = getGeometry();
            p.layout->setGeometry(g);
            p.gridOverlay->setImageBBox(getImageBBox());
        }

        void ViewWidget::_paintEvent(Event::Paint &)
        {
            DJV_PRIVATE_PTR();

            const auto& style = _getStyle();
            const BBox2f& g = getMargin().bbox(getGeometry(), style);

            // Draw the background.
            const auto& backgroundOptions = p.backgroundOptions->get();
            const auto& render = _getRender();
            switch (backgroundOptions.background)
            {
            case ViewBackground::Solid:
                render->setFillColor(backgroundOptions.color);
                render->drawRect(g);
                break;
            case ViewBackground::Checkers:
                UI::drawCheckers(
                    render,
                    g,
                    backgroundOptions.checkersSize,
                    backgroundOptions.checkersColors[0],
                    backgroundOptions.checkersColors[1]);
                break;
            default: break;
            }
            auto image = p.image->get();
            const float zoom = p.imageZoom->get();
            if (backgroundOptions.border && image)
            {
                const auto points = _getImagePoints(true);
                const BBox2f bbox = _getBBox(points);
                render->setFillColor(backgroundOptions.borderColor);
                UI::drawBorder(
                    render,
                    bbox.margin(backgroundOptions.borderWidth),
                    backgroundOptions.borderWidth);
            }

            // Draw the image.
            if (image)
            {
                glm::mat3x3 m(1.F);
                m = glm::translate(m, g.min + p.imagePos->get());
                m *= UI::ImageWidget::getXForm(image, p.imageRotate->get(), glm::vec2(zoom, zoom), p.imageAspectRatio->get());
                render->pushTransform(m);
                render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));
                AV::Render2D::ImageOptions options(p.imageOptions->get());
                auto i = p.ocioConfig.imageColorSpaces.find(image->getPluginName());
                if (i != p.ocioConfig.imageColorSpaces.end())
                {
                    options.colorSpace.input = i->second;
                }
                else
                {
                    i = p.ocioConfig.imageColorSpaces.find(std::string());
                    if (i != p.ocioConfig.imageColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                }
                options.colorSpace.output = p.outputColorSpace;
                options.cache = AV::Render2D::ImageCache::Dynamic;
                render->drawImage(image, glm::vec2(0.F, 0.F), options);
                render->popTransform();
            }

            // Draw the annotations.
            if (p.annotations.size())
            {
                const glm::vec2& pos = p.imagePos->get();
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

        std::vector<glm::vec3> ViewWidget::_getImagePoints(bool posAndZoom) const
        {
            DJV_PRIVATE_PTR();
            std::vector<glm::vec3> out;
            if (auto image = p.image->get())
            {
                const AV::Image::Size& size = image->getSize();
                out.resize(4);
                out[0].x = 0.F;
                out[0].y = 0.F;
                out[0].z = 1.F;
                out[1].x = 0.F + size.w;
                out[1].y = 0.F;
                out[1].z = 1.F;
                out[2].x = 0.F + size.w;
                out[2].y = 0.F + size.h;
                out[2].z = 1.F;
                out[3].x = 0.F;
                out[3].y = 0.F + size.h;
                out[3].z = 1.F;
                glm::mat3x3 m(1.F);
                if (posAndZoom)
                {
                    const auto& style = _getStyle();
                    const BBox2f& g = getMargin().bbox(getGeometry(), style);
                    m = glm::translate(m, g.min + p.imagePos->get());
                }
                const float zoom = p.imageZoom->get();
                m *= UI::ImageWidget::getXForm(
                    image,
                    p.imageRotate->get(),
                    posAndZoom ? glm::vec2(zoom, zoom) : glm::vec2(1.F, 1.F),
                    p.imageAspectRatio->get());
                for (auto& i : out)
                {
                    i = m * i;
                }
            }
            return out;
        }

        glm::vec2 ViewWidget::_getCenter(const std::vector<glm::vec3>& value)
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
        
        BBox2f ViewWidget::_getBBox(const std::vector<glm::vec3>& value)
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

        void ViewWidget::_animatePosAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            const glm::vec2 posPrev = p.imagePos->get();
            const float zoomPrev = p.imageZoom->get();
            auto weak = std::weak_ptr<ViewWidget>(std::dynamic_pointer_cast<ViewWidget>(shared_from_this()));
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
                        widget->_setPosAndZoom(posTmp, zoomTmp);
                    }
                },
                [weak, pos, zoom](float)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPosAndZoom(pos, zoom);
                    }
                });
        }

        void ViewWidget::_setPosAndZoom(const glm::vec2& pos, float zoom)
        {
            DJV_PRIVATE_PTR();
            if (p.imagePos->setIfChanged(pos))
            {
                _resize();
            }
            if (p.imageZoom->setIfChanged(std::max(0.F, zoom)))
            {
                _resize();
            }
            p.gridOverlay->setImagePosAndZoom(p.imagePos->get(), p.imageZoom->get());
            p.gridOverlay->setImageBBox(getImageBBox());
        }

        void ViewWidget::_hudUpdate()
        {
            DJV_PRIVATE_PTR();
            HUDData data;
            data.fileName = p.media->getFileInfo().getFileName(Frame::invalid, false);
            const auto& layers = p.media->observeLayers()->get();
            if (layers.second >= 0 && layers.second < static_cast<int>(layers.first.size()))
            {
                const auto& layer = layers.first[layers.second];
                data.layer = layer.name;
                data.size = layer.size;
                data.type = layer.type;
            }
            data.isSequence = p.sequence.getFrameCount() > 1;
            data.currentFrame = Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
            data.speed = p.speed;
            data.realSpeed = p.realSpeed;
            p.hudOverlay->setHUDData(data);
        }

    } // namespace ViewApp
} // namespace djv


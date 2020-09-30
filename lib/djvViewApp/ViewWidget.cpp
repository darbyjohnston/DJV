// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewWidget.h>

#include <djvViewApp/Annotate.h>
#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/ViewData.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidgetPrivate.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/Style.h>

#include <djvRender2D/Render.h>

#include <djvAV/AVSystem.h>
#include <djvAV/TimeFunc.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Animation.h>
#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>

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
            AV::Time::Units timeUnits = AV::Time::Units::First;
            std::shared_ptr<Media> media;
            std::shared_ptr<Observer::ValueSubject<std::shared_ptr<Image::Image> > > image;
            std::shared_ptr<Observer::ValueSubject<glm::vec2> > imagePos;
            std::shared_ptr<Observer::ValueSubject<float> > imageZoom;
            ViewLock lock = ViewLock::None;
            GridOptions gridOptions;
            HUDOptions hudOptions;
            ViewBackgroundOptions backgroundOptions;
            ImageData imageData;
            OCIO::Config ocioConfig;
            std::string outputColorSpace;
            Math::Rational speed;
            float realSpeed = 0.F;
            Math::Frame::Sequence sequence;
            Math::Frame::Index currentFrame = Math::Frame::invalidIndex;
            std::vector<std::shared_ptr<AnnotatePrimitive> > annotations;
            glm::vec2 pressedImagePos = glm::vec2(0.F, 0.F);
            bool viewInit = true;

            std::shared_ptr<GridOverlay> gridOverlay;
            std::shared_ptr<HUDOverlay> hudOverlay;
            std::shared_ptr<UI::StackLayout> layout;

            std::shared_ptr<Observer::ValueObserver<AV::Time::Units> > timeUnitsObserver;
            std::shared_ptr<Observer::ValueObserver<ViewLock> > lockObserver;
            std::shared_ptr<Observer::ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<Observer::ValueObserver<HUDOptions> > hudOptionsObserver;
            std::shared_ptr<Observer::ValueObserver<ViewBackgroundOptions> > backgroundOptionsObserver;
            std::shared_ptr<Observer::ValueObserver<ImageData> > imageDataObserver;
            std::shared_ptr<Observer::ValueObserver<OCIO::Config> > ocioConfigObserver;
            std::shared_ptr<Observer::ValueObserver<std::pair<std::vector<Image::Info>, int> > > layersObserver;
            std::shared_ptr<Observer::ValueObserver<Math::Rational> > speedObserver;
            std::shared_ptr<Observer::ValueObserver<float> > realSpeedObserver;
            std::shared_ptr<Observer::ValueObserver<Math::Frame::Sequence> > sequenceObserver;
            std::shared_ptr<Observer::ValueObserver<Math::Frame::Index> > currentFrameObserver;
            std::shared_ptr<Observer::ListObserver<std::shared_ptr<AnnotatePrimitive> > > annotationsObserver;

            std::shared_ptr<System::Animation::Animation> zoomAnimation;
        };

        void ViewWidget::_init(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ViewWidget");

            p.media = media;
            p.image = Observer::ValueSubject<std::shared_ptr<Image::Image> >::create();
            p.imagePos = Observer::ValueSubject<glm::vec2>::create();
            p.imageZoom = Observer::ValueSubject<float>::create(1.F);

            p.gridOverlay = GridOverlay::create(context);

            p.hudOverlay = HUDOverlay::create(context);

            p.layout = UI::StackLayout::create(context);
            p.layout->addChild(p.gridOverlay);
            p.layout->addChild(p.hudOverlay);
            addChild(p.layout);

            _gridUpdate();
            _hudUpdate();

            auto avSystem = context->getSystemT<AV::AVSystem>();
            auto weak = std::weak_ptr<ViewWidget>(std::dynamic_pointer_cast<ViewWidget>(shared_from_this()));
            p.timeUnitsObserver = Observer::ValueObserver<AV::Time::Units>::create(
                avSystem->observeTimeUnits(),
                [weak](AV::Time::Units value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->timeUnits = value;
                        widget->_hudUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.lockObserver = Observer::ValueObserver<ViewLock>::create(
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
            p.gridOptionsObserver = Observer::ValueObserver<GridOptions>::create(
                viewSettings->observeGridOptions(),
                [weak](const GridOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->gridOptions = value;
                        widget->_gridUpdate();
                    }
                });

            p.hudOptionsObserver = Observer::ValueObserver<HUDOptions>::create(
                viewSettings->observeHUDOptions(),
                [weak](const HUDOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions = value;
                        widget->_gridUpdate();
                    }
                });
            p.backgroundOptionsObserver = Observer::ValueObserver<ViewBackgroundOptions>::create(
                viewSettings->observeBackgroundOptions(),
                [weak](const ViewBackgroundOptions& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions = value;
                        widget->_redraw();
                    }
                });

            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            p.imageDataObserver = Observer::ValueObserver<ImageData>::create(
                imageSettings->observeData(),
                [weak](const ImageData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageData = value;
                        widget->_redraw();
                    }
                });

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.ocioConfigObserver = Observer::ValueObserver<OCIO::Config>::create(
                ocioSystem->observeCurrentConfig(),
                [weak, contextWeak](const OCIO::Config& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                            widget->_p->ocioConfig = value;
                            widget->_p->outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                            widget->_redraw();
                        }
                    }
                });

            p.layersObserver = Observer::ValueObserver<std::pair<std::vector<Image::Info>, int> >::create(
                p.media->observeLayers(),
                [weak](const std::pair<std::vector<Image::Info>, int>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_hudUpdate();
                    }
                });
            p.speedObserver = Observer::ValueObserver<Math::Rational>::create(
                p.media->observeSpeed(),
                [weak](const Math::Rational& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->speed = value;
                        widget->_hudUpdate();
                    }
                });
            p.realSpeedObserver = Observer::ValueObserver<float>::create(
                p.media->observeRealSpeed(),
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->realSpeed = value;
                        widget->_hudUpdate();
                    }
                });
            p.sequenceObserver = Observer::ValueObserver<Math::Frame::Sequence>::create(
                p.media->observeSequence(),
                [weak](const Math::Frame::Sequence& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->sequence = value;
                        widget->_hudUpdate();
                    }
                });
            p.currentFrameObserver = Observer::ValueObserver<Math::Frame::Index>::create(
                p.media->observeCurrentFrame(),
                [weak](Math::Frame::Index value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->currentFrame = value;
                        widget->_hudUpdate();
                    }
                });
            p.annotationsObserver = Observer::ListObserver<std::shared_ptr<AnnotatePrimitive> >::create(
                p.media->observeAnnotations(),
                [weak](const std::vector<std::shared_ptr<AnnotatePrimitive> >& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->annotations = value;
                        widget->_redraw();
                    }
                });

            p.zoomAnimation = System::Animation::Animation::create(context);
            p.zoomAnimation->setType(System::Animation::Type::SmoothStep);
        }

        ViewWidget::ViewWidget() :
            _p(new Private)
        {}

        ViewWidget::~ViewWidget()
        {}

        std::shared_ptr<ViewWidget> ViewWidget::create(
            const std::shared_ptr<Media>& media,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ViewWidget>(new ViewWidget);
            out->_init(media, context);
            return out;
        }

        std::shared_ptr<Observer::IValueSubject<std::shared_ptr<Image::Image> > > ViewWidget::observeImage() const
        {
            return _p->image;
        }

        void ViewWidget::setImage(const std::shared_ptr<Image::Image>& value)
        {
            DJV_PRIVATE_PTR();
            if (p.image->setIfChanged(value))
            {
                _gridUpdate();
            }
        }

        std::shared_ptr<Observer::IValueSubject<glm::vec2> > ViewWidget::observeImagePos() const
        {
            return _p->imagePos;
        }

        std::shared_ptr<Observer::IValueSubject<float> > ViewWidget::observeImageZoom() const
        {
            return _p->imageZoom;
        }

        Math::BBox2f ViewWidget::getImageBBox() const
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
            const Math::BBox2f& g = getGeometry();
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

        void ViewWidget::imageFrame(bool animate)
        {
            DJV_PRIVATE_PTR();
            if (p.image->get())
            {
                const Math::BBox2f& g = getGeometry();
                const auto pts = _getImagePoints();
                const glm::vec2 c = _getCenter(pts);
                const Math::BBox2f bbox = _getBBox(pts);
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
                const Math::BBox2f& g = getGeometry();
                const glm::vec2 c = _getCenter(_getImagePoints());
                setImagePosAndZoom(
                    glm::vec2(
                        g.w() / 2.F - c.x,
                        g.h() / 2.F - c.y),
                    1.F,
                    animate);
            }
        }

        void ViewWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            const float sa = style->getMetric(UI::MetricsRole::ScrollArea);
            glm::vec2 size = _p->layout->getMinimumSize();
            size.x = std::max(size.x, sa);
            size.y = std::max(size.x, sa);
            _setMinimumSize(size);
        }

        void ViewWidget::_layoutEvent(System::Event::Layout&)
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
            const Math::BBox2f& g = getGeometry();
            p.layout->setGeometry(g);
            p.gridOverlay->setImageBBox(getImageBBox());
        }

        void ViewWidget::_paintEvent(System::Event::Paint &)
        {
            DJV_PRIVATE_PTR();

            const auto& style = _getStyle();
            const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);

            // Draw the background.
            const auto& backgroundOptions = p.backgroundOptions;
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
                const Math::BBox2f bbox = _getBBox(points);
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
                m *= UI::ImageWidget::getXForm(image, p.imageData.rotate, glm::vec2(zoom, zoom), p.imageData.aspectRatio);
                render->pushTransform(m);
                render->setFillColor(Image::Color(1.F, 1.F, 1.F));
                Render2D::ImageOptions options;
                options.channelDisplay = p.imageData.channelDisplay;
                options.alphaBlend = p.imageData.alphaBlend;
                options.mirror = p.imageData.mirror;
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
                options.colorEnabled = p.imageData.colorEnabled;
                options.color = p.imageData.color;
                options.levelsEnabled = p.imageData.levelsEnabled;
                options.levels = p.imageData.levels;
                options.exposureEnabled = p.imageData.exposureEnabled;
                options.exposure = p.imageData.exposure;
                options.softClipEnabled = p.imageData.softClipEnabled;
                options.softClip = p.imageData.softClip;
                options.cache = Render2D::ImageCache::Dynamic;
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
                const Image::Size& size = image->getSize();
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
                    const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);
                    m = glm::translate(m, g.min + p.imagePos->get());
                }
                const float zoom = p.imageZoom->get();
                m *= UI::ImageWidget::getXForm(
                    image,
                    p.imageData.rotate,
                    posAndZoom ? glm::vec2(zoom, zoom) : glm::vec2(1.F, 1.F),
                    p.imageData.aspectRatio);
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
        
        Math::BBox2f ViewWidget::_getBBox(const std::vector<glm::vec3>& value)
        {
            Math::BBox2f out(0.F, 0.F, 0.F, 0.F);
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

        void ViewWidget::_gridUpdate()
        {
            DJV_PRIVATE_PTR();
            p.gridOverlay->setOptions(p.gridOptions);
            p.gridOverlay->setImagePosAndZoom(p.imagePos->get(), p.imageZoom->get());
            p.gridOverlay->setImageRotate(p.imageData.rotate);
            auto image = p.image->get();
            p.gridOverlay->setImageAspectRatio(
                p.imageData.aspectRatio,
                image ? image->getAspectRatio() : 1.F,
                image ? image->getInfo().pixelAspectRatio : 1.F);
            p.gridOverlay->setImageBBox(getImageBBox());
            _resize();
        }

        void ViewWidget::_hudUpdate()
        {
            DJV_PRIVATE_PTR();
            p.hudOverlay->setHUDOptions(p.hudOptions);
            HUDData data;
            data.fileName = p.media->getFileInfo().getFileName(Math::Frame::invalid, false);
            const auto& layers = p.media->observeLayers()->get();
            if (layers.second >= 0 && layers.second < static_cast<int>(layers.first.size()))
            {
                const auto& layer = layers.first[layers.second];
                data.layer = layer.name;
                data.size = layer.size;
                data.type = layer.type;
            }
            data.isSequence = p.sequence.getFrameCount() > 1;
            data.currentFrame = AV::Time::toString(p.sequence.getFrame(p.currentFrame), p.speed, p.timeUnits);
            data.speed = p.speed;
            data.realSpeed = p.realSpeed;
            p.hudOverlay->setHUDData(data);
        }

    } // namespace ViewApp
} // namespace djv


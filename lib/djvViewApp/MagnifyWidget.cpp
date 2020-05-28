// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MagnifyWidget.h>

#include <djvViewApp/ImageView.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>

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
        namespace
        {
            class ImageWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(ImageWidget);

            protected:
                void _init(const std::shared_ptr<Context>&);
                ImageWidget();

            public:
                virtual ~ImageWidget();

                static std::shared_ptr<ImageWidget> create(const std::shared_ptr<Context>&);

                void setImage(const std::shared_ptr<AV::Image::Image>&);
                void setImageOptions(const AV::Render2D::ImageOptions&);
                void setImagePos(const glm::vec2&);
                void setImageZoom(float);
                void setImageRotate(UI::ImageRotate);
                void setImageAspectRatio(UI::ImageAspectRatio);
                void setBackgroundColor(const AV::Image::Color&);
                void setMagnify(int);
                void setMagnifyPos(const glm::vec2&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _paintEvent(Event::Paint&) override;

            private:
                std::shared_ptr<AV::Image::Image> _image;
                AV::Render2D::ImageOptions _imageOptions;
                glm::vec2 _imagePos = glm::vec2(0.F, 0.F);
                float _imageZoom = 0.F;
                UI::ImageRotate _imageRotate = UI::ImageRotate::First;
                UI::ImageAspectRatio _imageAspectRatio = UI::ImageAspectRatio::First;
                AV::OCIO::Config _ocioConfig;
                std::string _outputColorSpace;
                AV::Image::Color _backgroundColor;
                int _magnify = 1;
                glm::vec2 _magnifyPos = glm::vec2(0.F, 0.F);
                std::shared_ptr<ValueObserver<AV::OCIO::Config> > _ocioConfigObserver;
            };

            void ImageWidget::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                auto weak = std::weak_ptr<ImageWidget>(std::dynamic_pointer_cast<ImageWidget>(shared_from_this()));
                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                auto contextWeak = std::weak_ptr<Context>(context);
                _ocioConfigObserver = ValueObserver<AV::OCIO::Config>::create(
                    ocioSystem->observeCurrentConfig(),
                    [weak, contextWeak](const AV::OCIO::Config& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                                widget->_ocioConfig = value;
                                widget->_outputColorSpace = ocioSystem->getColorSpace(value.display, value.view);
                                widget->_redraw();
                            }
                        }
                    });
            }

            ImageWidget::ImageWidget()
            {}

            ImageWidget::~ImageWidget()
            {}

            std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
                out->_init(context);
                return out;
            }

            void ImageWidget::setImage(const std::shared_ptr<AV::Image::Image>& value)
            {
                if (value == _image)
                    return;
                _image = value;
                _redraw();
            }

            void ImageWidget::setImageOptions(const AV::Render2D::ImageOptions& value)
            {
                if (value == _imageOptions)
                    return;
                _imageOptions = value;
                _redraw();
            }

            void ImageWidget::setImagePos(const glm::vec2& value)
            {
                if (value == _imagePos)
                    return;
                _imagePos = value;
                _redraw();
            }

            void ImageWidget::setImageZoom(float value)
            {
                if (value == _imageZoom)
                    return;
                _imageZoom = value;
                _redraw();
            }

            void ImageWidget::setImageRotate(UI::ImageRotate value)
            {
                if (value == _imageRotate)
                    return;
                _imageRotate = value;
                _redraw();
            }

            void ImageWidget::setImageAspectRatio(UI::ImageAspectRatio value)
            {
                if (value == _imageAspectRatio)
                    return;
                _imageAspectRatio = value;
                _redraw();
            }

            void ImageWidget::setBackgroundColor(const AV::Image::Color& value)
            {
                if (value == _backgroundColor)
                    return;
                _backgroundColor = value;
                _redraw();
            }

            void ImageWidget::setMagnify(int value)
            {
                if (value == _magnify)
                    return;
                _magnify = value;
                _redraw();
            }

            void ImageWidget::setMagnifyPos(const glm::vec2& value)
            {
                if (value == _magnifyPos)
                    return;
                _magnifyPos = value;
                _redraw();
            }

            void ImageWidget::_preLayoutEvent(Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float sw = style->getMetric(UI::MetricsRole::Swatch);
                _setMinimumSize(glm::vec2(sw, sw));
            }

            void ImageWidget::_paintEvent(Event::Paint&)
            {
                const auto& style = _getStyle();
                const BBox2f& g = getMargin().bbox(getGeometry(), style);
                const auto& render = _getRender();
                render->setFillColor(_backgroundColor);
                render->drawRect(g);

                if (_image)
                {
                    render->setFillColor(AV::Image::Color(1.F, 1.F, 1.F));

                    const float magnify = powf(_magnify, 2.F);
                    glm::mat3x3 m(1.F);
                    m = glm::translate(m, glm::vec2(g.w() / 2.F, g.h() / 2.F) - glm::vec2(_magnifyPos.x * magnify, _magnifyPos.y * magnify));
                    m = glm::translate(m, g.min + glm::vec2(_imagePos.x * magnify, _imagePos.y * magnify));
                    m *= UI::ImageWidget::getXForm(_image, _imageRotate, glm::vec2(_imageZoom * magnify, _imageZoom * magnify), _imageAspectRatio);
                    render->pushTransform(m);
                    AV::Render2D::ImageOptions options(_imageOptions);
                    auto i = _ocioConfig.fileColorSpaces.find(_image->getPluginName());
                    if (i != _ocioConfig.fileColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                    else
                    {
                        i = _ocioConfig.fileColorSpaces.find(std::string());
                        if (i != _ocioConfig.fileColorSpaces.end())
                        {
                            options.colorSpace.input = i->second;
                        }
                    }
                    options.colorSpace.output = _outputColorSpace;
                    options.cache = AV::Render2D::ImageCache::Dynamic;
                    render->drawImage(_image, glm::vec2(0.F, 0.F), options);
                    render->popTransform();
                }
            }

        } // namespace

        struct MagnifyWidget::Private
        {
            bool currentTool = false;
            size_t magnify = 1;
            glm::vec2 magnifyPos = glm::vec2(0.F, 0.F);
            std::shared_ptr<MediaWidget> activeWidget;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<ImageWidget> imageWidget;
            std::shared_ptr<UI::IntSlider> magnifySlider;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<AV::Image::Image> > > imageObserver;
            std::shared_ptr<ValueObserver<AV::Render2D::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > imagePosObserver;
            std::shared_ptr<ValueObserver<float> > imageZoomObserver;
            std::shared_ptr<ValueObserver<UI::ImageRotate> > imageRotateObserver;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > imageAspectRatioObserver;
            std::shared_ptr<ValueObserver<AV::Image::Color> > backgroundColorObserver;
            std::shared_ptr<ValueObserver<PointerData> > dragObserver;
        };

        void MagnifyWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MagnifyWidget");

            p.imageWidget = ImageWidget::create(context);
            p.imageWidget->setShadowOverlay({ UI::Side::Top });
            
            p.magnifySlider = UI::IntSlider::create(context);
            p.magnifySlider->setRange(IntRange(1, 10));

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->setBackgroundRole(UI::ColorRole::Background);
            layout->addChild(p.imageWidget);
            layout->setStretch(p.imageWidget, UI::RowStretch::Expand);
            layout->addChild(p.magnifySlider);
            addChild(layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<MagnifyWidget>(std::dynamic_pointer_cast<MagnifyWidget>(shared_from_this()));
            p.magnifySlider->setValueCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->magnify = value;
                        widget->_p->imageWidget->setMagnify(value);
                        widget->_redraw();
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->imageObserver = ValueObserver<std::shared_ptr<AV::Image::Image> >::create(
                                    widget->_p->activeWidget->getMedia()->observeCurrentImage(),
                                    [weak](const std::shared_ptr<AV::Image::Image>& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImage(value);
                                        }
                                    });

                                widget->_p->imageOptionsObserver = ValueObserver<AV::Render2D::ImageOptions>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageOptions(),
                                    [weak](const AV::Render2D::ImageOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImageOptions(value);
                                        }
                                    });

                                widget->_p->imagePosObserver = ValueObserver<glm::vec2>::create(
                                    widget->_p->activeWidget->getImageView()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImagePos(value);
                                        }
                                    });

                                widget->_p->imageZoomObserver = ValueObserver<float>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImageZoom(value);
                                        }
                                    });

                                widget->_p->imageRotateObserver = ValueObserver<UI::ImageRotate>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageRotate(),
                                    [weak](UI::ImageRotate value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImageRotate(value);
                                        }
                                    });

                                widget->_p->imageAspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                                    widget->_p->activeWidget->getImageView()->observeImageAspectRatio(),
                                    [weak](UI::ImageAspectRatio value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setImageAspectRatio(value);
                                        }
                                    });

                                widget->_p->backgroundColorObserver = ValueObserver<AV::Image::Color>::create(
                                    widget->_p->activeWidget->getImageView()->observeBackgroundColor(),
                                    [weak](const AV::Image::Color& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageWidget->setBackgroundColor(value);
                                        }
                                    });

                                widget->_p->dragObserver = ValueObserver<PointerData>::create(
                                    widget->_p->activeWidget->observeDrag(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            if (widget->_p->currentTool)
                                            {
                                                widget->_p->magnifyPos = value.pos;
                                                widget->_p->imageWidget->setMagnifyPos(value.pos);
                                            }
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageObserver.reset();
                                widget->_p->imageOptionsObserver.reset();
                                widget->_p->imagePosObserver.reset();
                                widget->_p->imageZoomObserver.reset();
                                widget->_p->imageRotateObserver.reset();
                                widget->_p->imageAspectRatioObserver.reset();
                                widget->_p->backgroundColorObserver.reset();
                                widget->_p->dragObserver.reset();
                            }
                        }
                    });
            }
        }

        MagnifyWidget::MagnifyWidget() :
            _p(new Private)
        {}

        MagnifyWidget::~MagnifyWidget()
        {}

        std::shared_ptr<MagnifyWidget> MagnifyWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MagnifyWidget>(new MagnifyWidget);
            out->_init(context);
            return out;
        }

        void MagnifyWidget::setCurrentTool(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.currentTool)
                return;
            p.currentTool = value;
            _widgetUpdate();
            _redraw();
        }
        size_t MagnifyWidget::getMagnify() const
        {
            return _p->magnify;
        }

        void MagnifyWidget::setMagnify(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.magnify)
                return;
            p.magnify = value;
            _widgetUpdate();
            _redraw();
        }

        const glm::vec2& MagnifyWidget::getMagnifyPos() const
        {
            return _p->magnifyPos;
        }

        void MagnifyWidget::setMagnifyPos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.magnifyPos)
                return;
            p.magnifyPos = value;
            _widgetUpdate();
            _redraw();
        }

        void MagnifyWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();

            setTitle(_getText(DJV_TEXT("widget_magnify_title")));

            p.magnifySlider->setTooltip(_getText(DJV_TEXT("widget_magnify_slider_tooltip")));
        }

        void MagnifyWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.imageWidget->setMagnify(p.magnify);
            p.imageWidget->setMagnifyPos(p.magnifyPos);
            p.magnifySlider->setValue(p.magnify);
        }

    } // namespace ViewApp
} // namespace djv


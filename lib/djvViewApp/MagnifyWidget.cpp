// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MagnifyWidget.h>

#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/MagnifySettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewData.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Action.h>
#include <djvUI/DrawUtil.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvRender2D/Render.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvImage/Image.h>

#include <djvSystem/Context.h>

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
                void _init(const std::shared_ptr<System::Context>&);
                ImageWidget();

            public:
                ~ImageWidget() override;

                static std::shared_ptr<ImageWidget> create(const std::shared_ptr<System::Context>&);

                void setCurrentTool(bool);

                const glm::vec2& getMagnifyPos() const;

                void setMagnifyPos(const glm::vec2&);

            protected:
                void _preLayoutEvent(System::Event::PreLayout&) override;
                void _paintEvent(System::Event::Paint&) override;

            private:
                bool _currentTool = false;
                std::shared_ptr<Image::Image> _image;
                glm::vec2 _imagePos = glm::vec2(0.F, 0.F);
                float _imageZoom = 0.F;
                ImageData _imageData;
                OCIO::Config _ocioConfig;
                std::string _outputColorSpace;
                ViewBackgroundOptions _backgroundOptions;
                int _magnify = 1;
                glm::vec2 _magnifyPos = glm::vec2(0.F, 0.F);

                std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > _activeWidgetObserver;
                std::shared_ptr<Observer::Value<std::shared_ptr<Image::Image> > > _imageObserver;
                std::shared_ptr<Observer::Value<glm::vec2> > _imagePosObserver;
                std::shared_ptr<Observer::Value<float> > _imageZoomObserver;
                std::shared_ptr<Observer::Value<PointerData> > _dragObserver;
                std::shared_ptr<Observer::Value<size_t> > _magnifyObserver;
                std::shared_ptr<Observer::Value<ViewBackgroundOptions> > _backgroundOptionsObserver;
                std::shared_ptr<Observer::Value<ImageData> > _imageDataObserver;
                std::shared_ptr<Observer::Value<OCIO::Config> > _ocioConfigObserver;
            };

            void ImageWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                auto weak = std::weak_ptr<ImageWidget>(std::dynamic_pointer_cast<ImageWidget>(shared_from_this()));
                if (auto windowSystem = context->getSystemT<WindowSystem>())
                {
                    _activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                        windowSystem->observeActiveWidget(),
                        [weak](const std::shared_ptr<MediaWidget>& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                if (value)
                                {
                                    widget->_imageObserver = Observer::Value<std::shared_ptr<Image::Image> >::create(
                                        value->getMedia()->observeCurrentImage(),
                                        [weak](const std::shared_ptr<Image::Image>& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_image = value;
                                                widget->_redraw();
                                            }
                                        });

                                    widget->_imagePosObserver = Observer::Value<glm::vec2>::create(
                                        value->getViewWidget()->observeImagePos(),
                                        [weak](const glm::vec2& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_imagePos = value;
                                                widget->_redraw();
                                            }
                                        });

                                    widget->_imageZoomObserver = Observer::Value<float>::create(
                                        value->getViewWidget()->observeImageZoom(),
                                        [weak](float value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                widget->_imageZoom = value;
                                                widget->_redraw();
                                            }
                                        });

                                    widget->_dragObserver = Observer::Value<PointerData>::create(
                                        value->observeDrag(),
                                        [weak](const PointerData& value)
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                if (widget->_currentTool)
                                                {
                                                    widget->_magnifyPos = value.pos;
                                                }
                                            }
                                        });
                                }
                                else
                                {
                                    widget->_imageObserver.reset();
                                    widget->_imagePosObserver.reset();
                                    widget->_imageZoomObserver.reset();
                                    widget->_dragObserver.reset();
                                }
                            }
                        });
                }
            
                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                auto settings = settingsSystem->getSettingsT<MagnifySettings>();
                _magnifyObserver = Observer::Value<size_t>::create(
                    settings->observeMagnify(),
                    [weak](const size_t& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_magnify = value;
                            widget->_redraw();
                        }
                    });

                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                _backgroundOptionsObserver = Observer::Value<ViewBackgroundOptions>::create(
                    viewSettings->observeBackgroundOptions(),
                    [weak](const ViewBackgroundOptions& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_backgroundOptions = value;
                            widget->_redraw();
                        }
                    });

                auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
                _imageDataObserver = Observer::Value<ImageData>::create(
                    imageSettings->observeData(),
                    [weak](const ImageData& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_imageData = value;
                            widget->_redraw();
                        }
                    });

                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                auto contextWeak = std::weak_ptr<System::Context>(context);
                _ocioConfigObserver = Observer::Value<OCIO::Config>::create(
                    ocioSystem->observeCurrentConfig(),
                    [weak, contextWeak](const OCIO::Config& value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
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

            std::shared_ptr<ImageWidget> ImageWidget::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<ImageWidget>(new ImageWidget);
                out->_init(context);
                return out;
            }

            void ImageWidget::setCurrentTool(bool value)
            {
                _currentTool = value;
            }

            const glm::vec2& ImageWidget::getMagnifyPos() const
            {
                return _magnifyPos;
            }

            void ImageWidget::setMagnifyPos(const glm::vec2& value)
            {
                if (value == _magnifyPos)
                    return;
                _magnifyPos = value;
                _redraw();
            }

            void ImageWidget::_preLayoutEvent(System::Event::PreLayout&)
            {
                const auto& style = _getStyle();
                const float sw = style->getMetric(UI::MetricsRole::Swatch);
                _setMinimumSize(glm::vec2(sw, sw));
            }

            void ImageWidget::_paintEvent(System::Event::Paint&)
            {
                const auto& style = _getStyle();
                const Math::BBox2f& g = getMargin().bbox(getGeometry(), style);

                const auto& render = _getRender();
                switch (_backgroundOptions.background)
                {
                case ViewBackground::Solid:
                    render->setFillColor(_backgroundOptions.color);
                    render->drawRect(g);
                    break;
                case ViewBackground::Checkers:
                    UI::drawCheckers(
                        render,
                        g,
                        _backgroundOptions.checkersSize,
                        _backgroundOptions.checkersColors[0],
                        _backgroundOptions.checkersColors[1]);
                    break;
                default: break;
                }

                if (_image)
                {
                    render->setFillColor(Image::Color(1.F, 1.F, 1.F));

                    const float magnify = powf(_magnify, 2.F);
                    glm::mat3x3 m(1.F);
                    m = glm::translate(m, glm::vec2(g.w() / 2.F, g.h() / 2.F) - glm::vec2(_magnifyPos.x * magnify, _magnifyPos.y * magnify));
                    m = glm::translate(m, g.min + glm::vec2(_imagePos.x * magnify, _imagePos.y * magnify));
                    m *= UI::ImageWidget::getXForm(_image, _imageData.rotate, glm::vec2(_imageZoom * magnify, _imageZoom * magnify), _imageData.aspectRatio);
                    render->pushTransform(m);
                    Render2D::ImageOptions options;
                    options.channelDisplay = _imageData.channelDisplay;
                    options.alphaBlend = _imageData.alphaBlend;
                    options.mirror = _imageData.mirror;
                    auto i = _ocioConfig.imageColorSpaces.find(_image->getPluginName());
                    if (i != _ocioConfig.imageColorSpaces.end())
                    {
                        options.colorSpace.input = i->second;
                    }
                    else
                    {
                        i = _ocioConfig.imageColorSpaces.find(std::string());
                        if (i != _ocioConfig.imageColorSpaces.end())
                        {
                            options.colorSpace.input = i->second;
                        }
                    }
                    options.colorSpace.output = _outputColorSpace;
                    options.colorEnabled = _imageData.colorEnabled;
                    options.color = _imageData.color;
                    options.levelsEnabled = _imageData.levelsEnabled;
                    options.levels = _imageData.levels;
                    options.exposureEnabled = _imageData.exposureEnabled;
                    options.exposure = _imageData.exposure;
                    options.softClipEnabled = _imageData.softClipEnabled;
                    options.softClip = _imageData.softClip;
                    options.cache = Render2D::ImageCache::Dynamic;
                    render->drawImage(_image, glm::vec2(0.F, 0.F), options);
                    render->popTransform();
                }
            }

        } // namespace

        struct MagnifyWidget::Private
        {
            size_t magnify = 1;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<ImageWidget> imageWidget;
            std::shared_ptr<UI::Numeric::IntSlider> magnifySlider;

            std::shared_ptr<Observer::Value<size_t> > magnifyObserver;
        };

        void MagnifyWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MagnifyWidget");

            p.imageWidget = ImageWidget::create(context);
            p.imageWidget->setShadowOverlay({ UI::Side::Top });
            
            p.magnifySlider = UI::Numeric::IntSlider::create(context);
            p.magnifySlider->setRange(Math::IntRange(1, 10));

            auto layout = UI::VerticalLayout::create(context);
            layout->setSpacing(UI::MetricsRole::None);
            layout->setBackgroundRole(UI::ColorRole::Background);
            layout->addChild(p.imageWidget);
            layout->setStretch(p.imageWidget, UI::RowStretch::Expand);
            layout->addChild(p.magnifySlider);
            addChild(layout);

            _widgetUpdate();

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.magnifySlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto settings = settingsSystem->getSettingsT<MagnifySettings>();
                        settings->setMagnify(value);
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto settings = settingsSystem->getSettingsT<MagnifySettings>();
            auto weak = std::weak_ptr<MagnifyWidget>(std::dynamic_pointer_cast<MagnifyWidget>(shared_from_this()));
            p.magnifyObserver = Observer::Value<size_t>::create(
                settings->observeMagnify(),
                [weak](size_t value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->magnify = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        MagnifyWidget::MagnifyWidget() :
            _p(new Private)
        {}

        MagnifyWidget::~MagnifyWidget()
        {}

        std::shared_ptr<MagnifyWidget> MagnifyWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<MagnifyWidget>(new MagnifyWidget);
            out->_init(context);
            return out;
        }

        void MagnifyWidget::setCurrentTool(bool value)
        {
            _p->imageWidget->setCurrentTool(value);
        }

        const glm::vec2& MagnifyWidget::getMagnifyPos() const
        {
            return _p->imageWidget->getMagnifyPos();
        }

        void MagnifyWidget::setMagnifyPos(const glm::vec2& value)
        {
            _p->imageWidget->setMagnifyPos(value);
        }

        void MagnifyWidget::_initEvent(System::Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_magnify_title")));

                p.magnifySlider->setTooltip(_getText(DJV_TEXT("widget_magnify_slider_tooltip")));
            }
        }

        void MagnifyWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.magnifySlider->setValue(p.magnify);
        }

    } // namespace ViewApp
} // namespace djv


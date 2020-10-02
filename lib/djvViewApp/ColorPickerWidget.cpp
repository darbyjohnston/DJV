// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorPickerWidget.h>

#include <djvViewApp/ColorPickerSettings.h>
#include <djvViewApp/ImageData.h>
#include <djvViewApp/ImageSettings.h>
#include <djvViewApp/Media.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/ColorSwatch.h>
#include <djvUI/EventSystem.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ImageWidget.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

#include <djvOCIO/OCIOSystem.h>

#include <djvGL/OffscreenBuffer.h>
#if defined(DJV_GL_ES2)
#include <djvGL/Mesh.h>
#include <djvGL/Shader.h>
#endif // DJV_GL_ES2

#include <djvImage/ColorFunc.h>
#include <djvImage/ImageFunc.h>

#include <djvSystem/Context.h>
#if defined(DJV_GL_ES2)
#include <djvSystem/ResourceSystem.h>
#endif // DJV_GL_ES2

#include <djvCore/StringFunc.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#include <iomanip>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t sampleSizeMax = 100;

            //! \todo What is this really?
            const size_t bufferSizeMin = 100;
        
        } // namespace

        struct ColorPickerWidget::Private
        {
            bool currentTool = false;
            Image::Color color = Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<Image::Image> image;
            glm::vec2 imagePos = glm::vec2(0.F, 0.F);
            float imageZoom = 1.F;
            glm::vec2 pickerPos = glm::vec2(0.F, 0.F);
            glm::vec2 pixelPos = glm::vec2(0.F, 0.F);
            ColorPickerData data;
            ImageData imageData;
            OCIO::Config ocioConfig;
            std::string outputColorSpace;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ColorSwatch> colorSwatch;
            std::shared_ptr<UI::Text::Label> colorLabel;
            std::shared_ptr<UI::Text::Label> pixelLabel;
            std::shared_ptr<UI::Numeric::IntSlider> sampleSizeSlider;
            std::shared_ptr<UI::ColorTypeWidget> typeWidget;
            std::shared_ptr<UI::ToolButton> copyButton;
            std::shared_ptr<UI::Menu> settingsMenu;
            std::shared_ptr<UI::PopupMenu> settingsPopupMenu;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<GL::OffscreenBuffer> offscreenBuffer;
#if defined(DJV_GL_ES2)
            std::shared_ptr<GL::Shader> shader;
#endif // DJV_GL_ES2

            std::shared_ptr<Observer::Value<ColorPickerData> > dataObserver;
            std::shared_ptr<Observer::Value<ImageData> > imageDataObserver;
            std::shared_ptr<Observer::Value<OCIO::Config> > ocioConfigObserver;
            std::map<std::string, std::shared_ptr<Observer::Value<bool> > > actionObservers;
            std::shared_ptr<Observer::Value<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<Observer::Value<std::shared_ptr<Image::Image> > > imageObserver;
            std::shared_ptr<Observer::Value<glm::vec2> > imagePosObserver;
            std::shared_ptr<Observer::Value<float> > imageZoomObserver;
            std::shared_ptr<Observer::Value<PointerData> > dragObserver;
        };

        void ColorPickerWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ColorPickerWidget");
            
            p.actions["LockType"] = UI::Action::create();
            p.actions["LockType"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ApplyColorOperations"] = UI::Action::create();
            p.actions["ApplyColorOperations"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ApplyColorSpace"] = UI::Action::create();
            p.actions["ApplyColorSpace"]->setButtonType(UI::ButtonType::Toggle);

            p.colorSwatch = UI::ColorSwatch::create(context);
            p.colorSwatch->setBorder(false);
            p.colorSwatch->setHAlign(UI::HAlign::Fill);

            p.colorLabel = UI::Text::Label::create(context);
            p.colorLabel->setFontFamily(Render2D::Font::familyMono);
            p.colorLabel->setTextHAlign(UI::TextHAlign::Left);
            p.colorLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.pixelLabel = UI::Text::Label::create(context);
            p.pixelLabel->setFontFamily(Render2D::Font::familyMono);
            p.pixelLabel->setTextHAlign(UI::TextHAlign::Left);
            p.pixelLabel->setMargin(UI::MetricsRole::MarginSmall);

            p.sampleSizeSlider = UI::Numeric::IntSlider::create(context);
            p.sampleSizeSlider->setRange(Math::IntRange(1, sampleSizeMax));

            p.typeWidget = UI::ColorTypeWidget::create(context);

            p.copyButton = UI::ToolButton::create(context);
            p.copyButton->setIcon("djvIconShare");

            p.settingsMenu = UI::Menu::create(context);
            p.settingsMenu->setIcon("djvIconSettings");
            p.settingsMenu->setMinimumSizeRole(UI::MetricsRole::None);
            p.settingsMenu->addAction(p.actions["LockType"]);
            p.settingsMenu->addAction(p.actions["ApplyColorOperations"]);
            p.settingsMenu->addAction(p.actions["ApplyColorSpace"]);
            p.settingsPopupMenu = UI::PopupMenu::create(context);
            p.settingsPopupMenu->setMenu(p.settingsMenu);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->setShadowOverlay({ UI::Side::Top });
            p.layout->addChild(p.colorSwatch);
            p.layout->setStretch(p.colorSwatch, UI::RowStretch::Expand);
            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.colorLabel);
            p.formLayout->addChild(p.pixelLabel);
            p.formLayout->addChild(p.sampleSizeSlider);
            p.layout->addChild(p.formLayout);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.typeWidget);
            hLayout->addChild(p.copyButton);
            hLayout->addExpander();
            hLayout->addChild(p.settingsPopupMenu);
            p.layout->addChild(hLayout);
            addChild(p.layout);

#if defined(DJV_GL_ES2)
            auto resourceSystem = context->getSystemT<System::ResourceSystem>();
            const System::File::Path shaderPath = resourceSystem->getPath(System::File::ResourcePath::Shaders);
            p.shader = GL::Shader::create(
                System::File::Path(shaderPath, "djvRender2DVertex.glsl"),
                System::File::Path(shaderPath, "djvRender2DFragment.glsl"));
#endif // DJV_GL_ES2

            _sampleUpdate();
            _widgetUpdate();

            auto weak = std::weak_ptr<ColorPickerWidget>(std::dynamic_pointer_cast<ColorPickerWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.sampleSizeSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.sampleSize = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
                            settings->setData(data);
                        }
                    }
                });

            p.typeWidget->setTypeCallback(
                [weak, contextWeak](Image::Type value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->color = widget->_p->color.convert(value);
                            widget->_widgetUpdate();

                            auto data = widget->_p->data;
                            data.lockType = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
                            settings->setData(data);
                        }
                    }
                });

            p.copyButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto eventSystem = widget->_getEventSystem().lock())
                        {
                            std::stringstream ss;
                            ss << Image::getLabel(widget->_p->color) << ", ";
                            ss << floorf(widget->_p->pixelPos.x) << " ";
                            ss << floorf(widget->_p->pixelPos.y);
                            eventSystem->setClipboard(ss.str());
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
            p.dataObserver = Observer::Value<ColorPickerData>::create(
                settings->observeData(),
                [weak](const ColorPickerData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->data = value;
                        if (widget->_p->data.lockType != Image::Type::None)
                        {
                            widget->_p->color = widget->_p->color.convert(widget->_p->data.lockType);
                        }
                        widget->_sampleUpdate();
                        widget->_widgetUpdate();
                    }
                });

            auto imageSettings = settingsSystem->getSettingsT<ImageSettings>();
            p.imageDataObserver = Observer::Value<ImageData>::create(
                imageSettings->observeData(),
                [weak](const ImageData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->imageData = value;
                        widget->_sampleUpdate();
                        widget->_widgetUpdate();
                    }
                });

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            p.ocioConfigObserver = Observer::Value<OCIO::Config>::create(
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
                            widget->_sampleUpdate();
                            widget->_widgetUpdate();
                        }
                    }
                });

            p.actionObservers["LockType"] = Observer::Value<bool>::create(
                p.actions["LockType"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            if (value)
                            {
                                data.lockType = widget->_p->typeWidget->getType();
                            }
                            else
                            {
                                data.lockType = Image::Type::None;
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
                            settings->setData(data);
                        }
                    }
                });

            p.actionObservers["ApplyColorOperations"] = Observer::Value<bool>::create(
                p.actions["ApplyColorOperations"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.applyColorOperations = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
                            settings->setData(data);
                        }
                    }
                });

            p.actionObservers["ApplyColorSpace"] = Observer::Value<bool>::create(
                p.actions["ApplyColorSpace"]->observeChecked(),
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto data = widget->_p->data;
                            data.applyColorSpace = value;
                            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                            auto settings = settingsSystem->getSettingsT<ColorPickerSettings>();
                            settings->setData(data);
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = Observer::Value<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            if (value)
                            {
                                widget->_p->imageObserver = Observer::Value<std::shared_ptr<Image::Image> >::create(
                                    value->getViewWidget()->observeImage(),
                                    [weak](const std::shared_ptr<Image::Image>& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->image = value;
                                            widget->_sampleUpdate();
                                            widget->_widgetUpdate();
                                        }
                                    });

                                widget->_p->imagePosObserver = Observer::Value<glm::vec2>::create(
                                    value->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imagePos = value;
                                            widget->_sampleUpdate();
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->imageZoomObserver = Observer::Value<float>::create(
                                    value->getViewWidget()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->imageZoom = value;
                                            widget->_sampleUpdate();
                                            widget->_widgetUpdate();
                                        }
                                    });

                                widget->_p->dragObserver = Observer::Value<PointerData>::create(
                                    value->observeDrag(),
                                    [weak](const PointerData& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            if (widget->_p->currentTool)
                                            {
                                                widget->_p->pickerPos = value.pos;
                                                widget->_sampleUpdate();
                                                widget->_widgetUpdate();
                                            }
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageObserver.reset();
                                widget->_p->imagePosObserver.reset();
                                widget->_p->imageZoomObserver.reset();
                                widget->_p->dragObserver.reset();
                            }
                        }
                    });
            }
        }

        ColorPickerWidget::ColorPickerWidget() :
            _p(new Private)
        {}

        ColorPickerWidget::~ColorPickerWidget()
        {}

        std::shared_ptr<ColorPickerWidget> ColorPickerWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerWidget>(new ColorPickerWidget);
            out->_init(context);
            return out;
        }

        void ColorPickerWidget::setCurrentTool(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.currentTool)
                return;
            p.currentTool = value;
            _sampleUpdate();
            _widgetUpdate();
        }

        const glm::vec2& ColorPickerWidget::getPickerPos() const
        {
            return _p->pickerPos;
        }

        void ColorPickerWidget::setPickerPos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.pickerPos)
                return;
            p.pickerPos = value;
            _sampleUpdate();
            _widgetUpdate();
        }

        void ColorPickerWidget::_initEvent(System::Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_picker")));

                p.actions["LockType"]->setText(_getText(DJV_TEXT("widget_color_picker_lock_color_type")));
                p.actions["LockType"]->setTooltip(_getText(DJV_TEXT("widget_color_picker_lock_color_type_tooltip")));
                p.actions["ApplyColorOperations"]->setText(_getText(DJV_TEXT("widget_color_picker_apply_color_operations")));
                p.actions["ApplyColorOperations"]->setTooltip(_getText(DJV_TEXT("widget_color_picker_apply_color_operations_tooltip")));
                p.actions["ApplyColorSpace"]->setText(_getText(DJV_TEXT("widget_color_picker_apply_color_space")));
                p.actions["ApplyColorSpace"]->setTooltip(_getText(DJV_TEXT("widget_color_picker_apply_color_space_tooltip")));

                p.sampleSizeSlider->setTooltip(_getText(DJV_TEXT("widget_color_picker_sample_size_tooltip")));
                p.copyButton->setTooltip(_getText(DJV_TEXT("widget_color_picker_copy_tooltip")));
                p.settingsPopupMenu->setTooltip(_getText(DJV_TEXT("widget_color_picker_settings_tooltip")));

                p.formLayout->setText(p.colorLabel, _getText(DJV_TEXT("widget_color_picker_color")) + ":");
                p.formLayout->setText(p.pixelLabel, _getText(DJV_TEXT("widget_color_picker_pixel")) + ":");
                p.formLayout->setText(p.sampleSizeSlider, _getText(DJV_TEXT("widget_color_picker_sample_size")) + ":");
            }
        }
        
        void ColorPickerWidget::_sampleUpdate()
        {
            DJV_PRIVATE_PTR();
            glm::vec3 pixelPos(0.F, 0.F, 1.F);
            if (p.image && p.image->isValid())
            {
                try
                {
                    glm::mat3x3 m(1.F);
                    m = glm::translate(m, -(p.pickerPos / p.imageZoom));
                    const float z = p.data.sampleSize / 2.F;
                    m = glm::translate(m, glm::vec2(z, z));
                    m = glm::translate(m, p.imagePos / p.imageZoom);
                    m *= UI::ImageWidget::getXForm(
                        p.image,
                        p.imageData.rotate,
                        glm::vec2(1.F, 1.F),
                        p.imageData.aspectRatio);
                    pixelPos = glm::inverse(glm::translate(m, glm::vec2(-.5F, -.5F))) * pixelPos;

                    const size_t sampleSize = std::max(p.data.sampleSize, bufferSizeMin);
                    const Image::Size size(sampleSize, sampleSize);
                    const Image::Type type = p.data.lockType != Image::Type::None ? p.data.lockType : p.image->getType();
                    
                    bool create = !p.offscreenBuffer;
                    create |= p.offscreenBuffer && size != p.offscreenBuffer->getSize();
                    create |= p.offscreenBuffer && type != p.offscreenBuffer->getColorType();
                    if (create)
                    {
                        p.offscreenBuffer = GL::OffscreenBuffer::create(
                            size,
                            type,
                            _getTextSystem());
                    }

                    p.offscreenBuffer->bind();
                    const auto& render = _getRender();
                    const auto imageFilterOptions = render->getImageFilterOptions();
                    render->setImageFilterOptions(Render2D::ImageFilterOptions(Render2D::ImageFilter::Nearest));
                    render->beginFrame(size);
                    render->setFillColor(Image::Color(0.F, 0.F, 0.F));
                    render->drawRect(Math::BBox2f(0.F, 0.F, sampleSize, sampleSize));
                    render->setFillColor(Image::Color(1.F, 1.F, 1.F));
                    render->pushTransform(m);
                    Render2D::ImageOptions options;
                    options.channelDisplay = p.imageData.channelDisplay;
                    options.alphaBlend = p.imageData.alphaBlend;
                    options.mirror = p.imageData.mirror;
                    if (p.data.applyColorSpace)
                    {
                        auto i = p.ocioConfig.imageColorSpaces.find(p.image->getPluginName());
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
                    }
                    options.colorEnabled = p.imageData.colorEnabled;
                    options.color = p.imageData.color;
                    options.levelsEnabled = p.imageData.levelsEnabled;
                    options.levels = p.imageData.levels;
                    options.exposureEnabled = p.imageData.exposureEnabled;
                    options.exposure = p.imageData.exposure;
                    options.softClipEnabled = p.imageData.softClipEnabled;
                    options.softClip = p.imageData.softClip;
                    if (!p.data.applyColorOperations)
                    {
                        options.colorEnabled    = false;
                        options.levelsEnabled   = false;
                        options.exposureEnabled = false;
                        options.softClipEnabled = false;
                    }
                    options.cache = Render2D::ImageCache::Dynamic;
                    render->drawImage(p.image, glm::vec2(0.F, 0.F), options);
                    render->popTransform();
                    render->endFrame();
                    render->setImageFilterOptions(imageFilterOptions);
                    auto data = Image::Data::create(Image::Info(p.data.sampleSize, p.data.sampleSize, type));
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if !defined(DJV_GL_ES2)  // \todo GL_READ_FRAMEBUFFER, glClampColor not in OpenGL ES 2
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, p.offscreenBuffer->getID());
                    glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
#endif // DJV_GL_ES2
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);
                    glReadPixels(
                        0,
                        static_cast<int>(sampleSize) - static_cast<int>(data->getHeight()),
                        data->getWidth(),
                        data->getHeight(),
                        Image::getGLFormat(type),
                        Image::getGLType(type),
                        data->getData());
                    p.color = Image::getAverageColor(data);
                }
                catch (const std::exception& e)
                {
                    std::vector<std::string> messages;
                    messages.push_back(_getText(DJV_TEXT("error_cannot_sample_color")));
                    messages.push_back(e.what());
                    _log(String::join(messages, ' '), System::LogLevel::Error);
                }
            }
            else if (p.offscreenBuffer)
            {
                p.offscreenBuffer.reset();
            }
            switch (p.imageData.rotate)
            {
            /*case UI::ImageRotate::_90:
            {
                const float tmp = p.pixelPos.x;
                p.pixelPos.x = pixelPos.y;
                p.pixelPos.y = tmp;
                break;
            }*/
            default:
                p.pixelPos.x = pixelPos.x;
                p.pixelPos.y = pixelPos.y;
                break;
            }
        }

        void ColorPickerWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            const Image::Type type = p.color.getType();
            p.typeWidget->setType(type);

            const bool lockType = p.data.lockType != Image::Type::None;
            p.actions["LockType"]->setChecked(lockType);
            p.actions["ApplyColorOperations"]->setChecked(p.data.applyColorOperations);
            p.actions["ApplyColorSpace"]->setChecked(p.data.applyColorSpace);

            p.colorSwatch->setColor(p.color);
            p.colorLabel->setText(Image::getLabel(p.color, 2, false));
            p.colorLabel->setTooltip(_getText(DJV_TEXT("color_label_tooltip")));
            {
                std::stringstream ss;
                ss << static_cast<int>(floorf(p.pixelPos.x)) << " " << static_cast<int>(floorf(p.pixelPos.y));
                p.pixelLabel->setText(ss.str());
            }
            p.pixelLabel->setTooltip(_getText(DJV_TEXT("pixel_label_tooltip")));
            p.sampleSizeSlider->setValue(p.data.sampleSize);
        }

    } // namespace ViewApp
} // namespace djv


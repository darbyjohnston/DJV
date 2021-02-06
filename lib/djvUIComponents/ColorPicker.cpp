// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Border.h>
#include <djvUI/ComboBox.h>
#include <djvUI/ColorSwatch.h>
#include <djvUI/GridLayout.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntSlider.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/Label.h>
#include <djvUI/LayoutUtil.h>
#include <djvUI/Overlay.h>
#include <djvUI/PopupLayout.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvRender2D/Render.h>

#include <djvImage/Color.h>

#include <djvSystem/Context.h>

#include <djvMath/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        struct ColorTypeWidget::Private
        {
            Image::Type type = Image::Type::First;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::function<void(Image::Type)> typeCallback;
        };

        void ColorTypeWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::ColorTypeWidget");
            setVAlign(UI::VAlign::Center);

            p.comboBox = UI::ComboBox::create(context);
            p.comboBox->setBackgroundColorRole(UI::ColorRole::None);
            addChild(p.comboBox);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorTypeWidget>(std::dynamic_pointer_cast<ColorTypeWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    const auto type = static_cast<Image::Type>(value + 1);
                    widget->setType(type);
                    if (widget->_p->typeCallback)
                    {
                        widget->_p->typeCallback(type);
                    }
                }
            });
        }

        ColorTypeWidget::ColorTypeWidget() :
            _p(new Private)
        {}

        ColorTypeWidget::~ColorTypeWidget()
        {}

        std::shared_ptr<ColorTypeWidget> ColorTypeWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorTypeWidget>(new ColorTypeWidget);
            out->_init(context);
            return out;
        }

        Image::Type ColorTypeWidget::getType() const
        {
            return _p->type;
        }

        void ColorTypeWidget::setType(Image::Type value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.type)
                return;
            p.type = value;
            p.comboBox->setCurrentItem(static_cast<int>(p.type) - 1);
        }

        void ColorTypeWidget::setTypeCallback(const std::function<void(Image::Type)> & callback)
        {
            _p->typeCallback = callback;
        }

        void ColorTypeWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void ColorTypeWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void ColorTypeWidget::_initEvent(System::Event::Init & event)
        {
            if (event.getData().text)
            {
                setTooltip(_getText(DJV_TEXT("color_type_widget_tooltip")));
                _widgetUpdate();
            }
        }

        void ColorTypeWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (size_t i = static_cast<size_t>(Image::Type::L_U8); i < static_cast<size_t>(Image::Type::Count); ++i)
            {
                std::stringstream ss;
                ss << static_cast<Image::Type>(i);
                items.push_back(_getText(ss.str()));
            }
            p.comboBox->setItems(items);
            p.comboBox->setCurrentItem(static_cast<int>(p.type) - 1);
        }

        struct ColorSliders::Private
        {
            Image::Color color = Image::Color(0.F, 0.F, 0.F);
            bool hsv = false;
            std::vector<std::shared_ptr<UI::Text::Label> > intLabels;
            std::vector<std::shared_ptr<UI::Numeric::IntEdit> > intEdits;
            std::vector<std::shared_ptr<UI::Numeric::BasicIntSlider> > intSliders;
            std::vector<std::shared_ptr<UI::Text::Label> > floatLabels;
            std::vector<std::shared_ptr<UI::Numeric::FloatEdit> > floatEdits;
            std::vector<std::shared_ptr<UI::Numeric::BasicFloatSlider> > floatSliders;
            std::shared_ptr<UI::GridLayout> layout;
            std::function<void(const Image::Color &)> colorCallback;
            std::vector<std::shared_ptr<Observer::Value<int> > > intObservers;
            std::vector < std::shared_ptr<Observer::Value<float> > > floatObservers;
        };

        void ColorSliders::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::ColorSliders");

            p.layout = UI::GridLayout::create(context);
            p.layout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall, UI::MetricsRole::None));
            addChild(p.layout);

            _widgetUpdate();
            _textUpdate();
        }

        ColorSliders::ColorSliders() :
            _p(new Private)
        {}

        ColorSliders::~ColorSliders()
        {}

        std::shared_ptr<ColorSliders> ColorSliders::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorSliders>(new ColorSliders);
            out->_init(context);
            return out;
        }

        const Image::Color & ColorSliders::getColor() const
        {
            return _p->color;
        }

        void ColorSliders::setColor(const Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            const bool typeUpdate = value.getType() != p.color.getType();
            p.color = value;
            if (typeUpdate)
            {
                _widgetUpdate();
            }
            else
            {
                _colorUpdate();
            }
            _textUpdate();
        }

        void ColorSliders::setColorCallback(const std::function<void(const Image::Color &)> & callback)
        {
            _p->colorCallback = callback;
        }

        bool ColorSliders::hasHSV() const
        {
            return _p->hsv;
        }
        
        void ColorSliders::setHSV(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.hsv)
                return;
            p.hsv = value;
            _widgetUpdate();
            _textUpdate();
        }

        void ColorSliders::_preLayoutEvent(System::Event::PreLayout& event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorSliders::_layoutEvent(System::Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void ColorSliders::_initEvent(System::Event::Init & event)
        {
            if (event.getData().text)
            {
                _textUpdate();
            }
        }

        void ColorSliders::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.intLabels.clear();
                p.intEdits.clear();
                p.intSliders.clear();
                p.floatLabels.clear();
                p.floatEdits.clear();
                p.floatSliders.clear();
                p.layout->clearChildren();
                p.intObservers.clear();
                p.floatObservers.clear();

                const auto type = p.color.getType();
                const size_t channelCount = Image::getChannelCount(type);
                for (size_t i = 0; i < channelCount; ++i)
                {
                    if (Image::isIntType(type))
                    {
                        auto intLabel = UI::Text::Label::create(context);
                        p.layout->addChild(intLabel);
                        p.layout->setGridPos(intLabel, 0, static_cast<int>(i));
                        p.intLabels.push_back(intLabel);

                        auto intEdit = UI::Numeric::IntEdit::create(context);
                        p.layout->addChild(intEdit);
                        p.layout->setGridPos(intEdit, 1, static_cast<int>(i));
                        p.intEdits.push_back(intEdit);

                        auto intSlider = UI::Numeric::BasicIntSlider::create(UI::Orientation::Horizontal, context);
                        p.layout->addChild(intSlider);
                        p.layout->setGridPos(intSlider, 2, static_cast<int>(i));
                        p.layout->setStretch(intSlider, UI::GridStretch::Horizontal);
                        intEdit->setModel(intSlider->getModel());
                        p.intSliders.push_back(intSlider);
                    }
                    else
                    {
                        auto floatLabel = UI::Text::Label::create(context);
                        p.layout->addChild(floatLabel);
                        p.layout->setGridPos(floatLabel, 0, static_cast<int>(i));
                        p.floatLabels.push_back(floatLabel);

                        auto floatEdit = UI::Numeric::FloatEdit::create(context);
                        p.layout->addChild(floatEdit);
                        p.layout->setGridPos(floatEdit, 1, static_cast<int>(i));
                        p.floatEdits.push_back(floatEdit);

                        auto floatSlider = UI::Numeric::BasicFloatSlider::create(UI::Orientation::Horizontal, context);
                        p.layout->addChild(floatSlider);
                        p.layout->setGridPos(floatSlider, 2, static_cast<int>(i));
                        p.layout->setStretch(floatSlider, UI::GridStretch::Horizontal);
                        floatEdit->setModel(floatSlider->getModel());
                        p.floatSliders.push_back(floatSlider);
                    }
                }

                _colorUpdate();

                auto weak = std::weak_ptr<ColorSliders>(std::dynamic_pointer_cast<ColorSliders>(shared_from_this()));
                for (size_t i = 0; i < channelCount; ++i)
                {
                    if (Image::isIntType(type))
                    {
                        p.intObservers.push_back(Observer::Value<int>::create(
                            p.intSliders[i]->getModel()->observeValue(),
                            [weak, i](int value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto color = widget->_p->color;
                                    switch (Image::getDataType(color.getType()))
                                    {
                                    case Image::DataType::U8:  color.setU8(static_cast<Image::U8_T> (value), i); break;
                                    case Image::DataType::U10: color.setU10(static_cast<Image::U10_T>(value), i); break;
                                    case Image::DataType::U16: color.setU16(static_cast<Image::U16_T>(value), i); break;
                                    case Image::DataType::U32: color.setU32(static_cast<Image::U32_T>(value), i); break;
                                    default: break;
                                    }
                                    widget->setColor(color);
                                    if (widget->_p->colorCallback)
                                    {
                                        widget->_p->colorCallback(color);
                                    }
                                }
                            }));
                    }
                    else
                    {
                        p.floatObservers.push_back(Observer::Value<float>::create(
                            p.floatSliders[i]->getModel()->observeValue(),
                            [weak, i](float value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto color = widget->_p->color;
                                    switch (Image::getDataType(color.getType()))
                                    {
                                    case Image::DataType::F16: color.setF16(static_cast<Image::F16_T>(value), i); break;
                                    case Image::DataType::F32: color.setF32(static_cast<Image::F32_T>(value), i); break;
                                    default: break;
                                    }
                                    widget->setColor(color);
                                    if (widget->_p->colorCallback)
                                    {
                                        widget->_p->colorCallback(color);
                                    }
                                }
                            }));
                    }
                }
            }
        }

        void ColorSliders::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto type = p.color.getType();
            const size_t channelCount = Image::getChannelCount(type);
            for (size_t i = 0; i < channelCount; ++i)
            {
                if (Image::isIntType(type))
                {
                    auto slider = p.intSliders[i];
                    slider->setRange(Image::getIntRange(type));
                    int v = 0;
                    switch (Image::getDataType(type))
                    {
                    case Image::DataType::U8:  v = p.color.getU8(i); break;
                    case Image::DataType::U10: v = p.color.getU10(i); break;
                    case Image::DataType::U16: v = p.color.getU16(i); break;
                    case Image::DataType::U32: v = p.color.getU32(i); break;
                    default: break;
                    }
                    slider->setValue(v);
                }
                else
                {
                    auto slider = p.floatSliders[i];
                    slider->setRange(Image::getFloatRange(type));
                    float v = 0.F;
                    switch (Image::getDataType(type))
                    {
                    case Image::DataType::F16: v = p.color.getF16(i); break;
                    case Image::DataType::F32: v = p.color.getF32(i); break;
                    default: break;
                    }
                    slider->setValue(v);
                }
            }
        }

        void ColorSliders::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto type = p.color.getType();
            const size_t channelCount = Image::getChannelCount(type);
            switch (channelCount)
            {
            case 1:
            {
                const std::vector<std::string> text =
                {
                   _getText(DJV_TEXT("color_sliders_luminance"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("color_sliders_luminance_tooltip"))
                };
                if (Image::isIntType(type))
                {
                    p.intLabels[0]->setText(text[0]);
                    p.intLabels[0]->setTooltip(tooltips[0]);
                    p.intEdits[0]->setTooltip(tooltips[0]);
                    p.intSliders[0]->setTooltip(tooltips[0]);
                }
                else
                {
                    p.floatLabels[0]->setText(text[0]);
                    p.floatLabels[0]->setTooltip(tooltips[0]);
                    p.floatEdits[0]->setTooltip(tooltips[0]);
                    p.floatSliders[0]->setTooltip(tooltips[0]);
                }
                break;
            }
            case 2:
            {
                const std::vector<std::string> text =
                {
                 _getText(DJV_TEXT("color_sliders_luminance")),
                    _getText(DJV_TEXT("color_sliders_alpha"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("color_sliders_luminance_tooltip")),
                    _getText(DJV_TEXT("color_sliders_alpha_tooltip"))
                };
                if (Image::isIntType(type))
                {
                    for (size_t i = 0; i < 2; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 2; ++i)
                    {
                        p.intLabels[i]->setTooltip(tooltips[i]);
                        p.intEdits[i]->setTooltip(tooltips[i]);
                        p.intSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                else
                {
                    for (size_t i = 0; i < 2; ++i)
                    {
                        p.floatLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 2; ++i)
                    {
                        p.floatLabels[i]->setTooltip(tooltips[i]);
                        p.floatEdits[i]->setTooltip(tooltips[i]);
                        p.floatSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                break;
            }
            case 3:
            {
                std::vector<std::string> text;
                std::vector<std::string> tooltips;
                if (p.hsv)
                {
                    text =
                    {
                        _getText(DJV_TEXT("color_sliders_hue")),
                        _getText(DJV_TEXT("color_sliders_saturation")),
                        _getText(DJV_TEXT("color_sliders_value"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_hue_tooltip")),
                        _getText(DJV_TEXT("color_sliders_saturation_tooltip")),
                        _getText(DJV_TEXT("color_sliders_value_tooltip"))
                    };
                }
                else
                {
                    text =
                    {
                        _getText(DJV_TEXT("color_sliders_red")),
                        _getText(DJV_TEXT("color_sliders_green")),
                        _getText(DJV_TEXT("color_sliders_blue"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_red_tooltip")),
                        _getText(DJV_TEXT("color_sliders_green_tooltip")),
                        _getText(DJV_TEXT("color_sliders_blue_tooltip"))
                    };
                }
                if (Image::isIntType(type))
                {
                    for (size_t i = 0; i < 3; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 3; ++i)
                    {
                        p.intLabels[i]->setTooltip(tooltips[i]);
                        p.intEdits[i]->setTooltip(tooltips[i]);
                        p.intSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                else
                {
                    for (size_t i = 0; i < 3; ++i)
                    {
                        p.floatLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 3; ++i)
                    {
                        p.floatLabels[i]->setTooltip(tooltips[i]);
                        p.floatEdits[i]->setTooltip(tooltips[i]);
                        p.floatSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                break;
            }
            case 4:
            {
                std::vector<std::string> text;
                std::vector<std::string> tooltips;
                if (p.hsv)
                {
                    text =
                    {
                        _getText(DJV_TEXT("color_sliders_hue")),
                        _getText(DJV_TEXT("color_sliders_saturation")),
                        _getText(DJV_TEXT("color_sliders_value")),
                        _getText(DJV_TEXT("color_sliders_alpha"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_hue_tooltip")),
                        _getText(DJV_TEXT("color_sliders_saturation_tooltip")),
                        _getText(DJV_TEXT("color_sliders_value_tooltip")),
                        _getText(DJV_TEXT("color_sliders_alpha_tooltip"))
                    };
                }
                else
                {
                    text =
                    {
                        _getText(DJV_TEXT("color_sliders_red")),
                        _getText(DJV_TEXT("color_sliders_green")),
                        _getText(DJV_TEXT("color_sliders_blue")),
                        _getText(DJV_TEXT("color_sliders_alpha"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_red_tooltip")),
                        _getText(DJV_TEXT("color_sliders_green_tooltip")),
                        _getText(DJV_TEXT("color_sliders_blue_tooltip")),
                        _getText(DJV_TEXT("color_sliders_alpha_tooltip"))
                    };
                }
                if (Image::isIntType(type))
                {
                    for (size_t i = 0; i < 4; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 4; ++i)
                    {
                        p.intLabels[i]->setTooltip(tooltips[i]);
                        p.intEdits[i]->setTooltip(tooltips[i]);
                        p.intSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                else
                {
                    for (size_t i = 0; i < 4; ++i)
                    {
                        p.floatLabels[i]->setText(text[i]);
                    }
                    for (size_t i = 0; i < 4; ++i)
                    {
                        p.floatLabels[i]->setTooltip(tooltips[i]);
                        p.floatEdits[i]->setTooltip(tooltips[i]);
                        p.floatSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                break;
            }
            }
        }

        struct ColorPicker::Private
        {
            Image::Color color = Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<UI::ColorSwatch> colorSwatch;
            std::shared_ptr<ColorSliders> sliders;
            std::shared_ptr<ColorTypeWidget> typeWidget;
            std::shared_ptr<UI::HorizontalLayout> layout;
            std::function<void(const Image::Color &)> colorCallback;
        };

        void ColorPicker::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::ColorPicker");

            p.colorSwatch = UI::ColorSwatch::create(context);

            p.sliders = ColorSliders::create(context);

            p.typeWidget = ColorTypeWidget::create(context);

            p.layout = UI::HorizontalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::SpacingSmall);
            p.layout->addChild(p.colorSwatch);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            vLayout->addChild(p.sliders);
            vLayout->addChild(p.typeWidget);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _colorUpdate();

            auto weak = std::weak_ptr<ColorPicker>(std::dynamic_pointer_cast<ColorPicker>(shared_from_this()));
            p.sliders->setColorCallback(
                [weak](const Image::Color & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->setColor(value);
                    if (widget->_p->colorCallback)
                    {
                        widget->_p->colorCallback(value);
                    }
                }
            });

            p.typeWidget->setTypeCallback(
                [weak](Image::Type value)
            {
                if (auto widget = weak.lock())
                {
                    const auto color = widget->_p->color.convert(value);
                    widget->setColor(color);
                    if (widget->_p->colorCallback)
                    {
                        widget->_p->colorCallback(color);
                    }
                }
            });
        }

        ColorPicker::ColorPicker() :
            _p(new Private)
        {}

        ColorPicker::~ColorPicker()
        {}

        std::shared_ptr<ColorPicker> ColorPicker::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorPicker>(new ColorPicker);
            out->_init(context);
            return out;
        }

        const Image::Color & ColorPicker::getColor() const
        {
            return _p->color;
        }

        void ColorPicker::setColor(const Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _colorUpdate();
        }

        void ColorPicker::setColorCallback(const std::function<void(const Image::Color &)> & callback)
        {
            _p->colorCallback = callback;
        }

        void ColorPicker::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.layout->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorPicker::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const Math::BBox2f& g = getGeometry();
            const auto& style = _getStyle();
            p.layout->setGeometry(UI::Layout::getAlign(getMargin().bbox(g, style), p.layout->getMinimumSize(), getHAlign(), getVAlign()));
        }

        void ColorPicker::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            p.colorSwatch->setColor(p.color);
            p.sliders->setColor(p.color);
            p.typeWidget->setType(p.color.getType());
        }

        struct ColorPickerSwatch::Private
        {
            Image::Color color = Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<UI::ColorSwatch> colorSwatch;
            std::shared_ptr<UI::Window> window;
            std::function<void(const Image::Color&)> colorCallback;
        };

        void ColorPickerSwatch::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UIComponents::ColorPickerSwatch");
            setHAlign(UI::HAlign::Left);

            p.colorSwatch = UI::ColorSwatch::create(context);
            addChild(p.colorSwatch);

            _colorUpdate();

            auto weak = std::weak_ptr<ColorPickerSwatch>(std::dynamic_pointer_cast<ColorPickerSwatch>(shared_from_this()));
            p.colorSwatch->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->open();
                    }
                });
        }

        ColorPickerSwatch::ColorPickerSwatch() :
            _p(new Private)
        {}

        ColorPickerSwatch::~ColorPickerSwatch()
        {}

        std::shared_ptr<ColorPickerSwatch> ColorPickerSwatch::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSwatch>(new ColorPickerSwatch);
            out->_init(context);
            return out;
        }

        const Image::Color& ColorPickerSwatch::getColor() const
        {
            return _p->color;
        }

        void ColorPickerSwatch::setColor(const Image::Color& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _colorUpdate();
        }

        void ColorPickerSwatch::setColorCallback(const std::function<void(const Image::Color&)>& callback)
        {
            _p->colorCallback = callback;
        }

        UI::MetricsRole ColorPickerSwatch::getSwatchSizeRole() const
        {
            return _p->colorSwatch->getSwatchSizeRole();
        }

        void ColorPickerSwatch::setSwatchSizeRole(UI::MetricsRole value)
        {
            _p->colorSwatch->setSwatchSizeRole(value);
        }

        void ColorPickerSwatch::open()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (!p.window)
                {
                    auto colorPicker = ColorPicker::create(context);
                    colorPicker->setColor(p.color);
                    colorPicker->setMargin(UI::MetricsRole::MarginSmall);

                    auto popupWidget = UI::PopupWidget::create(context);
                    popupWidget->addChild(colorPicker);
                    auto popupLayout = UI::Layout::Popup::create(context);
                    popupLayout->addChild(popupWidget);
                    popupLayout->setButton(p.colorSwatch);

                    auto overlay = UI::Layout::Overlay::create(context);
                    overlay->setFadeIn(false);
                    overlay->setBackgroundColorRole(UI::ColorRole::None);
                    overlay->addChild(popupLayout);

                    p.window = UI::Window::create(context);
                    p.window->setBackgroundColorRole(UI::ColorRole::None);
                    p.window->addChild(overlay);
                    
                    auto weak = std::weak_ptr<ColorPickerSwatch>(std::dynamic_pointer_cast<ColorPickerSwatch>(shared_from_this()));
                    colorPicker->setColorCallback(
                        [weak](const Image::Color& value)
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->color = value;
                                widget->_colorUpdate();
                                if (widget->_p->colorCallback)
                                {
                                    widget->_p->colorCallback(widget->_p->color);
                                }
                            }
                        });

                    overlay->setCloseCallback(
                        [weak]
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->close();
                            }
                        });

                    p.window->show();
                }
            }
        }

        void ColorPickerSwatch::close()
        {
            DJV_PRIVATE_PTR();
            if (p.window)
            {
                p.window->close();
                p.window.reset();
            }
            p.colorSwatch->takeTextFocus();
        }

        void ColorPickerSwatch::_preLayoutEvent(System::Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.colorSwatch->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorPickerSwatch::_layoutEvent(System::Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const Math::BBox2f& g = getGeometry();
            const auto& style = _getStyle();
            p.colorSwatch->setGeometry(UI::Layout::getAlign(getMargin().bbox(g, style), p.colorSwatch->getMinimumSize(), getHAlign(), getVAlign()));
        }

        void ColorPickerSwatch::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            p.colorSwatch->setEnabled(p.color.getType() != Image::Type::None);
            p.colorSwatch->setColor(p.color);
        }

    } // namespace UIComponents
} // namespace djv


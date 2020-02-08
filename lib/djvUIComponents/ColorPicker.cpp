//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Color.h>
#include <djvAV/Render2D.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ColorTypeWidget::Private
        {
            AV::Image::Type type = AV::Image::Type::First;
            std::shared_ptr<ComboBox> comboBox;
            std::function<void(AV::Image::Type)> typeCallback;
        };

        void ColorTypeWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ColorTypeWidget");
            setVAlign(VAlign::Center);

            p.comboBox = ComboBox::create(context);
            p.comboBox->setBackgroundRole(ColorRole::None);
            addChild(p.comboBox);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorTypeWidget>(std::dynamic_pointer_cast<ColorTypeWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    const auto type = static_cast<AV::Image::Type>(value + 1);
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

        std::shared_ptr<ColorTypeWidget> ColorTypeWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorTypeWidget>(new ColorTypeWidget);
            out->_init(context);
            return out;
        }

        AV::Image::Type ColorTypeWidget::getType() const
        {
            return _p->type;
        }

        void ColorTypeWidget::setType(AV::Image::Type value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.type)
                return;
            p.type = value;
            p.comboBox->setCurrentItem(static_cast<int>(p.type) - 1);
        }

        void ColorTypeWidget::setTypeCallback(const std::function<void(AV::Image::Type)> & callback)
        {
            _p->typeCallback = callback;
        }

        void ColorTypeWidget::_preLayoutEvent(Event::PreLayout & event)
        {
            _setMinimumSize(_p->comboBox->getMinimumSize());
        }

        void ColorTypeWidget::_layoutEvent(Event::Layout &)
        {
            _p->comboBox->setGeometry(getGeometry());
        }

        void ColorTypeWidget::_initEvent(Event::Init & event)
        {
            Widget::_initEvent(event);
            setTooltip(_getText(DJV_TEXT("color_type_widget_tooltip")));
            _widgetUpdate();
        }

        void ColorTypeWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            for (size_t i = static_cast<size_t>(AV::Image::Type::L_U8); i < static_cast<size_t>(AV::Image::Type::Count); ++i)
            {
                std::stringstream ss;
                ss << static_cast<AV::Image::Type>(i);
                p.comboBox->addItem(ss.str());
            }
            p.comboBox->setCurrentItem(static_cast<int>(p.type) - 1);
        }

        struct ColorSliders::Private
        {
            AV::Image::Color color = AV::Image::Color(0.F, 0.F, 0.F);
            bool hsv = false;
            std::vector<std::shared_ptr<Label> > intLabels;
            std::vector<std::shared_ptr<IntEdit> > intEdits;
            std::vector<std::shared_ptr<BasicIntSlider> > intSliders;
            std::vector<std::shared_ptr<Label> > floatLabels;
            std::vector<std::shared_ptr<FloatEdit> > floatEdits;
            std::vector<std::shared_ptr<BasicFloatSlider> > floatSliders;
            std::shared_ptr<GridLayout> layout;
            std::function<void(const AV::Image::Color &)> colorCallback;
            std::vector<std::shared_ptr<ValueObserver<int> > > intObservers;
            std::vector < std::shared_ptr<ValueObserver<float> > > floatObservers;
        };

        void ColorSliders::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ColorSliders");

            p.layout = GridLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
            addChild(p.layout);

            _widgetUpdate();
            _textUpdate();
        }

        ColorSliders::ColorSliders() :
            _p(new Private)
        {}

        ColorSliders::~ColorSliders()
        {}

        std::shared_ptr<ColorSliders> ColorSliders::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSliders>(new ColorSliders);
            out->_init(context);
            return out;
        }

        const AV::Image::Color & ColorSliders::getColor() const
        {
            return _p->color;
        }

        void ColorSliders::setColor(const AV::Image::Color & value)
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

        void ColorSliders::setColorCallback(const std::function<void(const AV::Image::Color &)> & callback)
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

        void ColorSliders::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorSliders::_layoutEvent(Event::Layout &)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void ColorSliders::_initEvent(Event::Init & event)
        {
            Widget::_initEvent(event);
            _textUpdate();
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
                const size_t channelCount = AV::Image::getChannelCount(type);
                for (size_t i = 0; i < channelCount; ++i)
                {
                    if (AV::Image::isIntType(type))
                    {
                        auto intLabel = Label::create(context);
                        p.layout->addChild(intLabel);
                        p.layout->setGridPos(intLabel, 0, static_cast<int>(i));
                        p.intLabels.push_back(intLabel);

                        auto intEdit = IntEdit::create(context);
                        p.layout->addChild(intEdit);
                        p.layout->setGridPos(intEdit, 1, static_cast<int>(i));
                        p.intEdits.push_back(intEdit);

                        auto intSlider = BasicIntSlider::create(Orientation::Horizontal, context);
                        p.layout->addChild(intSlider);
                        p.layout->setGridPos(intSlider, 2, static_cast<int>(i));
                        p.layout->setStretch(intSlider, GridStretch::Horizontal);
                        intEdit->setModel(intSlider->getModel());
                        p.intSliders.push_back(intSlider);
                    }
                    else
                    {
                        auto floatLabel = Label::create(context);
                        p.layout->addChild(floatLabel);
                        p.layout->setGridPos(floatLabel, 0, static_cast<int>(i));
                        p.floatLabels.push_back(floatLabel);

                        auto floatEdit = FloatEdit::create(context);
                        p.layout->addChild(floatEdit);
                        p.layout->setGridPos(floatEdit, 1, static_cast<int>(i));
                        p.floatEdits.push_back(floatEdit);

                        auto floatSlider = BasicFloatSlider::create(Orientation::Horizontal, context);
                        p.layout->addChild(floatSlider);
                        p.layout->setGridPos(floatSlider, 2, static_cast<int>(i));
                        p.layout->setStretch(floatSlider, GridStretch::Horizontal);
                        floatEdit->setModel(floatSlider->getModel());
                        p.floatSliders.push_back(floatSlider);
                    }
                }

                _colorUpdate();

                auto weak = std::weak_ptr<ColorSliders>(std::dynamic_pointer_cast<ColorSliders>(shared_from_this()));
                for (size_t i = 0; i < channelCount; ++i)
                {
                    if (AV::Image::isIntType(type))
                    {
                        p.intObservers.push_back(ValueObserver<int>::create(
                            p.intSliders[i]->getModel()->observeValue(),
                            [weak, i](int value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto color = widget->_p->color;
                                    switch (AV::Image::getDataType(color.getType()))
                                    {
                                    case AV::Image::DataType::U8:  color.setU8(static_cast<AV::Image::U8_T> (value), i); break;
                                    case AV::Image::DataType::U10: color.setU10(static_cast<AV::Image::U10_T>(value), i); break;
                                    case AV::Image::DataType::U16: color.setU16(static_cast<AV::Image::U16_T>(value), i); break;
                                    case AV::Image::DataType::U32: color.setU32(static_cast<AV::Image::U32_T>(value), i); break;
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
                        p.floatObservers.push_back(ValueObserver<float>::create(
                            p.floatSliders[i]->getModel()->observeValue(),
                            [weak, i](float value)
                            {
                                if (auto widget = weak.lock())
                                {
                                    auto color = widget->_p->color;
                                    switch (AV::Image::getDataType(color.getType()))
                                    {
                                    case AV::Image::DataType::F16: color.setF16(static_cast<AV::Image::F16_T>(value), i); break;
                                    case AV::Image::DataType::F32: color.setF32(static_cast<AV::Image::F32_T>(value), i); break;
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
            const size_t channelCount = AV::Image::getChannelCount(type);
            for (size_t i = 0; i < channelCount; ++i)
            {
                if (AV::Image::isIntType(type))
                {
                    auto model = p.intSliders[i]->getModel();
                    model->setRange(AV::Image::getIntRange(type));
                    int v = 0;
                    switch (AV::Image::getDataType(type))
                    {
                    case AV::Image::DataType::U8:  v = p.color.getU8(i); break;
                    case AV::Image::DataType::U10: v = p.color.getU10(i); break;
                    case AV::Image::DataType::U16: v = p.color.getU16(i); break;
                    case AV::Image::DataType::U32: v = p.color.getU32(i); break;
                    default: break;
                    }
                    model->setValue(v);
                }
                else
                {
                    auto model = p.floatSliders[i]->getModel();
                    model->setRange(AV::Image::getFloatRange(type));
                    float v = 0.F;
                    switch (AV::Image::getDataType(type))
                    {
                    case AV::Image::DataType::F16: v = p.color.getF16(i); break;
                    case AV::Image::DataType::F32: v = p.color.getF32(i); break;
                    default: break;
                    }
                    model->setValue(v);
                }
            }
        }

        void ColorSliders::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto type = p.color.getType();
            const size_t channelCount = AV::Image::getChannelCount(type);
            switch (channelCount)
            {
            case 1:
            {
                const std::vector<std::string> text =
                {
                    _getText(DJV_TEXT("key_l"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("color_sliders_luminance_tooltip"))
                };
                if (AV::Image::isIntType(type))
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
                    _getText(DJV_TEXT("key_l")),
                    _getText(DJV_TEXT("key_a"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("color_sliders_luminance_tooltip")),
                    _getText(DJV_TEXT("color_sliders_alpha_tooltip"))
                };
                if (AV::Image::isIntType(type))
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
                        _getText(DJV_TEXT("key_h")),
                        _getText(DJV_TEXT("key_s")),
                        _getText(DJV_TEXT("key_v"))
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
                        _getText(DJV_TEXT("key_r")),
                        _getText(DJV_TEXT("key_g")),
                        _getText(DJV_TEXT("key_b"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_red_tooltip")),
                        _getText(DJV_TEXT("color_sliders_green_tooltip")),
                        _getText(DJV_TEXT("color_sliders_blue_tooltip"))
                    };
                }
                if (AV::Image::isIntType(type))
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
                        _getText(DJV_TEXT("key_h")),
                        _getText(DJV_TEXT("key_s")),
                        _getText(DJV_TEXT("key_v")),
                        _getText(DJV_TEXT("key_a"))
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
                        _getText(DJV_TEXT("key_r")),
                        _getText(DJV_TEXT("key_g")),
                        _getText(DJV_TEXT("key_b")),
                        _getText(DJV_TEXT("key_a"))
                    };
                    tooltips =
                    {
                        _getText(DJV_TEXT("color_sliders_red_tooltip")),
                        _getText(DJV_TEXT("color_sliders_green_tooltip")),
                        _getText(DJV_TEXT("color_sliders_blue_tooltip")),
                        _getText(DJV_TEXT("color_sliders_alpha_tooltip"))
                    };
                }
                if (AV::Image::isIntType(type))
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
            AV::Image::Color color = AV::Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<ColorSwatch> colorSwatch;
            std::shared_ptr<ColorSliders> sliders;
            std::shared_ptr<ColorTypeWidget> typeWidget;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(const AV::Image::Color &)> colorCallback;
        };

        void ColorPicker::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ColorPicker");

            p.colorSwatch = ColorSwatch::create(context);

            p.sliders = ColorSliders::create(context);

            p.typeWidget = ColorTypeWidget::create(context);

            p.layout = HorizontalLayout::create(context);
            p.layout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
            p.layout->addChild(p.colorSwatch);
            auto vLayout = VerticalLayout::create(context);
            vLayout->setSpacing(Layout::Spacing(MetricsRole::SpacingSmall));
            vLayout->addChild(p.sliders);
            vLayout->addChild(p.typeWidget);
            p.layout->addChild(vLayout);
            addChild(p.layout);

            _colorUpdate();

            auto weak = std::weak_ptr<ColorPicker>(std::dynamic_pointer_cast<ColorPicker>(shared_from_this()));
            p.sliders->setColorCallback(
                [weak](const AV::Image::Color & value)
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
                [weak](AV::Image::Type value)
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

        std::shared_ptr<ColorPicker> ColorPicker::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorPicker>(new ColorPicker);
            out->_init(context);
            return out;
        }

        const AV::Image::Color & ColorPicker::getColor() const
        {
            return _p->color;
        }

        void ColorPicker::setColor(const AV::Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _colorUpdate();
        }

        void ColorPicker::setColorCallback(const std::function<void(const AV::Image::Color &)> & callback)
        {
            _p->colorCallback = callback;
        }

        void ColorPicker::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.layout->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorPicker::_layoutEvent(Event::Layout &)
        {
            DJV_PRIVATE_PTR();
            const BBox2f g = getGeometry();
            const auto& style = _getStyle();
            p.layout->setGeometry(getAlign(getMargin().bbox(g, style), p.layout->getMinimumSize(), getHAlign(), getVAlign()));
        }

        void ColorPicker::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            p.colorSwatch->setColor(p.color);
            p.sliders->setColor(p.color);
            p.typeWidget->setType(p.color.getType());
        }

        namespace
        {
            class OverlayLayout : public Widget
            {
                DJV_NON_COPYABLE(OverlayLayout);

            protected:
                void _init(const std::shared_ptr<Context>&);
                OverlayLayout();

            public:
                static std::shared_ptr<OverlayLayout> create(const std::shared_ptr<Context>&);

                void setButton(const std::shared_ptr<Widget>&, const std::weak_ptr<Widget>&);

            protected:
                void _layoutEvent(Event::Layout&) override;
                void _paintEvent(Event::Paint&) override;

                void _childRemovedEvent(Event::ChildRemoved&) override;

            private:
                std::map<std::shared_ptr<Widget>, std::weak_ptr<Widget> > _widgetToButton;
                std::map< std::shared_ptr<Widget>, Popup> _widgetToPopup;
            };

            void OverlayLayout::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::PopupWidget::OverlayLayout");
            }

            OverlayLayout::OverlayLayout()
            {}

            std::shared_ptr<OverlayLayout> OverlayLayout::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<OverlayLayout>(new OverlayLayout);
                out->_init(context);
                return out;
            }

            void OverlayLayout::setButton(const std::shared_ptr<Widget>& widget, const std::weak_ptr<Widget>& button)
            {
                _widgetToButton[widget] = button;
            }

            void OverlayLayout::_layoutEvent(Event::Layout&)
            {
                const BBox2f& g = getGeometry();
                for (const auto& i : _widgetToButton)
                {
                    if (auto button = i.second.lock())
                    {
                        const auto& buttonBBox = button->getGeometry();
                        const auto& minimumSize = i.first->getMinimumSize();
                        Popup popup = Popup::BelowRight;
                        auto j = _widgetToPopup.find(i.first);
                        if (j != _widgetToPopup.end())
                        {
                            popup = j->second;
                        }
                        else
                        {
                            popup = Layout::getPopup(popup, g, buttonBBox, minimumSize);
                            _widgetToPopup[i.first] = popup;
                        }
                        i.first->setGeometry(Layout::getPopupGeometry(popup, buttonBBox, minimumSize).intersect(g));
                    }
                }
            }

            void OverlayLayout::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                const auto& style = _getStyle();
                const float sh = style->getMetric(MetricsRole::Shadow);
                auto render = _getRender();
                render->setFillColor(style->getColor(ColorRole::Shadow));
                for (const auto& i : getChildWidgets())
                {
                    BBox2f g = i->getGeometry();
                    g.min.x -= sh;
                    g.max.x += sh;
                    g.max.y += sh;
                    if (g.isValid())
                    {
                        render->drawShadow(g, sh);
                    }
                }
            }

            void OverlayLayout::_childRemovedEvent(Event::ChildRemoved& event)
            {
                if (auto widget = std::dynamic_pointer_cast<Widget>(event.getChild()))
                {
                    const auto j = _widgetToButton.find(widget);
                    if (j != _widgetToButton.end())
                    {
                        _widgetToButton.erase(j);
                    }
                }
            }

        } // namespace

        struct ColorPickerSwatch::Private
        {
            AV::Image::Color color = AV::Image::Color(0.F, 0.F, 0.F);
            std::shared_ptr<ColorSwatch> colorSwatch;
            std::shared_ptr<Window> window;
            std::function<void(const AV::Image::Color&)> colorCallback;
        };

        void ColorPickerSwatch::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ColorPickerSwatch");
            setHAlign(HAlign::Left);

            p.colorSwatch = ColorSwatch::create(context);
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

        std::shared_ptr<ColorPickerSwatch> ColorPickerSwatch::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSwatch>(new ColorPickerSwatch);
            out->_init(context);
            return out;
        }

        const AV::Image::Color& ColorPickerSwatch::getColor() const
        {
            return _p->color;
        }

        void ColorPickerSwatch::setColor(const AV::Image::Color& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _colorUpdate();
        }

        void ColorPickerSwatch::setColorCallback(const std::function<void(const AV::Image::Color&)>& callback)
        {
            _p->colorCallback = callback;
        }

        MetricsRole ColorPickerSwatch::getSwatchSizeRole() const
        {
            return _p->colorSwatch->getSwatchSizeRole();
        }

        void ColorPickerSwatch::setSwatchSizeRole(MetricsRole value)
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
                    p.window = Window::create(context);
                    p.window->setBackgroundRole(UI::ColorRole::None);

                    auto colorPicker = ColorPicker::create(context);
                    colorPicker->setColor(p.color);
                    colorPicker->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
                    auto border = Layout::Border::create(context);
                    border->setBackgroundRole(UI::ColorRole::Background);
                    border->addChild(colorPicker);

                    auto overlayLayout = OverlayLayout::create(context);
                    overlayLayout->addChild(border);
                    overlayLayout->setButton(border, p.colorSwatch);

                    auto overlay = Layout::Overlay::create(context);
                    overlay->setFadeIn(false);
                    overlay->setBackgroundRole(UI::ColorRole::None);
                    overlay->addChild(overlayLayout);

                    p.window->addChild(overlay);
                    
                    auto weak = std::weak_ptr<ColorPickerSwatch>(std::dynamic_pointer_cast<ColorPickerSwatch>(shared_from_this()));
                    colorPicker->setColorCallback(
                        [weak](const AV::Image::Color& value)
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

        void ColorPickerSwatch::_preLayoutEvent(Event::PreLayout& event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.colorSwatch->getMinimumSize() + getMargin().getSize(style));
        }

        void ColorPickerSwatch::_layoutEvent(Event::Layout&)
        {
            DJV_PRIVATE_PTR();
            const BBox2f g = getGeometry();
            const auto& style = _getStyle();
            p.colorSwatch->setGeometry(getAlign(getMargin().bbox(g, style), p.colorSwatch->getMinimumSize(), getHAlign(), getVAlign()));
        }

        void ColorPickerSwatch::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            p.colorSwatch->setEnabled(p.color.getType() != AV::Image::Type::None);
            p.colorSwatch->setColor(p.color);
        }

    } // namespace UI
} // namespace djv


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

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/ComboBox.h>
#include <djvUI/ColorSwatch.h>
#include <djvUI/GridLayout.h>
#include <djvUI/EventSystem.h>
#include <djvUI/IDialog.h>
#include <djvUI/IntEdit.h>
#include <djvUI/IntSlider.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvAV/Color.h>

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

        void ColorTypeWidget::_init(Context * context)
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

        std::shared_ptr<ColorTypeWidget> ColorTypeWidget::create(Context * context)
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

        void ColorTypeWidget::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            setTooltip(_getText(DJV_TEXT("Color type widget tooltip")));
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

        struct RGBColorSliders::Private
        {
            AV::Image::Color color = AV::Image::Color(0.f, 0.f, 0.f);
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

        void RGBColorSliders::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::RGBColorSliders");

            p.layout = GridLayout::create(context);
            addChild(p.layout);

            _colorTypeUpdate();
            _colorUpdate();
            _textUpdate();
        }

        RGBColorSliders::RGBColorSliders() :
            _p(new Private)
        {}

        RGBColorSliders::~RGBColorSliders()
        {}

        std::shared_ptr<RGBColorSliders> RGBColorSliders::create(Context * context)
        {
            auto out = std::shared_ptr<RGBColorSliders>(new RGBColorSliders);
            out->_init(context);
            return out;
        }

        const AV::Image::Color & RGBColorSliders::getColor() const
        {
            return _p->color;
        }

        void RGBColorSliders::setColor(const AV::Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            const bool typeUpdate = value.getType() != p.color.getType();
            p.color = value;
            if (typeUpdate)
            {
                _colorTypeUpdate();
            }
            _colorUpdate();
            _textUpdate();
        }

        void RGBColorSliders::setColorCallback(const std::function<void(const AV::Image::Color &)> & callback)
        {
            _p->colorCallback = callback;
        }

        void RGBColorSliders::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void RGBColorSliders::_layoutEvent(Event::Layout &)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void RGBColorSliders::_localeEvent(Event::Locale &)
        {
            _textUpdate();
        }

        void RGBColorSliders::_colorTypeUpdate()
        {
            DJV_PRIVATE_PTR();
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
            auto context = getContext();
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
            auto weak = std::weak_ptr<RGBColorSliders>(std::dynamic_pointer_cast<RGBColorSliders>(shared_from_this()));
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

        void RGBColorSliders::_colorUpdate()
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
                    float v = 0.f;
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

        void RGBColorSliders::_textUpdate()
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
                    _getText(DJV_TEXT("L"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("RGB color sliders luminance tooltip"))
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
                    _getText(DJV_TEXT("L")),
                    _getText(DJV_TEXT("A"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("RGB color sliders luminance tooltip")),
                    _getText(DJV_TEXT("RGB color sliders alpha tooltip"))
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
                const std::vector<std::string> text =
                {
                    _getText(DJV_TEXT("R")),
                    _getText(DJV_TEXT("G")),
                    _getText(DJV_TEXT("B"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("RGB color sliders red tooltip")),
                    _getText(DJV_TEXT("RGB color sliders green tooltip")),
                    _getText(DJV_TEXT("RGB color sliders blue tooltip"))
                };
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
                const std::vector<std::string> text =
                {
                    _getText(DJV_TEXT("R")),
                    _getText(DJV_TEXT("G")),
                    _getText(DJV_TEXT("B")),
                    _getText(DJV_TEXT("A"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("RGB color sliders red tooltip")),
                    _getText(DJV_TEXT("RGB color sliders green tooltip")),
                    _getText(DJV_TEXT("RGB color sliders blue tooltip")),
                    _getText(DJV_TEXT("RGB color sliders alpha tooltip"))
                };
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

        struct HSVColorSliders::Private
        {
            AV::Image::Color color = AV::Image::Color(0.f, 0.f, 0.f);
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

        void HSVColorSliders::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::HSVColorSliders");

            p.layout = GridLayout::create(context);
            addChild(p.layout);

            _colorTypeUpdate();
            _colorUpdate();
            _textUpdate();
        }

        HSVColorSliders::HSVColorSliders() :
            _p(new Private)
        {}

        HSVColorSliders::~HSVColorSliders()
        {}

        std::shared_ptr<HSVColorSliders> HSVColorSliders::create(Context * context)
        {
            auto out = std::shared_ptr<HSVColorSliders>(new HSVColorSliders);
            out->_init(context);
            return out;
        }

        const AV::Image::Color & HSVColorSliders::getColor() const
        {
            return _p->color;
        }

        void HSVColorSliders::setColor(const AV::Image::Color & value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            const bool typeUpdate = value.getType() != p.color.getType();
            p.color = value;
            if (typeUpdate)
            {
                _colorTypeUpdate();
            }
            _colorUpdate();
            _textUpdate();
        }

        void HSVColorSliders::setColorCallback(const std::function<void(const AV::Image::Color &)> & callback)
        {
            _p->colorCallback = callback;
        }

        void HSVColorSliders::_preLayoutEvent(Event::PreLayout & event)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }

        void HSVColorSliders::_layoutEvent(Event::Layout &)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void HSVColorSliders::_localeEvent(Event::Locale &)
        {
            _textUpdate();
        }

        void HSVColorSliders::_colorTypeUpdate()
        {
            DJV_PRIVATE_PTR();
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
            auto context = getContext();
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
            auto weak = std::weak_ptr<HSVColorSliders>(std::dynamic_pointer_cast<HSVColorSliders>(shared_from_this()));
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
                            const auto type = color.getType();
                            switch (AV::Image::getDataType(type))
                            {
                            case AV::Image::DataType::U8:  color.setU8(static_cast<AV::Image::U8_T> (value), i); break;
                            case AV::Image::DataType::U10: color.setU10(static_cast<AV::Image::U10_T>(value), i); break;
                            case AV::Image::DataType::U16: color.setU16(static_cast<AV::Image::U16_T>(value), i); break;
                            case AV::Image::DataType::U32: color.setU32(static_cast<AV::Image::U32_T>(value), i); break;
                            default: break;
                            }
                            /*switch (AV::Image::getChannelType(type))
                            {
                            case AV::Image::ChannelType::RGB:
                            case AV::Image::ChannelType::RGBA:
                            {
                                const auto tmp = color.convert(AV::Image::Type::RGBA_F32);
                                AV::Image::Color tmp2(AV::Image::Type::RGBA_F32);
                                AV::Image::Color::hsvToRGB(reinterpret_cast<const float *>(tmp.getData()), reinterpret_cast<float *>(tmp2.getData()));
                                color = tmp2.convert(type);
                                break;
                            }
                            default: break;
                            }*/
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
                            const auto type = color.getType();
                            switch (AV::Image::getDataType(type))
                            {
                            case AV::Image::DataType::F16: color.setF16(static_cast<AV::Image::F16_T>(value), i); break;
                            case AV::Image::DataType::F32: color.setF32(static_cast<AV::Image::F32_T>(value), i); break;
                            default: break;
                            }
                            /*switch (AV::Image::getChannelType(type))
                            {
                            case AV::Image::ChannelType::RGB:
                            case AV::Image::ChannelType::RGBA:
                            {
                                const auto tmp = color.convert(AV::Image::Type::RGBA_F32);
                                AV::Image::Color tmp2(AV::Image::Type::RGBA_F32);
                                AV::Image::Color::hsvToRGB(reinterpret_cast<const float *>(tmp.getData()), reinterpret_cast<float *>(tmp2.getData()));
                                color = tmp2.convert(type);
                                break;
                            }
                            default: break;
                            }*/
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

        void HSVColorSliders::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            const auto type = p.color.getType();
            auto color = p.color;
            /*switch (AV::Image::getChannelType(type))
            {
            case AV::Image::ChannelType::RGB:
            case AV::Image::ChannelType::RGBA:
            {
                const auto tmp = color.convert(AV::Image::Type::RGBA_F32);
                AV::Image::Color tmp2(AV::Image::Type::RGBA_F32);
                AV::Image::Color::rgbToHSV(reinterpret_cast<const float *>(tmp.getData()), reinterpret_cast<float *>(tmp2.getData()));
                const auto color = tmp2.convert(type);
                break;
            }
            default: break;
            }*/
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
                    case AV::Image::DataType::U8:  v = color.getU8(i); break;
                    case AV::Image::DataType::U10: v = color.getU10(i); break;
                    case AV::Image::DataType::U16: v = color.getU16(i); break;
                    case AV::Image::DataType::U32: v = color.getU32(i); break;
                    default: break;
                    }
                    model->setValue(v);
                    p.intLabels[i]->show();
                    p.intEdits[i]->show();
                    p.intSliders[i]->show();
                }
                else
                {
                    auto model = p.floatSliders[i]->getModel();
                    model->setRange(AV::Image::getFloatRange(type));
                    float v = 0.f;
                    switch (AV::Image::getDataType(type))
                    {
                    case AV::Image::DataType::F16: v = color.getF16(i); break;
                    case AV::Image::DataType::F32: v = color.getF32(i); break;
                    default: break;
                    }
                    model->setValue(v);
                    p.floatLabels[i]->show();
                    p.floatEdits[i]->show();
                    p.floatSliders[i]->show();
                }
            }
        }

        void HSVColorSliders::_textUpdate()
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
                    _getText(DJV_TEXT("V"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("HSV color sliders value tooltip"))
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
                    _getText(DJV_TEXT("V")),
                    _getText(DJV_TEXT("A"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("HSV color sliders value tooltip")),
                    _getText(DJV_TEXT("HSV color sliders alpha tooltip"))
                };
                if (AV::Image::isIntType(type))
                {
                    for (size_t i = 0; i < 2; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
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
                        p.floatLabels[i]->setTooltip(tooltips[i]);
                        p.floatEdits[i]->setTooltip(tooltips[i]);
                        p.floatSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                break;
            }
            case 3:
            {
                const std::vector<std::string> text =
                {
                    _getText(DJV_TEXT("H")),
                    _getText(DJV_TEXT("S")),
                    _getText(DJV_TEXT("V"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("HSV color sliders hue tooltip")),
                    _getText(DJV_TEXT("HSV color sliders saturation tooltip")),
                    _getText(DJV_TEXT("HSV color sliders value tooltip"))
                };
                if (AV::Image::isIntType(type))
                {
                    for (size_t i = 0; i < 3; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
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
                        p.floatLabels[i]->setTooltip(tooltips[i]);
                        p.floatEdits[i]->setTooltip(tooltips[i]);
                        p.floatSliders[i]->setTooltip(tooltips[i]);
                    }
                }
                break;
            }
            case 4:
            {
                const std::vector<std::string> text =
                {
                    _getText(DJV_TEXT("H")),
                    _getText(DJV_TEXT("S")),
                    _getText(DJV_TEXT("V")),
                    _getText(DJV_TEXT("A"))
                };
                const std::vector<std::string> tooltips =
                {
                    _getText(DJV_TEXT("HSV color sliders hue tooltip")),
                    _getText(DJV_TEXT("HSV color sliders saturation tooltip")),
                    _getText(DJV_TEXT("HSV color sliders value tooltip")),
                    _getText(DJV_TEXT("HSV color sliders alpha tooltip"))
                };
                if (AV::Image::isIntType(type))
                {
                    for (size_t i = 0; i < 4; ++i)
                    {
                        p.intLabels[i]->setText(text[i]);
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
            AV::Image::Color color = AV::Image::Color(0.f, 0.f, 0.f);
            std::shared_ptr<ColorSwatch> colorSwatch;
            std::shared_ptr<RGBColorSliders> sliders;
            std::shared_ptr<ColorTypeWidget> typeWidget;
            std::shared_ptr<HorizontalLayout> layout;
            std::function<void(const AV::Image::Color &)> colorCallback;
        };

        void ColorPicker::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ColorPicker");

            p.colorSwatch = ColorSwatch::create(context);

            p.sliders = RGBColorSliders::create(context);
            p.sliders->setMargin(MetricsRole::MarginSmall);

            p.typeWidget = ColorTypeWidget::create(context);

            p.layout = HorizontalLayout::create(context);
            p.layout->addChild(p.colorSwatch);
            auto vLayout = VerticalLayout::create(context);
            vLayout->setSpacing(MetricsRole::None);
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

        std::shared_ptr<ColorPicker> ColorPicker::create(Context * context)
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
            class ColorPickerDialog : public IDialog
            {
                DJV_NON_COPYABLE(ColorPickerDialog);

            protected:
                void _init(Context* context)
                {
                    IDialog::_init(context);

                    setClassName("djv::UI::ColorPickerDialog");
                    setFillLayout(false);

                    _colorPicker = ColorPicker::create(context);

                    auto layout = VerticalLayout::create(context);
                    layout->setMargin(MetricsRole::Margin);
                    layout->setShadowOverlay({ UI::Side::Top });
                    layout->addChild(_colorPicker);
                    addChild(layout);
                    setStretch(layout, RowStretch::Expand);
                }

                ColorPickerDialog()
                {}

            public:
                static std::shared_ptr<ColorPickerDialog> create(Context* context)
                {
                    auto out = std::shared_ptr<ColorPickerDialog>(new ColorPickerDialog);
                    out->_init(context);
                    return out;
                }

                void setColor(const AV::Image::Color& value)
                {
                    _colorPicker->setColor(value);
                }

                void setColorCallback(const std::function<void(const AV::Image::Color)>& value)
                {
                    _colorPicker->setColorCallback(value);
                }

            protected:

            private:
                std::shared_ptr<ColorPicker> _colorPicker;
            };

        } // namespace

        struct ColorPickerDialogSystem::Private
        {
            std::shared_ptr<ColorPickerDialog> colorPickerDialog;
        };

        void ColorPickerDialogSystem::_init(Context* context)
        {
            ISystem::_init("djv::UI::ColorPickerDialogSystem", context);
        }

        ColorPickerDialogSystem::ColorPickerDialogSystem() :
            _p(new Private)
        {}

        ColorPickerDialogSystem::~ColorPickerDialogSystem()
        {}

        std::shared_ptr<ColorPickerDialogSystem> ColorPickerDialogSystem::create(Context* context)
        {
            auto out = std::shared_ptr<ColorPickerDialogSystem>(new ColorPickerDialogSystem);
            out->_init(context);
            return out;
        }

        void ColorPickerDialogSystem::colorPicker(
            const std::string& title,
            const AV::Image::Color& color,
            const std::function<void(const AV::Image::Color&)>& callback)
        {
            auto context = getContext();
            DJV_PRIVATE_PTR();
            auto eventSystem = context->getSystemT<UI::EventSystem>();
            if (auto window = eventSystem->getCurrentWindow().lock())
            {
                if (!p.colorPickerDialog)
                {
                    p.colorPickerDialog = ColorPickerDialog::create(context);
                }
                p.colorPickerDialog->setTitle(title);
                p.colorPickerDialog->setColor(color);
                auto weak = std::weak_ptr<ColorPickerDialogSystem>(std::dynamic_pointer_cast<ColorPickerDialogSystem>(shared_from_this()));
                p.colorPickerDialog->setColorCallback(callback);
                p.colorPickerDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            if (auto parent = system->_p->colorPickerDialog->getParent().lock())
                            {
                                parent->removeChild(system->_p->colorPickerDialog);
                            }
                            system->_p->colorPickerDialog.reset();
                        }
                    });
                window->addChild(p.colorPickerDialog);
                p.colorPickerDialog->show();
            }
        }

    } // namespace UI
} // namespace djv


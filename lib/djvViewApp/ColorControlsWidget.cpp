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

#include <djvViewApp/ColorControlsWidget.h>

#include <djvUI/Action.h>
#include <djvUI/CheckBox.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TabWidget.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorControlsWidget::Private
        {
            AV::Render::ImageColor color;
            AV::Render::ImageLevels levels;
            AV::Render::ImageExposure exposure;
            bool exposureEnabled = false;
            float softClip = 0.F;

            std::map<std::string, std::shared_ptr<UI::FloatSlider> > colorSliders;
            std::shared_ptr<UI::FormLayout> colorLayout;

            std::map<std::string, std::shared_ptr<UI::FloatSlider> > levelsSliders;
            std::shared_ptr<UI::FormLayout> levelsLayout;
            
            std::shared_ptr<UI::CheckBox> exposureCheckBox;
            std::map<std::string, std::shared_ptr<UI::FloatSlider> > exposureSliders;
            std::shared_ptr<UI::FormLayout> exposureFormLayout;
            std::shared_ptr<UI::VerticalLayout> exposureLayout;

            std::shared_ptr<UI::FloatSlider> softClipSlider;
            std::shared_ptr<UI::VerticalLayout> softClipLayout;
                        
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::string, size_t> tabIDs;
            std::function<void(const AV::Render::ImageColor&)> colorCallback;
            std::function<void(const AV::Render::ImageLevels&)> levelsCallback;
            std::function<void(const AV::Render::ImageExposure&)> exposureCallback;
            std::function<void(bool)> exposureEnabledCallback;
            std::function<void(float)> softClipCallback;
        };

        void ColorControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ColorControlsWidget");

            p.colorSliders["Brightness"] = UI::FloatSlider::create(context);
            p.colorSliders["Brightness"]->setRange(FloatRange(0.F, 4.F));
            const AV::Render::ImageColor color;
            p.colorSliders["Brightness"]->setDefault(color.brightness);
            p.colorSliders["Contrast"] = UI::FloatSlider::create(context);
            p.colorSliders["Contrast"]->setRange(FloatRange(0.F, 4.F));
            p.colorSliders["Contrast"]->setDefault(color.contrast);
            p.colorSliders["Saturation"] = UI::FloatSlider::create(context);
            p.colorSliders["Saturation"]->setRange(FloatRange(0.F, 4.F));
            p.colorSliders["Saturation"]->setDefault(color.saturation);
            for (const auto& slider : p.colorSliders)
            {
                slider.second->setDefaultVisible(true);
            }    
            
            p.levelsSliders["InLow"] = UI::FloatSlider::create(context);
            const AV::Render::ImageLevels levels;
            p.levelsSliders["InLow"]->setDefault(levels.inLow);
            p.levelsSliders["InHigh"] = UI::FloatSlider::create(context);
            p.levelsSliders["InHigh"]->setDefault(levels.inHigh);
            p.levelsSliders["Gamma"] = UI::FloatSlider::create(context);
            p.levelsSliders["Gamma"]->setRange(FloatRange(0.F, 4.F));
            p.levelsSliders["Gamma"]->setDefault(levels.gamma);
            p.levelsSliders["OutLow"] = UI::FloatSlider::create(context);
            p.levelsSliders["OutLow"]->setDefault(levels.outLow);
            p.levelsSliders["OutHigh"] = UI::FloatSlider::create(context);
            p.levelsSliders["OutHigh"]->setDefault(levels.outHigh);
            for (const auto& slider : p.levelsSliders)
            {
                slider.second->setDefaultVisible(true);
            }    

            p.exposureCheckBox = UI::CheckBox::create(context);
            p.exposureSliders["Exposure"] = UI::FloatSlider::create(context);
            p.exposureSliders["Exposure"]->setRange(FloatRange(-10.F, 10.F));
            const AV::Render::ImageExposure exposure;
            p.exposureSliders["Exposure"]->setDefault(exposure.exposure);
            p.exposureSliders["Defog"] = UI::FloatSlider::create(context);
            p.exposureSliders["Defog"]->setRange(FloatRange(0.F, .01F));
            p.exposureSliders["Defog"]->setDefault(exposure.defog);
            p.exposureSliders["KneeLow"] = UI::FloatSlider::create(context);
            p.exposureSliders["KneeLow"]->setRange(FloatRange(-3.F, 3.F));
            p.exposureSliders["KneeLow"]->setDefault(exposure.kneeLow);
            p.exposureSliders["KneeHigh"] = UI::FloatSlider::create(context);
            p.exposureSliders["KneeHigh"]->setRange(FloatRange(3.5F, 7.5F));
            p.exposureSliders["KneeHigh"]->setDefault(exposure.kneeHigh);
            for (const auto& slider : p.exposureSliders)
            {
                slider.second->setDefaultVisible(true);
            }

            p.softClipSlider = UI::FloatSlider::create(context);
            p.softClipSlider->setDefaultVisible(true);

            p.colorLayout = UI::FormLayout::create(context);
            p.colorLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.colorLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            for (const auto& i : { "Brightness", "Contrast", "Saturation" })
            {
                p.colorLayout->addChild(p.colorSliders[i]);
            }
            p.levelsLayout = UI::FormLayout::create(context);
            p.levelsLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.levelsLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            for (const auto& i : { "InLow", "InHigh", "Gamma", "OutLow", "OutHigh" })
            {
                p.levelsLayout->addChild(p.levelsSliders[i]);
            }

            p.exposureLayout = UI::VerticalLayout::create(context);
            p.exposureLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.exposureLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.exposureLayout->addChild(p.exposureCheckBox);
            p.exposureFormLayout = UI::FormLayout::create(context);
            p.exposureFormLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            for (const auto& i : { "Exposure", "Defog", "KneeLow", "KneeHigh" })
            {
                p.exposureFormLayout->addChild(p.exposureSliders[i]);
            }
            p.exposureLayout->addChild(p.exposureFormLayout);
            
            p.softClipLayout = UI::VerticalLayout::create(context);
            p.softClipLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.softClipLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            p.softClipLayout->addChild(p.softClipSlider);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabIDs["Color"] = p.tabWidget->addTab(std::string(), p.colorLayout);
            p.tabIDs["Levels"] = p.tabWidget->addTab(std::string(), p.levelsLayout);
            p.tabIDs["Exposure"] = p.tabWidget->addTab(std::string(), p.exposureLayout);
            p.tabIDs["SoftClip"] = p.tabWidget->addTab(std::string(), p.softClipLayout);
            addChild(p.tabWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorControlsWidget>(std::dynamic_pointer_cast<ColorControlsWidget>(shared_from_this()));
            p.colorSliders["Brightness"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->color.brightness = value;
                        widget->_widgetUpdate();
                        if (widget->_p->colorCallback)
                        {
                            widget->_p->colorCallback(widget->_p->color);
                        }
                    }
                });
            
            p.colorSliders["Contrast"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->color.contrast = value;
                        widget->_widgetUpdate();
                        if (widget->_p->colorCallback)
                        {
                            widget->_p->colorCallback(widget->_p->color);
                        }
                    }
                });
            p.colorSliders["Saturation"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->color.saturation = value;
                        widget->_widgetUpdate();
                        if (widget->_p->colorCallback)
                        {
                            widget->_p->colorCallback(widget->_p->color);
                        }
                    }
                });

            p.levelsSliders["InLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->levels.inLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->levelsCallback)
                        {
                            widget->_p->levelsCallback(widget->_p->levels);
                        }
                    }
                });
            p.levelsSliders["InHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->levels.inHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->levelsCallback)
                        {
                            widget->_p->levelsCallback(widget->_p->levels);
                        }
                    }
                });
            p.levelsSliders["Gamma"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->levels.gamma = value;
                        widget->_widgetUpdate();
                        if (widget->_p->levelsCallback)
                        {
                            widget->_p->levelsCallback(widget->_p->levels);
                        }
                    }
                });
            p.levelsSliders["OutLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->levels.outLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->levelsCallback)
                        {
                            widget->_p->levelsCallback(widget->_p->levels);
                        }
                    }
                });
            p.levelsSliders["OutHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->levels.outHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->levelsCallback)
                        {
                            widget->_p->levelsCallback(widget->_p->levels);
                        }
                    }
                });
              
            p.exposureCheckBox->setCheckedCallback(
                [weak](bool value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->exposureEnabled = value;
                        if (widget->_p->exposureEnabledCallback)
                        {
                            widget->_p->exposureEnabledCallback(widget->_p->exposureEnabled);
                        }
                    }
                });
              
            p.exposureSliders["Exposure"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->exposure.exposure = value;
                        widget->_widgetUpdate();
                        if (widget->_p->exposureCallback)
                        {
                            widget->_p->exposureCallback(widget->_p->exposure);
                        }
                    }
                });
            p.exposureSliders["Defog"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->exposure.defog = value;
                        widget->_widgetUpdate();
                        if (widget->_p->exposureCallback)
                        {
                            widget->_p->exposureCallback(widget->_p->exposure);
                        }
                    }
                });
            p.exposureSliders["KneeLow"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->exposure.kneeLow = value;
                        widget->_widgetUpdate();
                        if (widget->_p->exposureCallback)
                        {
                            widget->_p->exposureCallback(widget->_p->exposure);
                        }
                    }
                });
            p.exposureSliders["KneeHigh"]->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->exposure.kneeHigh = value;
                        widget->_widgetUpdate();
                        if (widget->_p->exposureCallback)
                        {
                            widget->_p->exposureCallback(widget->_p->exposure);
                        }
                    }
                });
            
            p.softClipSlider->setValueCallback(
                [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->softClip = value;
                        widget->_widgetUpdate();
                        if (widget->_p->softClipCallback)
                        {
                            widget->_p->softClipCallback(widget->_p->softClip);
                        }
                    }
                });
        }

        ColorControlsWidget::ColorControlsWidget() :
            _p(new Private)
        {}

        ColorControlsWidget::~ColorControlsWidget()
        {}

        std::shared_ptr<ColorControlsWidget> ColorControlsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorControlsWidget>(new ColorControlsWidget);
            out->_init(context);
            return out;
        }

        void ColorControlsWidget::setColor(const AV::Render::ImageColor& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.color)
                return;
            p.color = value;
            _widgetUpdate();
        }
        
        void ColorControlsWidget::setLevels(const AV::Render::ImageLevels& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.levels)
                return;
            p.levels = value;
            _widgetUpdate();
        }

        void ColorControlsWidget::setExposure(const AV::Render::ImageExposure& value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.exposure)
                return;
            p.exposure = value;
            _widgetUpdate();
        }
        
        void ColorControlsWidget::setExposureEnabled(bool value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.exposureEnabled)
                return;
            p.exposureEnabled = value;
            _widgetUpdate();
        }

        void ColorControlsWidget::setSoftClip(float value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.softClip)
                return;
            p.softClip = value;
            _widgetUpdate();
        }
        
        int ColorControlsWidget::getCurrentTab() const
        {
            return _p->tabWidget->getCurrentTab();
        }

        void ColorControlsWidget::setCurrentTab(int value)
        {
            _p->tabWidget->setCurrentTab(value);
        }
        
        void ColorControlsWidget::setColorCallback(const std::function<void(const AV::Render::ImageColor&)>& callback)
        {
            _p->colorCallback = callback;
        }
        
        void ColorControlsWidget::setLevelsCallback(const std::function<void(const AV::Render::ImageLevels&)>& callback)
        {
            _p->levelsCallback = callback;
        }
        
        void ColorControlsWidget::setExposureCallback(const std::function<void(const AV::Render::ImageExposure&)>& callback)
        {
            _p->exposureCallback = callback;
        }

        void ColorControlsWidget::setExposureEnabledCallback(const std::function<void(bool)>& callback)
        {
            _p->exposureEnabledCallback = callback;
        }

        void ColorControlsWidget::setSoftClipCallback(const std::function<void(float)>& callback)
        {
            _p->softClipCallback = callback;
        }

        void ColorControlsWidget::_textUpdateEvent(Event::TextUpdate & event)
        {
            MDIWidget::_textUpdateEvent(event);

            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Color Controls")));
            
            p.colorLayout->setText(p.colorSliders["Brightness"], _getText(DJV_TEXT("Brightness")) + ":");
            p.colorLayout->setText(p.colorSliders["Contrast"], _getText(DJV_TEXT("Contrast")) + ":");
            p.colorLayout->setText(p.colorSliders["Saturation"], _getText(DJV_TEXT("Saturation")) + ":");

            p.levelsLayout->setText(p.levelsSliders["InLow"], _getText(DJV_TEXT("In Low")) + ":");
            p.levelsLayout->setText(p.levelsSliders["InHigh"], _getText(DJV_TEXT("In High")) + ":");
            p.levelsLayout->setText(p.levelsSliders["Gamma"], _getText(DJV_TEXT("Gamma")) + ":");
            p.levelsLayout->setText(p.levelsSliders["OutLow"], _getText(DJV_TEXT("Out Low")) + ":");
            p.levelsLayout->setText(p.levelsSliders["OutHigh"], _getText(DJV_TEXT("Out High")) + ":");

            p.exposureCheckBox->setText(_getText(DJV_TEXT("Enabled")));
            p.exposureFormLayout->setText(p.exposureSliders["Exposure"], _getText(DJV_TEXT("Exposure")) + ":");
            p.exposureFormLayout->setText(p.exposureSliders["Defog"], _getText(DJV_TEXT("Defog")) + ":");
            p.exposureFormLayout->setText(p.exposureSliders["KneeLow"], _getText(DJV_TEXT("Knee Low")) + ":");
            p.exposureFormLayout->setText(p.exposureSliders["KneeHigh"], _getText(DJV_TEXT("Knee High")) + ":");

            p.tabWidget->setText(p.tabIDs["Color"], _getText(DJV_TEXT("Color")));
            p.tabWidget->setText(p.tabIDs["Levels"], _getText(DJV_TEXT("Levels")));
            p.tabWidget->setText(p.tabIDs["Exposure"], _getText(DJV_TEXT("Exposure")));
            p.tabWidget->setText(p.tabIDs["SoftClip"], _getText(DJV_TEXT("Soft Clip")));
        }

        void ColorControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            
            p.colorSliders["Brightness"]->setValue(p.color.brightness);
            p.colorSliders["Contrast"]->setValue(p.color.contrast);
            p.colorSliders["Saturation"]->setValue(p.color.saturation);
            
            p.levelsSliders["InLow"]->setValue(p.levels.inLow);
            p.levelsSliders["InHigh"]->setValue(p.levels.inHigh);
            p.levelsSliders["Gamma"]->setValue(p.levels.gamma);
            p.levelsSliders["OutLow"]->setValue(p.levels.outLow);
            p.levelsSliders["OutHigh"]->setValue(p.levels.outHigh);
            
            p.exposureCheckBox->setChecked(p.exposureEnabled);
            p.exposureSliders["Exposure"]->setValue(p.exposure.exposure);
            p.exposureSliders["Defog"]->setValue(p.exposure.defog);
            p.exposureSliders["KneeLow"]->setValue(p.exposure.kneeLow);
            p.exposureSliders["KneeHigh"]->setValue(p.exposure.kneeHigh);
            
            p.softClipSlider->setValue(p.softClip);
        }

    } // namespace ViewApp
} // namespace djv


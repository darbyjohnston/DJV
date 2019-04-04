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

#include <djvViewLib/ColorPickerTool.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Action.h>
#include <djvUI/ColorSwatch.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TabWidget.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ColorPickerTool::Private
        {
            AV::Image::Color color = AV::Image::Color(0.f, 0.f, 0.f);
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ColorSwatch> colorSwatch;
            std::shared_ptr<UI::RGBColorSliders> rgbSliders;
            std::shared_ptr<UI::HSVColorSliders> hsvSliders;
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::string, size_t> tabText;
            std::shared_ptr<UI::IntSlider> sampleSizeSlider;
            std::shared_ptr<UI::ColorTypeWidget> typeWidget;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<UI::PopupMenu> popupMenu;
        };

        void ColorPickerTool::_init(Context * context)
        {
            ITool::_init(context);

            DJV_PRIVATE_PTR();
            p.actions["Lock"] = UI::Action::create();
            p.actions["Lock"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["ColorProfile"] = UI::Action::create();
            p.actions["ColorProfile"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["DisplayProfile"] = UI::Action::create();
            p.actions["DisplayProfile"]->setButtonType(UI::ButtonType::Toggle);

            p.colorSwatch = UI::ColorSwatch::create(context);

            p.rgbSliders = UI::RGBColorSliders::create(context);
            p.rgbSliders->setMargin(UI::MetricsRole::MarginSmall);
            p.hsvSliders = UI::HSVColorSliders::create(context);
            p.hsvSliders->setMargin(UI::MetricsRole::MarginSmall);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setPointerEnabled(true);
            p.tabText["RGB"] = p.tabWidget->addTab(std::string(), p.rgbSliders);
            p.tabText["HSV"] = p.tabWidget->addTab(std::string(), p.hsvSliders);

            p.sampleSizeSlider = UI::IntSlider::create(context);
            p.sampleSizeSlider->setRange(IntRange(1, 100));
            p.sampleSizeSlider->setMargin(UI::MetricsRole::MarginSmall);

            p.typeWidget = UI::ColorTypeWidget::create(context);

            p.menu = UI::Menu::create(context);
            p.menu->setIcon("djvIconMenuPopup");
            p.menu->addAction(p.actions["Lock"]);
            p.menu->addAction(p.actions["ColorProfile"]);
            p.menu->addAction(p.actions["DisplayProfile"]);
            p.popupMenu = UI::PopupMenu::create(context);
            p.popupMenu->setMenu(p.menu);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.colorSwatch);
            p.layout->setStretch(p.colorSwatch, UI::RowStretch::Expand);
            p.layout->addChild(p.tabWidget);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.sampleSizeSlider);
            hLayout->setStretch(p.sampleSizeSlider, UI::RowStretch::Expand);
            hLayout->addChild(p.typeWidget);
            hLayout->addChild(p.popupMenu);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            _colorUpdate();

            auto weak = std::weak_ptr<ColorPickerTool>(std::dynamic_pointer_cast<ColorPickerTool>(shared_from_this()));
            p.rgbSliders->setColorCallback(
                [weak](const AV::Image::Color & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->color = value;
                    widget->_colorUpdate();
                }
            });

            p.hsvSliders->setColorCallback(
                [weak](const AV::Image::Color & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->color = value;
                    widget->_colorUpdate();
                }
            });

            p.typeWidget->setTypeCallback(
                [weak](AV::Image::Type value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->color = widget->_p->color.convert(value);
                    widget->_colorUpdate();
                }
            });
        }

        ColorPickerTool::ColorPickerTool() :
            _p(new Private)
        {}

        ColorPickerTool::~ColorPickerTool()
        {}

        std::shared_ptr<ColorPickerTool> ColorPickerTool::create(Context * context)
        {
            auto out = std::shared_ptr<ColorPickerTool>(new ColorPickerTool);
            out->_init(context);
            return out;
        }

        void ColorPickerTool::_localeEvent(Event::Locale & event)
        {
            ITool::_localeEvent(event);

            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Color Picker")));

            p.actions["Lock"]->setTitle(_getText(DJV_TEXT("Lock Color Type")));
            p.actions["Lock"]->setTooltip(_getText(DJV_TEXT("Color picker lock color type tooltip")));
            p.actions["ColorProfile"]->setTitle(_getText(DJV_TEXT("Enable Color Profile")));
            p.actions["ColorProfile"]->setTooltip(_getText(DJV_TEXT("Color picker enable color profile tooltip")));
            p.actions["DisplayProfile"]->setTitle(_getText(DJV_TEXT("Enable Display Profile")));
            p.actions["DisplayProfile"]->setTooltip(_getText(DJV_TEXT("Color picker enable display profile tooltip")));

            p.tabWidget->setText(p.tabText["RGB"], _getText(DJV_TEXT("RGB")));
            p.tabWidget->setText(p.tabText["HSV"], _getText(DJV_TEXT("HSV")));

            p.sampleSizeSlider->setTooltip(_getText(DJV_TEXT("Color picker sample size tooltip")));
        }

        void ColorPickerTool::_colorUpdate()
        {
            DJV_PRIVATE_PTR();
            p.colorSwatch->setColor(p.color);
            p.rgbSliders->setColor(p.color);
            p.hsvSliders->setColor(p.color);
            p.typeWidget->setType(p.color.getType());
        }

    } // namespace ViewLib
} // namespace djv


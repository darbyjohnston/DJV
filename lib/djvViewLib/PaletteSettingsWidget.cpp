//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/ColorPaletteSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/PushButton.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/UISystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct ColorPaletteSettingsWidget::Private
        {
            std::vector<std::shared_ptr<UI::Style::Style> > customStyles;
            std::vector<std::shared_ptr<UI::Button::Push> > buttons;
            std::shared_ptr<UI::Button::Group> buttonGroup;
            std::shared_ptr<UI::Layout::Flow> layout;
            std::map<int, std::string> indexToPalette;
            std::map<std::shared_ptr<UI::Button::Push>, std::string> buttonToPalette;
            std::map<std::string, int> paletteToIndex;
            std::shared_ptr<MapObserver<std::string, UI::Style::Palette> > palettesObserver;
            std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
        };

        void ColorPaletteSettingsWidget::_init(Context * context)
        {
            GroupBox::_init(context);

            DJV_PRIVATE_PTR();
            p.buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);                    

            p.layout = UI::Layout::Flow::create(context);
            addWidget(p.layout);

            auto weak = std::weak_ptr<ColorPaletteSettingsWidget>(std::dynamic_pointer_cast<ColorPaletteSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
                    {
                        const auto i = widget->_p->indexToPalette.find(value);
                        if (i != widget->_p->indexToPalette.end())
                        {
                            uiSystem->getStyleSettings()->setCurrentPalette(i->second);
                        }
                    }
                }
            });

            if (auto uiSystem = context->getSystemT<UI::UISystem>().lock())
            {
                p.palettesObserver = MapObserver<std::string, UI::Style::Palette>::create(
                    uiSystem->getStyleSettings()->observePalettes(),
                    [weak, context](const std::map<std::string, UI::Style::Palette > & value)
                {
                    if (auto widget = weak.lock())
                    {
                        int currentItem = widget->_p->buttonGroup->getChecked();
                        if (-1 == currentItem && value.size())
                        {
                            currentItem = 0;
                        }
                        widget->_p->buttons.clear();
                        widget->_p->buttonGroup->clearButtons();
                        widget->_p->layout->clearWidgets();
                        widget->_p->indexToPalette.clear();
                        widget->_p->buttonToPalette.clear();
                        widget->_p->paletteToIndex.clear();
                        int j = 0;
                        for (const auto & i : value)
                        {
                            auto customStyle = UI::Style::Style::create(context);
                            customStyle->setPalette(i.second);
                            widget->_p->customStyles.push_back(customStyle);
                            auto button = UI::Button::Push::create(context);
                            button->setBackgroundRole(UI::Style::ColorRole::Background);
                            button->setStyle(customStyle);
                            widget->_p->buttons.push_back(button);
                            widget->_p->buttonGroup->addButton(button);
                            widget->_p->layout->addWidget(button);
                            widget->_p->indexToPalette[j] = i.first;
                            widget->_p->buttonToPalette[button] = i.first;
                            widget->_p->paletteToIndex[i.first] = j;
                            ++j;
                        }
                        widget->_p->buttonGroup->setChecked(currentItem);
                        widget->_buttonTextUpdate();
                    }
                });
                p.currentPaletteObserver = ValueObserver<std::string>::create(
                    uiSystem->getStyleSettings()->observeCurrentPaletteName(),
                    [weak](const std::string & value)
                {
                    if (auto widget = weak.lock())
                    {
                        const auto i = widget->_p->paletteToIndex.find(value);
                        if (i != widget->_p->paletteToIndex.end())
                        {
                            widget->_p->buttonGroup->setChecked(static_cast<int>(i->second));
                        }
                    }
                });
            }
        }

        ColorPaletteSettingsWidget::ColorPaletteSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ColorPaletteSettingsWidget> ColorPaletteSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<ColorPaletteSettingsWidget>(new ColorPaletteSettingsWidget);
            out->_init(context);
            return out;
        }

        void ColorPaletteSettingsWidget::_localeEvent(Event::Locale &)
        {
            setText(_getText(DJV_TEXT("djv::ViewLib", "Palette")));
            _buttonTextUpdate();
        }

        void ColorPaletteSettingsWidget::_styleEvent(Event::Style &)
        {
            if (auto style = _getStyle().lock())
            {
                for (auto i : _p->customStyles)
                {
                    i->setMetrics(style->getMetrics());
                    i->setFont(style->getFont());
                    i->setDPI(style->getDPI());
                }
            }
        }

        void ColorPaletteSettingsWidget::_buttonTextUpdate()
        {
            DJV_PRIVATE_PTR();
            for (auto i : p.buttons)
            {
                const auto j = p.buttonToPalette.find(i);
                if (j != p.buttonToPalette.end())
                {
                    i->setText(_getText(j->second));
                }
            }
        }
        
    } // namespace ViewLib
} // namespace djv


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

#include <djvUIComponents/PaletteSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/FlatButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PaletteSettingsWidget::Private
        {
            std::vector<std::shared_ptr<FlatButton> > buttons;
            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> layout;
            std::map<int, std::string> indexToPalette;
            std::map<std::shared_ptr<FlatButton>, std::string> buttonToPalette;
            std::map<std::string, int> paletteToIndex;
            std::shared_ptr<MapObserver<std::string, Style::Palette> > palettesObserver;
            std::shared_ptr<ValueObserver<std::string> > currentPaletteObserver;
        };

        void PaletteSettingsWidget::_init(Context * context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            p.buttonGroup = ButtonGroup::create(ButtonType::Radio);

            p.layout = VerticalLayout::create(context);
            p.layout->setSpacing(MetricsRole::None);
            addChild(p.layout);

            auto weak = std::weak_ptr<PaletteSettingsWidget>(std::dynamic_pointer_cast<PaletteSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
                    {
                        if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                        {
                            const auto i = widget->_p->indexToPalette.find(value);
                            if (i != widget->_p->indexToPalette.end())
                            {
                                styleSettings->setCurrentPalette(i->second);
                            }
                        }
                    }
                }
            });

            if (auto settingsSystem = context->getSystemT<Settings::System>().lock())
            {
                if (auto styleSettings = settingsSystem->getSettingsT<Settings::Style>())
                {
                    p.palettesObserver = MapObserver<std::string, Style::Palette>::create(
                        styleSettings->observePalettes(),
                        [weak, context](const std::map<std::string, Style::Palette > & value)
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
                            widget->_p->layout->clearChildren();
                            widget->_p->indexToPalette.clear();
                            widget->_p->buttonToPalette.clear();
                            widget->_p->paletteToIndex.clear();
                            int j = 0;
                            for (const auto & i : value)
                            {
                                auto button = FlatButton::create(context);
                                widget->_p->buttons.push_back(button);
                                widget->_p->buttonGroup->addButton(button);
                                widget->_p->layout->addChild(button);
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
                        styleSettings->observeCurrentPaletteName(),
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
        }

        PaletteSettingsWidget::PaletteSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<PaletteSettingsWidget> PaletteSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<PaletteSettingsWidget>(new PaletteSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string PaletteSettingsWidget::getName() const
        {
            return DJV_TEXT("Palette");
        }

        std::string PaletteSettingsWidget::getSortKey() const
        {
            return "A";
        }

        void PaletteSettingsWidget::_localeEvent(Event::Locale & event)
        {
            ISettingsWidget::_localeEvent(event);
            _buttonTextUpdate();
        }

        void PaletteSettingsWidget::_buttonTextUpdate()
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
        
    } // namespace UI
} // namespace djv


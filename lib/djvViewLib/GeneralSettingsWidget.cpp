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

#include <djvViewLib/GeneralSettingsWidget.h>

#include <djvViewLib/DisplaySettingsWidget.h>
#include <djvViewLib/LanguageSettingsWidget.h>
#include <djvViewLib/PaletteSettingsWidget.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct GeneralSettingsWidget::Private
        {
            std::shared_ptr<UI::Button::List> displayButton;
            std::shared_ptr<UI::Button::List> languageButton;
            std::shared_ptr<UI::Button::List> paletteButton;
            std::shared_ptr<UI::Button::Group> buttonGroup;
            std::shared_ptr<UI::Layout::Horizontal> layout;
        };

        void GeneralSettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.displayButton = UI::Button::List::create(context);
            p.languageButton = UI::Button::List::create(context);
            p.paletteButton = UI::Button::List::create(context);

            p.buttonGroup = UI::Button::Group::create(UI::ButtonType::Radio);
            p.buttonGroup->addButton(p.displayButton);
            p.buttonGroup->addButton(p.languageButton);
            p.buttonGroup->addButton(p.paletteButton);

            auto buttonLayout = UI::Layout::Vertical::create(context);
            buttonLayout->setSpacing(UI::Style::MetricsRole::None);
            buttonLayout->addWidget(p.displayButton);
            buttonLayout->addWidget(p.languageButton);
            buttonLayout->addWidget(p.paletteButton);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(buttonLayout);

            auto soloLayout = UI::Layout::Solo::create(context);
            soloLayout->addWidget(DisplaySettingsWidget::create(context));
            soloLayout->addWidget(LanguageSettingsWidget::create(context));
            soloLayout->addWidget(PaletteSettingsWidget::create(context));

            p.layout = UI::Layout::Horizontal::create(context);
            p.layout->setSpacing(UI::Style::MetricsRole::None);
            p.layout->addWidget(scrollWidget);
            p.layout->addWidget(soloLayout, UI::Layout::RowStretch::Expand);
            p.layout->setParent(shared_from_this());

            auto weak = std::weak_ptr<GeneralSettingsWidget>(std::dynamic_pointer_cast<GeneralSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [soloLayout](int value)
            {
                soloLayout->setCurrentIndex(value);
            });
        }

        GeneralSettingsWidget::GeneralSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<GeneralSettingsWidget> GeneralSettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<GeneralSettingsWidget>(new GeneralSettingsWidget);
            out->_init(context);
            return out;
        }

        float GeneralSettingsWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void GeneralSettingsWidget::_preLayoutEvent(Event::PreLayout &)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void GeneralSettingsWidget::_layoutEvent(Event::Layout &)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void GeneralSettingsWidget::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            p.displayButton->setText(_getText(DJV_TEXT("djv::ViewLib", "Display")));
            p.languageButton->setText(_getText(DJV_TEXT("djv::ViewLib", "Language")));
            p.paletteButton->setText(_getText(DJV_TEXT("djv::ViewLib", "Palette")));
        }

    } // namespace ViewLib
} // namespace djv


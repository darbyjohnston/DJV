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

#include <djvUI/GroupBox.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct GeneralSettingsWidget::Private
        {
            std::shared_ptr<UI::GroupBox> displayGroupBox;
            std::shared_ptr<UI::GroupBox> languageGroupBox;
            std::shared_ptr<UI::GroupBox> paletteGroupBox;
            std::shared_ptr<UI::VerticalLayout> layout;
        };

        void GeneralSettingsWidget::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.displayGroupBox = UI::GroupBox::create(context);
            p.displayGroupBox->addWidget(DisplaySettingsWidget::create(context));
            p.languageGroupBox = UI::GroupBox::create(context);
            p.languageGroupBox->addWidget(LanguageSettingsWidget::create(context));
            p.paletteGroupBox = UI::GroupBox::create(context);
            p.paletteGroupBox->addWidget(PaletteSettingsWidget::create(context));

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setMargin(UI::Style::MetricsRole::MarginLarge);
            p.layout->setSpacing(UI::Style::MetricsRole::SpacingLarge);
            p.layout->addWidget(p.displayGroupBox);
            p.layout->addWidget(p.languageGroupBox);
            p.layout->addWidget(p.paletteGroupBox);
            p.layout->setParent(shared_from_this());
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
            p.displayGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib::GeneralSettingsWidget", "Display Size")));
            p.languageGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib::GeneralSettingsWidget", "Language")));
            p.paletteGroupBox->setText(_getText(DJV_TEXT("djv::ViewLib::GeneralSettingsWidget", "Color Palette")));
        }

    } // namespace ViewLib
} // namespace djv


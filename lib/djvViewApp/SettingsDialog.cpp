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

#include <djvViewApp/SettingsDialog.h>

#include <djvViewApp/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Splitter.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsDialog::Private
        {
            std::map<std::string, std::vector<std::shared_ptr<UI::ISettingsWidget> > > widgets;
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::ListButton> > buttons;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::Bellows> > bellows;
        };

        void SettingsDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();

            for (auto system : context->getSystemsT<IViewSystem>())
            {
                for (auto widget : system->createSettingsWidgets())
                {
                    p.widgets[widget->getSettingsSortKey()].push_back(widget);
                }
            }

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            auto buttonLayout = UI::VerticalLayout::create(context);
            buttonLayout->setSpacing(UI::MetricsRole::None);
            auto soloLayout = UI::SoloLayout::create(context);
            soloLayout->setSizeForAll(false);
            for (const auto & i : p.widgets)
            {
                if (i.second.size())
                {
                    auto vLayout = UI::VerticalLayout::create(context);
                    vLayout->setSpacing(UI::MetricsRole::None);
                    for (const auto& j : i.second)
                    {
                        auto button = UI::ListButton::create(context);
                        button->setInsideMargin(UI::MetricsRole::Margin);
                        p.buttons[j] = button;
                        p.buttonGroup->addButton(button);
                        vLayout->addChild(button);
                        soloLayout->addChild(j);
                    }
                    auto bellows = UI::Bellows::create(context);
                    bellows->addChild(vLayout);
                    p.bellows[i.second[0]] = bellows;
                    buttonLayout->addChild(bellows);
                }
            }

            auto splitter = UI::Layout::Splitter::create(UI::Orientation::Horizontal, context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(buttonLayout);
            splitter->addChild(scrollWidget);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(soloLayout);
            splitter->addChild(scrollWidget);
            splitter->setSplit({ .2f, 1.f });
            addChild(splitter);
            setStretch(splitter, UI::RowStretch::Expand);

            auto weak = std::weak_ptr<SettingsDialog>(std::dynamic_pointer_cast<SettingsDialog>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [soloLayout](int value)
            {
                soloLayout->setCurrentIndex(value);
            });
        }

        SettingsDialog::SettingsDialog() :
            _p(new Private)
        {}

        SettingsDialog::~SettingsDialog()
        {}

        std::shared_ptr<SettingsDialog> SettingsDialog::create(Context * context)
        {
            auto out = std::shared_ptr<SettingsDialog>(new SettingsDialog);
            out->_init(context);
            return out;
        }

        void SettingsDialog::_localeEvent(Event::Locale & event)
        {
            IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Settings")));
            for (const auto& i : p.bellows)
            {
                i.second->setText(_getText(i.first->getSettingsGroup()));
            }
            for (const auto& i : p.buttons)
            {
                i.second->setText(_getText(i.first->getSettingsName()));
            }
        }

    } // namespace ViewApp
} // namespace djv


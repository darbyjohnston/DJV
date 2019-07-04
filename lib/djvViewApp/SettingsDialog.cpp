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

#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TabWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsDialog::Private
        {
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::string, std::vector<std::shared_ptr<UI::ISettingsWidget> > > widgets;
            std::map<size_t, std::shared_ptr<UI::ISettingsWidget> > tabs;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::GroupBox> > groupBoxes;
        };

        void SettingsDialog::_init(Context* context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SettingsDialog");

            for (auto system : context->getSystemsT<IViewSystem>())
            {
                for (auto widget : system->createSettingsWidgets())
                {
                    p.widgets[widget->getSettingsSortKey()].push_back(widget);
                }
            }

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });

            for (const auto& i : p.widgets)
            {
                if (i.second.size())
                {
                    auto vLayout = UI::VerticalLayout::create(context);
                    vLayout->setMargin(UI::MetricsRole::MarginLarge);
                    vLayout->setSpacing(UI::MetricsRole::SpacingLarge);
                    for (const auto& j : i.second)
                    {
                        auto groupBox = UI::GroupBox::create(context);
                        groupBox->addChild(j);
                        p.groupBoxes[j] = groupBox;
                        vLayout->addChild(groupBox);
                    }

                    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                    scrollWidget->setBorder(false);
                    scrollWidget->setShadowOverlay({ UI::Side::Top });
                    scrollWidget->addChild(vLayout);

                    size_t id = p.tabWidget->addTab(i.second[0]->getSettingsGroup(), scrollWidget);
                    p.tabs[id] = i.second[0];
                }
            }
            
            addChild(p.tabWidget);
            setStretch(p.tabWidget, UI::RowStretch::Expand);
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
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Settings")));
            for (const auto& i : p.tabs)
            {
                p.tabWidget->setText(i.first, _getText(i.second->getSettingsGroup()));
            }
            for (const auto& i : p.groupBoxes)
            {
                i.second->setText(_getText(i.first->getSettingsName()));
            }
        }

    } // namespace ViewApp
} // namespace djv


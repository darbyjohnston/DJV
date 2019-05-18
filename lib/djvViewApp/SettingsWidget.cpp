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

#include <djvViewApp/SettingsWidget.h>

#include <djvViewApp/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/ButtonGroup.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::map<std::string, std::vector<std::shared_ptr<UI::ISettingsWidget> > > widgets;
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::ListButton> > buttons;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::Bellows> > bellows;
            std::shared_ptr<UI::SoloLayout> soloLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(void)> closeCallback;
        };

        void SettingsWidget::_init(Context* context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SettingsWidget");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setFontSizeRole(UI::MetricsRole::FontHeader);
            p.titleLabel->setMargin(UI::MetricsRole::Margin);
            auto closeButton = UI::ToolButton::create(context);
            closeButton->setIcon("djvIconClose");

            for (auto system : context->getSystemsT<IViewSystem>())
            {
                for (auto widget : system->createSettingsWidgets())
                {
                    p.widgets[widget->getSettingsSortKey()].push_back(widget);
                }
            }

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Push);
            auto buttonLayout = UI::VerticalLayout::create(context);
            buttonLayout->setSpacing(UI::MetricsRole::None);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->setBackgroundRole(UI::ColorRole::BackgroundHeader);
            hLayout->addChild(p.titleLabel);
            hLayout->setStretch(p.titleLabel, UI::RowStretch::Expand);
            hLayout->addChild(closeButton);
            vLayout->addChild(hLayout);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(buttonLayout);
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);

            p.soloLayout = UI::SoloLayout::create(context);
            p.soloLayout->setSoloMinimumSize(UI::Layout::SoloMinimumSize::Horizontal);
            p.soloLayout->addChild(vLayout);

            for (const auto& i : p.widgets)
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
                        p.soloLayout->addChild(j);
                    }
                    auto bellows = UI::Bellows::create(context);
                    bellows->addChild(vLayout);
                    p.bellows[i.second[0]] = bellows;
                    buttonLayout->addChild(bellows);
                }
            }
            
            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.soloLayout);
            p.layout->setStretch(p.soloLayout, UI::RowStretch::Expand);
            addChild(p.layout);

            auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
            closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->closeCallback)
                    {
                        widget->_p->closeCallback();
                    }
                }
            });

            p.buttonGroup->setPushCallback(
                [weak](int value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->soloLayout->setCurrentIndex(1 + value, UI::Side::Right);
                }
            });

            for (const auto& i : p.widgets)
            {
                for (const auto& j : i.second)
                {
                    j->setBackCallback(
                        [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->soloLayout->setCurrentIndex(0);
                        }
                    });
                    j->setCloseCallback(
                        [weak]
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->closeCallback)
                            {
                                widget->_p->closeCallback();
                            }
                        }
                    });
                }
            }
        }

        SettingsWidget::SettingsWidget() :
            _p(new Private)
        {}

        SettingsWidget::~SettingsWidget()
        {}

        std::shared_ptr<SettingsWidget> SettingsWidget::create(Context * context)
        {
            auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
            out->_init(context);
            return out;
        }

        void SettingsWidget::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void SettingsWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void SettingsWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void SettingsWidget::_localeEvent(Event::Locale & event)
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(_getText(DJV_TEXT("Settings")));
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


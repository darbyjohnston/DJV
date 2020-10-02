// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/SettingsWidget.h>

#include <djvViewApp/ApplicationSettings.h>
#include <djvViewApp/IViewSystem.h>

#include <djvUIComponents/SettingsIWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsWidget::Private
        {
            std::map<std::string, std::vector<std::shared_ptr<UIComponents::Settings::IWidget> > > widgets;
            std::map<std::shared_ptr<UIComponents::Settings::IWidget>, std::shared_ptr<UI::Bellows> > bellows;
            std::map<std::shared_ptr<UIComponents::Settings::IWidget>, std::shared_ptr<UI::Text::Label> > labels;
            std::shared_ptr<UI::Text::LabelSizeGroup> sizeGroup;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
        };

        void SettingsWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::SettingsWidget");

            p.sizeGroup = UI::Text::LabelSizeGroup::create();

            for (auto system : context->getSystemsT<IViewSystem>())
            {
                for (auto widget : system->createSettingsWidgets())
                {
                    widget->setLabelSizeGroup(p.sizeGroup);
                    p.widgets[widget->getSettingsSortKey()].push_back(widget);
                }
            };

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto appSettings = settingsSystem->getSettingsT<ApplicationSettings>();
            const auto& settingsBellows = appSettings->observeSettingsBellows()->get();
            auto contextWeak = std::weak_ptr<System::Context>(context);
            const size_t widgetsSize = p.widgets.size();
            for (const auto& i : p.widgets)
            {
                if (i.second.size())
                {
                    auto layout = UI::VerticalLayout::create(context);
                    layout->setMargin(UI::MetricsRole::MarginSmall);
                    const size_t size = i.second.size();
                    for (size_t j = 0; j < size; ++j)
                    {
                        const auto& widget = i.second[j];
                        if (!widget->getSettingsName().empty())
                        {
                            auto label = UI::Text::Label::create(context);
                            label->setFontFace("Bold");
                            label->setMargin(UI::MetricsRole::MarginSmall);
                            label->setHAlign(UI::HAlign::Left);
                            p.labels[widget] = label;

                            auto vLayout = UI::VerticalLayout::create(context);
                            vLayout->setSpacing(UI::MetricsRole::SpacingSmall);
                            vLayout->addChild(label);
                            vLayout->addChild(widget);
                            layout->addChild(vLayout);
                        }
                        else
                        {
                            layout->addChild(widget);
                        }
                        if (j < size - 1)
                        {
                            layout->addSeparator();
                        }
                    }

                    auto bellows = UI::Bellows::create(context);
                    bellows->addChild(layout);
                    p.layout->addChild(bellows);
                    p.bellows[i.second[0]] = bellows;

                    const std::string group = i.second[0]->getSettingsGroup();
                    const auto k = settingsBellows.find(group);
                    bellows->setOpen(k != settingsBellows.end() ? k->second : false, false);

                    bellows->setOpenCallback(
                        [group, contextWeak](bool value)
                        {
                            if (auto context = contextWeak.lock())
                            {
                                auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                                auto appSettings = settingsSystem->getSettingsT<ApplicationSettings>();
                                auto settingsBellows = appSettings->observeSettingsBellows()->get();
                                settingsBellows[group] = value;
                                appSettings->setSettingsBellows(settingsBellows);
                            }
                        });
                }
            }

            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setBorder(false);
            p.scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);

            _textUpdate();
        }

        SettingsWidget::SettingsWidget() :
            _p(new Private)
        {}

        SettingsWidget::~SettingsWidget()
        {}

        std::shared_ptr<SettingsWidget> SettingsWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
            out->_init(context);
            return out;
        }

        void SettingsWidget::_initLayoutEvent(System::Event::InitLayout& event)
        {
            Widget::_initLayoutEvent(event);
            _p->sizeGroup->calcMinimumSize();
        }

        void SettingsWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->scrollWidget->getMinimumSize());
        }

        void SettingsWidget::_layoutEvent(System::Event::Layout& event)
        {
            _p->scrollWidget->setGeometry(getGeometry());
        }

        void SettingsWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                _textUpdate();
            }
        }

        void SettingsWidget::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.bellows)
            {
                i.second->setText(_getText(i.first->getSettingsGroup()));
            }
            for (const auto& i : p.labels)
            {
                i.second->setText(_getText(i.first->getSettingsName()));
            }
        }

    } // namespace ViewApp
} // namespace djv


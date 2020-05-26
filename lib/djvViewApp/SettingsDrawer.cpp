// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/SettingsDrawer.h>

#include <djvViewApp/ApplicationSettings.h>
#include <djvViewApp/IViewSystem.h>

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/Bellows.h>
#include <djvUI/GroupBox.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct SettingsDrawer::Private
        {
            std::map<std::string, std::vector<std::shared_ptr<UI::ISettingsWidget> > > widgets;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::Bellows> > bellows;
            std::map<std::shared_ptr<UI::ISettingsWidget>, std::shared_ptr<UI::GroupBox> > groupBoxes;
            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
        };

        void SettingsDrawer::_init(const std::shared_ptr<Context>& context)
        {
            Drawer::_init(UI::Side::Right, context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::SettingsDrawer");

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);

            p.scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            p.scrollWidget->addChild(p.layout);
            addChild(p.scrollWidget);
        }

        SettingsDrawer::SettingsDrawer() :
            _p(new Private)
        {}

        SettingsDrawer::~SettingsDrawer()
        {}

        std::shared_ptr<SettingsDrawer> SettingsDrawer::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<SettingsDrawer>(new SettingsDrawer);
            out->_init(context);
            return out;
        }

        void SettingsDrawer::_openStart(void)
        {
            DJV_PRIVATE_PTR();
            auto contextWeak = getContext();
            if (auto context = contextWeak.lock())
            {
                for (auto system : context->getSystemsT<IViewSystem>())
                {
                    for (auto widget : system->createSettingsWidgets())
                    {
                        widget->setLabelSizeGroup(p.sizeGroup);
                        p.widgets[widget->getSettingsSortKey()].push_back(widget);
                    }
                };

                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto appSettings = settingsSystem->getSettingsT<ApplicationSettings>();
                const auto& settingsBellows = appSettings->observeSettingsBellows()->get();
                for (const auto& i : p.widgets)
                {
                    if (i.second.size())
                    {
                        auto layout = UI::VerticalLayout::create(context);
                        layout->setMargin(UI::MetricsRole::Margin);
                        for (const auto& j : i.second)
                        {
                            auto groupBox = UI::GroupBox::create(context);
                            groupBox->addChild(j);
                            p.groupBoxes[j] = groupBox;
                            layout->addChild(groupBox);
                        }

                        auto bellows = UI::Bellows::create(context);
                        bellows->addChild(layout);
                        p.layout->addChild(bellows);
                        p.bellows[i.second[0]] = bellows;

                        const std::string group = i.second[0]->getSettingsGroup();
                        const auto k = settingsBellows.find(group);
                        bellows->setOpen(k != settingsBellows.end() ? k->second : false);

                        bellows->setOpenCallback(
                            [group, contextWeak](bool value)
                            {
                                if (auto context = contextWeak.lock())
                                {
                                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                    auto appSettings = settingsSystem->getSettingsT<ApplicationSettings>();
                                    auto settingsBellows = appSettings->observeSettingsBellows()->get();
                                    settingsBellows[group] = value;
                                    appSettings->setSettingsBellows(settingsBellows);
                                }
                            });
                    }
                }

                _textUpdate();
            }
        }

        void SettingsDrawer::_closeEnd(void)
        {
            DJV_PRIVATE_PTR();
            p.widgets.clear();
            p.bellows.clear();
            p.groupBoxes.clear();
            p.layout->clearChildren();
        }

        void SettingsDrawer::_initLayoutEvent(Event::InitLayout& event)
        {
            Drawer::_initLayoutEvent(event);
            _p->sizeGroup->calcMinimumSize();
        }

        void SettingsDrawer::_initEvent(Event::Init& event)
        {
            Drawer::_initEvent(event);
            _textUpdate();
        }

        void SettingsDrawer::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : p.bellows)
            {
                i.second->setText(_getText(i.first->getSettingsGroup()));
            }
            for (const auto& i : p.groupBoxes)
            {
                i.second->setText(_getText(i.first->getSettingsName()));
            }
        }

    } // namespace ViewApp
} // namespace djv


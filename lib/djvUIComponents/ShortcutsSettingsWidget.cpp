// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ShortcutsSettingsWidget.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Shortcut.h>
#include <djvUI/ShortcutsSettings.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ShortcutsSettingsWidget::Private
        {
            std::map<std::string, std::vector<ShortcutData> > shortcuts;

            std::shared_ptr<ButtonGroup> buttonGroup;
            std::shared_ptr<VerticalLayout> itemsLayout;
            std::shared_ptr<SearchBox> searchBox;
            std::shared_ptr<VerticalLayout> layout;

            std::shared_ptr<MapObserver<std::string, std::vector<ShortcutData> > > shortcutsObserver;
        };

        void ShortcutsSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ShortcutsSettingsWidget");

            p.buttonGroup = ButtonGroup::create(ButtonType::Push);
            p.itemsLayout = VerticalLayout::create(context);
            p.itemsLayout->setSpacing(MetricsRole::None);
            auto scrollWidget = ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->addChild(p.itemsLayout);

            p.searchBox = SearchBox::create(context);

            p.layout = VerticalLayout::create(context);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, RowStretch::Expand);
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto shortcutsSettings = settingsSystem->getSettingsT<UI::Settings::Shortcuts>();
            auto weak = std::weak_ptr<ShortcutsSettingsWidget>(std::dynamic_pointer_cast<ShortcutsSettingsWidget>(shared_from_this()));
            p.shortcutsObserver = MapObserver<std::string, std::vector<ShortcutData>>::create(
                shortcutsSettings->observeShortcuts(),
                [weak](const std::map<std::string, std::vector<ShortcutData> >& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->shortcuts = value;
                        widget->_itemsUpdate();
                    }
                });
        }

        ShortcutsSettingsWidget::ShortcutsSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ShortcutsSettingsWidget> ShortcutsSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ShortcutsSettingsWidget>(new ShortcutsSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ShortcutsSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("settings_input_section_shortcuts");
        }

        std::string ShortcutsSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("settings_title_input");
        }

        std::string ShortcutsSettingsWidget::getSettingsSortKey() const
        {
            return "Z";
        }

        void ShortcutsSettingsWidget::_initEvent(Event::Init & event)
        {
            ISettingsWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
            }
        }

        void ShortcutsSettingsWidget::_itemsUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.buttonGroup->clearButtons();
                p.itemsLayout->clearChildren();
                for (const auto& i : p.shortcuts)
                {
                    auto button = ListButton::create(context);
                    button->setText(_getText(i.first));
                    p.buttonGroup->addButton(button);
                    p.itemsLayout->addChild(button);
                }
            }
        }

    } // namespace UI
} // namespace djv


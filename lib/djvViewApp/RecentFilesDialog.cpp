// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/RecentFilesDialog.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/RecentFilesPrivate.h>

#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/Menu.h>
#include <djvUI/PopupButton.h>
#include <djvUI/PopupMenu.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>

#include <djvSystem/Context.h>
#include <djvSystem/FileInfo.h>
#include <djvSystem/TimerFunc.h>

#include <djvMath/MathFunc.h>

#include <djvCore/StringFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <regex>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct RecentFilesDialog::Private
        {
            std::vector<System::File::Info> fileInfo;
            std::string filter;
            size_t itemCount = 0;
            std::vector<System::File::Info> selected;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::PopupButton> thumbnailPopupButton;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::Menu> menu;
            std::shared_ptr<UI::PopupMenu> popupMenu;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::Text::Label> itemCountLabel;
            std::shared_ptr<UI::PushButton> acceptButton;
            std::shared_ptr<UI::PushButton> cancelButton;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void(const std::vector<System::File::Info>&)> callback;

            std::shared_ptr<Observer::List<System::File::Info> > recentFilesObserver;
            std::shared_ptr<Observer::Value<Image::Size> > thumbnailSizeSettingsObserver;
            std::shared_ptr<Observer::Map<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void RecentFilesDialog::_init(const std::shared_ptr<System::Context>& context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::RecentFilesDialog");

            p.actions["SelectAll"] = UI::Action::create();
            p.actions["SelectNone"] = UI::Action::create();
            p.actions["InvertSelection"] = UI::Action::create();

            p.actions["IncreaseThumbnailSize"] = UI::Action::create();
            p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
            p.actions["DecreaseThumbnailSize"] = UI::Action::create();
            p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");

            p.thumbnailPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.thumbnailPopupButton->setIcon("djvIconThumbnailSize");

            p.searchBox = UI::SearchBox::create(context);

            p.menu = UI::Menu::create(context);
            p.menu->setIcon("djvIconMenu");
            p.menu->setMinimumSizeRole(UI::MetricsRole::None);
            p.menu->addAction(p.actions["SelectAll"]);
            p.menu->addAction(p.actions["SelectNone"]);
            p.menu->addAction(p.actions["InvertSelection"]);
            p.menu->addSeparator();
            p.menu->addAction(p.actions["IncreaseThumbnailSize"]);
            p.menu->addAction(p.actions["DecreaseThumbnailSize"]);

            p.popupMenu = UI::PopupMenu::create(context);
            p.popupMenu->setMenu(p.menu);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.thumbnailPopupButton);
            toolBar->addChild(p.searchBox);
            toolBar->addChild(p.popupMenu);

            p.itemView = UI::FileBrowser::ItemView::create(UI::SelectionType::Multiple, context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.itemView);

            p.itemCountLabel = UI::Text::Label::create(context);
            p.itemCountLabel->setHAlign(UI::HAlign::Right);
            p.itemCountLabel->setVAlign(UI::VAlign::Bottom);
            p.itemCountLabel->setMargin(UI::MetricsRole::Margin);

            p.acceptButton = UI::PushButton::create(context);
            p.cancelButton = UI::PushButton::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(toolBar);
            for (auto action : p.actions)
            {
                p.layout->addAction(action.second);
            }
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(scrollWidget);
            stackLayout->addChild(p.itemCountLabel);
            p.layout->addChild(stackLayout);
            p.layout->setStretch(stackLayout, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setMargin(UI::MetricsRole::MarginSmall);
            hLayout->setSpacing(UI::MetricsRole::SpacingSmall);
            hLayout->addExpander();
            hLayout->addChild(p.acceptButton);
            hLayout->addChild(p.cancelButton);
            p.layout->addChild(hLayout);
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            _itemsUpdate();
            _selectedUpdate();

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            p.actions["SelectAll"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->selectAll();
                    }
                });
            p.actions["SelectNone"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->selectNone();
                    }
                });
            p.actions["InvertSelection"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->invertSelection();
                    }
                });

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.actions["IncreaseThumbnailSize"]->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                        auto size = fileBrowserSettings->observeThumbnailSize()->get();
                        size.w = Math::clamp(
                            static_cast<int>(size.w * 1.25F),
                            UI::FileBrowser::thumbnailSizeRange.getMin(),
                            UI::FileBrowser::thumbnailSizeRange.getMax());
                        size.h = static_cast<int>(ceilf(size.w / 2.F));
                        fileBrowserSettings->setThumbnailSize(size);
                    }
                });
            p.actions["DecreaseThumbnailSize"]->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
                        auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                        auto size = fileBrowserSettings->observeThumbnailSize()->get();
                        size.w = Math::clamp(
                            static_cast<int>(size.w * .75F),
                            UI::FileBrowser::thumbnailSizeRange.getMin(),
                            UI::FileBrowser::thumbnailSizeRange.getMax());
                        size.h = static_cast<int>(ceilf(size.w / 2.F));
                        fileBrowserSettings->setThumbnailSize(size);
                    }
                });

            p.thumbnailPopupButton->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<Widget>
                {
                    std::shared_ptr<Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            out = RecentFilesThumbnailWidget::create(context);
                        }
                    }
                    return out;
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->filter = value;
                        widget->_itemsUpdate();
                    }
                });

            p.itemView->setSelectedCallback(
                [weak](const std::vector<System::File::Info>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->selected = value;
                        widget->_selectedUpdate();
                    }
                });
            p.itemView->setActivatedCallback(
                [weak](const std::vector<System::File::Info>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(value);
                        }
                    }
                });

            p.acceptButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        if (widget->_p->callback)
                        {
                            widget->_p->callback(widget->_p->selected);
                        }
                    }
                });
            p.cancelButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_doCloseCallback();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
            p.recentFilesObserver = Observer::List<System::File::Info>::create(
                fileSettings->observeRecentFiles(),
                [weak](const std::vector<System::File::Info>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileInfo = value;
                        widget->_itemsUpdate();
                    }
                });

            auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            p.thumbnailSizeSettingsObserver = Observer::Value<Image::Size>::create(
                fileBrowserSettings->observeThumbnailSize(),
                [weak](const Image::Size& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->itemView->setThumbnailSize(value);
                    }
                });

            auto shortcutsSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            p.shortcutsObserver = Observer::Map<std::string, UI::ShortcutDataPair>::create(
                shortcutsSettings->observeKeyShortcuts(),
                [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto i = value.find("file_browser_shortcut_select_all");
                        if (i != value.end())
                        {
                            widget->_p->actions["SelectAll"]->setShortcuts(i->second);
                        }
                        i = value.find("file_browser_shortcut_select_none");
                        if (i != value.end())
                        {
                            widget->_p->actions["SelectNone"]->setShortcuts(i->second);
                        }
                        i = value.find("file_browser_shortcut_invert_selection");
                        if (i != value.end())
                        {
                            widget->_p->actions["InvertSelection"]->setShortcuts(i->second);
                        }
                        i = value.find("file_browser_shortcut_increase_thumbnail_size");
                        if (i != value.end())
                        {
                            widget->_p->actions["IncreaseThumbnailSize"]->setShortcuts(i->second);
                        }
                        i = value.find("file_browser_shortcut_decrease_thumbnail_size");
                        if (i != value.end())
                        {
                            widget->_p->actions["DecreaseThumbnailSize"]->setShortcuts(i->second);
                        }
                    }
                });
        }

        RecentFilesDialog::RecentFilesDialog() :
            _p(new Private)
        {}

        RecentFilesDialog::~RecentFilesDialog()
        {}

        std::shared_ptr<RecentFilesDialog> RecentFilesDialog::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<RecentFilesDialog>(new RecentFilesDialog);
            out->_init(context);
            return out;
        }

        void RecentFilesDialog::setCallback(const std::function<void(const std::vector<System::File::Info>&)>& value)
        {
            _p->callback = value;
        }

        void RecentFilesDialog::_initEvent(System::Event::Init& event)
        {
            IDialog::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("recent_files_title")));

                p.actions["SelectAll"]->setText(_getText(DJV_TEXT("file_browser_select_all")));
                p.actions["SelectAll"]->setTooltip(_getText(DJV_TEXT("file_browser_select_all_tooltip")));
                p.actions["SelectNone"]->setText(_getText(DJV_TEXT("file_browser_select_none")));
                p.actions["SelectNone"]->setTooltip(_getText(DJV_TEXT("file_browser_select_none_tooltip")));
                p.actions["InvertSelection"]->setText(_getText(DJV_TEXT("file_browser_invert_selection")));
                p.actions["InvertSelection"]->setTooltip(_getText(DJV_TEXT("file_browser_invert_selection_tooltip")));

                p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_increase_thumbnail_size")));
                p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_increase_thumbnail_size_tooltip")));
                p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("file_browser_decrease_thumbnail_size")));
                p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("file_browser_decrease_thumbnail_size_tooltip")));

                p.thumbnailPopupButton->setTooltip(_getText(DJV_TEXT("file_browser_thumbnail_size_tooltip")));

                p.searchBox->setTooltip(_getText(DJV_TEXT("file_browser_search_tooltip")));

                p.popupMenu->setTooltip(_getText(DJV_TEXT("file_browser_menu_tooltip")));

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                p.acceptButton->setText(_getText(DJV_TEXT("file_browser_accept")));
                p.cancelButton->setText(_getText(DJV_TEXT("file_browser_cancel")));
            }
        }

        std::string RecentFilesDialog::_getItemCountLabel(size_t size) const
        {
            std::stringstream ss;
            ss << size << " " << _getText(DJV_TEXT("file_browser_items"));
            return ss.str();
        }

        void RecentFilesDialog::_itemsUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<System::File::Info> items;
            std::regex r(p.filter);
            for (const auto& i : p.fileInfo)
            {
                if (String::match(i.getFileName(Math::Frame::invalid, false), p.filter))
                {
                    items.push_back(i);
                }
            }
            p.itemView->setItems(items);
            p.itemCount = items.size();
            p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));
        }

        void RecentFilesDialog::_selectedUpdate()
        {
            DJV_PRIVATE_PTR();
            p.acceptButton->setEnabled(!p.selected.empty());
        }

    } // namespace ViewApp
} // namespace djv


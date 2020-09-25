// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/RecentFilesDialog.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/RecentFilesDialogPrivate.h>

#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/PopupButton.h>
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

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::PopupButton> menuPopupButton;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void(const System::File::Info&)> callback;

            std::shared_ptr<ListObserver<System::File::Info> > recentFilesObserver;
            std::shared_ptr<ValueObserver<Image::Size> > thumbnailSizeSettingsObserver;
            std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void RecentFilesDialog::_init(const std::shared_ptr<System::Context>& context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::RecentFilesDialog");

            p.actions["IncreaseThumbnailSize"] = UI::Action::create();
            p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
            p.actions["DecreaseThumbnailSize"] = UI::Action::create();
            p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");

            p.searchBox = UI::SearchBox::create(context);

            p.menuPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.menuPopupButton->setIcon("djvIconMenu");

            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.searchBox);
            toolBar->addChild(p.menuPopupButton);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.itemView);

            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setHAlign(UI::HAlign::Right);
            p.itemCountLabel->setVAlign(UI::VAlign::Bottom);
            p.itemCountLabel->setMargin(UI::MetricsRole::Margin);

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
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            _itemsUpdate();

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

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            p.menuPopupButton->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<Widget>
                {
                    std::shared_ptr<Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            out = RecentFilesMenu::create(widget->_p->actions, context);
                        }
                    }
                    return out;
                });

            p.itemView->setCallback(
                [weak](const System::File::Info& value)
            {
                if (auto widget = weak.lock())
                {
                    if (widget->_p->callback)
                    {
                        widget->_p->callback(value);
                    }
                }
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

            auto settingsSystem = context->getSystemT<UI::Settings::SettingsSystem>();
            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
            p.recentFilesObserver = ListObserver<System::File::Info>::create(
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
            p.thumbnailSizeSettingsObserver = ValueObserver<Image::Size>::create(
                fileBrowserSettings->observeThumbnailSize(),
                [weak](const Image::Size& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->itemView->setThumbnailSize(value);
                }
            });

            auto shortcutsSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            p.shortcutsObserver = MapObserver<std::string, UI::ShortcutDataPair>::create(
                shortcutsSettings->observeKeyShortcuts(),
                [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto i = value.find("file_browser_shortcut_increase_thumbnail_size");
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

        void RecentFilesDialog::setCallback(const std::function<void(const System::File::Info&)>& value)
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

                p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("recent_files_increase_thumbnail_size")));
                p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("recent_files_increase_thumbnail_size_tooltip")));
                p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("recent_files_decrease_thumbnail_size")));
                p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("recent_files_decrease_thumbnail_size_tooltip")));

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

                p.searchBox->setTooltip(_getText(DJV_TEXT("recent_files_search_tooltip")));

                p.menuPopupButton->setTooltip(_getText(DJV_TEXT("recent_files_menu_tooltip")));
            }
        }

        std::string RecentFilesDialog::_getItemCountLabel(size_t size) const
        {
            std::stringstream ss;
            ss << size << " " << _getText(DJV_TEXT("recent_files_label_items"));
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

    } // namespace ViewApp
} // namespace djv


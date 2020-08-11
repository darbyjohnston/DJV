// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/RecentFilesDialog.h>

#include <djvViewApp/FileSettings.h>

#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/FileBrowserSettings.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionButton.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Bellows.h>
#include <djvUI/Drawer.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolBar.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <regex>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        namespace
        {
            class SettingsWidget : public UI::Widget
            {
                DJV_NON_COPYABLE(SettingsWidget);

            protected:
                void _init(
                    const std::map<std::string, std::shared_ptr<UI::Action> >&,
                    const std::shared_ptr<Context>&);
                SettingsWidget();

            public:
                static std::shared_ptr<SettingsWidget> create(
                    const std::map<std::string, std::shared_ptr<UI::Action> >&,
                    const std::shared_ptr<Context>&);

                std::map<std::string, bool> getBellowsState() const;
                void setBellowsState(const std::map<std::string, bool>&);

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                std::shared_ptr<UI::IntSlider> _maxSlider;
                std::shared_ptr<UI::IntSlider> _thumbnailSizeSlider;
                std::map<std::string, std::shared_ptr<UI::Bellows> > _bellows;
                std::shared_ptr<UI::ScrollWidget> _scrollWidget;

                std::shared_ptr<ValueObserver<size_t> > _recentFilesMaxObserver;
                std::shared_ptr<ValueObserver<AV::Image::Size> > _thumbnailSizeSettingsObserver;
            };

            void SettingsWidget::_init(
                const std::map<std::string, std::shared_ptr<UI::Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                _maxSlider = UI::IntSlider::create(context);
                _maxSlider->setRange(IntRange(5, 100));
                _maxSlider->setDelay(Time::getTime(Time::TimerValue::Medium));

                auto increaseThumbnailSizeButton = UI::ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
                auto decreaseThumbnailSizeButton = UI::ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));

                _thumbnailSizeSlider = UI::IntSlider::create(context);
                _thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
                _thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));

                auto vLayout = UI::VerticalLayout::create(context);
                vLayout->setSpacing(UI::MetricsRole::None);
                auto vLayout2 = UI::VerticalLayout::create(context);
                vLayout2->setMargin(UI::MetricsRole::MarginSmall);
                vLayout2->setSpacing(UI::MetricsRole::None);
                vLayout2->addChild(_maxSlider);
                _bellows["Max"] = UI::Bellows::create(context);
                _bellows["Max"]->addChild(vLayout2);
                vLayout->addChild(_bellows["Max"]);
                vLayout2 = UI::VerticalLayout::create(context);
                vLayout2->setMargin(UI::MetricsRole::MarginSmall);
                vLayout2->setSpacing(UI::MetricsRole::None);
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(_thumbnailSizeSlider);
                _bellows["Thumbnails"] = UI::Bellows::create(context);
                _bellows["Thumbnails"]->addChild(vLayout2);
                vLayout->addChild(_bellows["Thumbnails"]);
                _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->setMinimumSizeRole(UI::MetricsRole::None);
                _scrollWidget->setBackgroundRole(UI::ColorRole::Background);
                _scrollWidget->addChild(vLayout);
                addChild(_scrollWidget);

                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                auto weak = std::weak_ptr<SettingsWidget>(std::dynamic_pointer_cast<SettingsWidget>(shared_from_this()));
                _recentFilesMaxObserver = ValueObserver<size_t>::create(
                    fileSettings->observeRecentFilesMax(),
                    [weak](size_t value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_maxSlider->setValue(value);
                        }
                    });

                auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                _thumbnailSizeSettingsObserver = ValueObserver<AV::Image::Size>::create(
                    fileBrowserSettings->observeThumbnailSize(),
                    [weak](const AV::Image::Size& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_thumbnailSizeSlider->setValue(value.w);
                        }
                    });

                auto contextWeak = std::weak_ptr<Context>(context);
                _maxSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                            fileSettings->setRecentFilesMax(value);
                        }
                    });

                _thumbnailSizeSlider->setValueCallback(
                    [contextWeak](int value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                            fileBrowserSettings->setThumbnailSize(AV::Image::Size(value, ceilf(value / 2.F)));
                        }
                    });
            }

            SettingsWidget::SettingsWidget()
            {}

            std::shared_ptr<SettingsWidget> SettingsWidget::create(
                const std::map<std::string, std::shared_ptr<UI::Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<SettingsWidget>(new SettingsWidget);
                out->_init(actions, context);
                return out;
            }

            std::map<std::string, bool> SettingsWidget::getBellowsState() const
            {
                std::map<std::string, bool> out;
                for (const auto& i : _bellows)
                {
                    out[i.first] = i.second->isOpen();
                }
                return out;
            }

            void SettingsWidget::setBellowsState(const std::map<std::string, bool>& value)
            {
                for (const auto& i : value)
                {
                    const auto j = _bellows.find(i.first);
                    if (j != _bellows.end())
                    {
                        j->second->setOpen(i.second, false);
                    }
                }
            }

            void SettingsWidget::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_scrollWidget->getMinimumSize());
            }

            void SettingsWidget::_layoutEvent(Event::Layout&)
            {
                _scrollWidget->setGeometry(getGeometry());
            }

            void SettingsWidget::_initEvent(Event::Init& event)
            {
                if (event.getData().text)
                {
                    _bellows["Max"]->setText(_getText(DJV_TEXT("recent_files_settings_max")));
                    _bellows["Thumbnails"]->setText(_getText(DJV_TEXT("recent_files_settings_thumbnails")));
                    _thumbnailSizeSlider->setTooltip(_getText(DJV_TEXT("recent_files_settings_thumbnail_size_tooltip")));
                }
            }

        } // namespace

        struct RecentFilesDialog::Private
        {
            std::vector<Core::FileSystem::FileInfo> recentFiles;
            std::string filter;
            size_t itemCount = 0;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<SettingsWidget> settingsWidget;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void(const Core::FileSystem::FileInfo&)> callback;

            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeSettingsObserver;
            std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void RecentFilesDialog::_init(const std::shared_ptr<Core::Context>& context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::RecentFilesDialog");

            p.actions["IncreaseThumbnailSize"] = UI::Action::create();
            p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
            p.actions["DecreaseThumbnailSize"] = UI::Action::create();
            p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");

            p.actions["Settings"] = UI::Action::create();
            p.actions["Settings"]->setButtonType(UI::ButtonType::Toggle);
            p.actions["Settings"]->setIcon("djvIconSettings");

            p.searchBox = UI::SearchBox::create(context);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.searchBox);
            toolBar->addAction(p.actions["Settings"]);

            auto settingsDrawer = UI::Drawer::create(UI::Side::Right, context);

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
            auto stackLayout2 = UI::StackLayout::create(context);
            stackLayout2->addChild(scrollWidget);
            stackLayout2->addChild(p.itemCountLabel);
            auto stackLayout = UI::StackLayout::create(context);
            stackLayout->addChild(stackLayout2);
            stackLayout->addChild(settingsDrawer);
            p.layout->addChild(stackLayout);
            p.layout->setStretch(stackLayout, UI::RowStretch::Expand);
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            _recentFilesUpdate();

            auto contextWeak = std::weak_ptr<Context>(context);
            p.actions["IncreaseThumbnailSize"]->setClickedCallback(
                [contextWeak]
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
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
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
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

            p.actions["Settings"]->setCheckedCallback(
                [settingsDrawer](bool value)
                {
                    settingsDrawer->setOpen(value);
                });

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            settingsDrawer->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->settingsWidget = SettingsWidget::create(widget->_p->actions, context);
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                            widget->_p->settingsWidget->setBellowsState(fileSettings->getRecentFilesSettingsBellowsState());
                            out = widget->_p->settingsWidget;
                        }
                    }
                    return out;
                });
            settingsDrawer->setCloseCallback(
                [weak, contextWeak](const std::shared_ptr<Widget>&)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            if (widget->_p->settingsWidget)
                            {
                                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                                auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                                fileSettings->setRecentFilesSettingsBellowsState(widget->_p->settingsWidget->getBellowsState());
                                widget->_p->settingsWidget.reset();
                            }
                        }
                    }
                });

            p.itemView->setCallback(
                [weak](const Core::FileSystem::FileInfo& value)
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
                    widget->_recentFilesUpdate();
                }
            });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
            p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                fileSettings->observeRecentFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo>& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->recentFiles = value;
                    widget->_recentFilesUpdate();
                }
            });

            auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            p.thumbnailSizeSettingsObserver = ValueObserver<AV::Image::Size>::create(
                fileBrowserSettings->observeThumbnailSize(),
                [weak](const AV::Image::Size& value)
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
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.settingsWidget)
                {
                    auto settingsSystem = context->getSystemT<UI::Settings::System>();
                    auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
                    fileSettings->setRecentFilesSettingsBellowsState(p.settingsWidget->getBellowsState());
                }
            }
        }

        std::shared_ptr<RecentFilesDialog> RecentFilesDialog::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<RecentFilesDialog>(new RecentFilesDialog);
            out->_init(context);
            return out;
        }

        void RecentFilesDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo&)>& value)
        {
            _p->callback = value;
        }

        void RecentFilesDialog::_initEvent(Event::Init& event)
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
                p.actions["Settings"]->setTooltip(_getText(DJV_TEXT("recent_files_settings_tooltip")));

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));
                p.searchBox->setTooltip(_getText(DJV_TEXT("recent_files_search_tooltip")));
            }
        }

        std::string RecentFilesDialog::_getItemCountLabel(size_t size) const
        {
            std::stringstream ss;
            ss << size << " " << _getText(DJV_TEXT("recent_files_label_items"));
            return ss.str();
        }

        void RecentFilesDialog::_recentFilesUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<Core::FileSystem::FileInfo> recentFiles;
            std::regex r(p.filter);
            for (const auto& i : p.recentFiles)
            {
                if (String::match(i.getFileName(), p.filter))
                {
                    recentFiles.push_back(i);
                }
            }
            p.itemView->setItems(recentFiles);
            p.itemCount = recentFiles.size();
            p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));
        }

    } // namespace ViewApp
} // namespace djv


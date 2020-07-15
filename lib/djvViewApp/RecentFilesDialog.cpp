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

            protected:
                void _preLayoutEvent(Event::PreLayout&) override;
                void _layoutEvent(Event::Layout&) override;

                void _initEvent(Event::Init&) override;

            private:
                std::shared_ptr<UI::Label> _maxLabel;
                std::shared_ptr<UI::IntSlider> _maxSlider;
                std::shared_ptr<UI::Label> _thumbnailSizeLabel;
                std::shared_ptr<UI::IntSlider> _thumbnailSizeSlider;
                std::shared_ptr<UI::ScrollWidget> _scrollWidget;

                std::shared_ptr<ValueObserver<size_t> > _recentFilesMaxObserver;
                std::shared_ptr<ValueObserver<AV::Image::Size> > _thumbnailSizeSettingsObserver;
            };

            void SettingsWidget::_init(
                const std::map<std::string, std::shared_ptr<UI::Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                auto increaseThumbnailSizeButton = UI::ActionButton::create(context);
                increaseThumbnailSizeButton->addAction(actions.at("IncreaseThumbnailSize"));
                auto decreaseThumbnailSizeButton = UI::ActionButton::create(context);
                decreaseThumbnailSizeButton->addAction(actions.at("DecreaseThumbnailSize"));

                _maxLabel = UI::Label::create(context);
                _maxLabel->setTextHAlign(UI::TextHAlign::Left);
                _maxLabel->setBackgroundRole(UI::ColorRole::Trough);
                _maxLabel->setMargin(UI::MetricsRole::MarginSmall);
                _maxSlider = UI::IntSlider::create(context);
                _maxSlider->setRange(IntRange(5, 100));
                _maxSlider->setDelay(Time::getTime(Time::TimerValue::Medium));
                _maxSlider->setMargin(UI::MetricsRole::MarginSmall);

                _thumbnailSizeLabel = UI::Label::create(context);
                _thumbnailSizeLabel->setTextHAlign(UI::TextHAlign::Left);
                _thumbnailSizeLabel->setBackgroundRole(UI::ColorRole::Trough);
                _thumbnailSizeLabel->setMargin(UI::MetricsRole::MarginSmall);
                _thumbnailSizeSlider = UI::IntSlider::create(context);
                _thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
                _thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));
                _thumbnailSizeSlider->setMargin(UI::MetricsRole::MarginSmall);

                auto vLayout = UI::VerticalLayout::create(context);
                vLayout->setSpacing(UI::MetricsRole::None);
                vLayout->addChild(_maxLabel);
                vLayout->addSeparator();
                vLayout->addChild(_maxSlider);
                vLayout->addSeparator();
                vLayout->addChild(_thumbnailSizeLabel);
                vLayout->addSeparator();
                auto vLayout2 = UI::VerticalLayout::create(context);
                vLayout2->setSpacing(UI::MetricsRole::SpacingSmall);
                vLayout2->addChild(increaseThumbnailSizeButton);
                vLayout2->addChild(decreaseThumbnailSizeButton);
                vLayout2->addChild(_thumbnailSizeSlider);
                vLayout->addChild(vLayout2);
                _scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
                _scrollWidget->setBorder(false);
                _scrollWidget->setMinimumSizeRole(UI::MetricsRole::None);
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
                    _maxLabel->setText(_getText(DJV_TEXT("recent_files_max")));
                    _thumbnailSizeLabel->setText(_getText(DJV_TEXT("recent_files_thumbnail_size")));
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
            std::shared_ptr<UI::PopupButton> settingsPopupButton;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::StackLayout> layout;

            std::function<void(const Core::FileSystem::FileInfo&)> callback;

            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeSettingsObserver;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<MapObserver<std::string, UI::ShortcutDataPair> > shortcutsObserver;
        };

        void RecentFilesDialog::_init(const std::shared_ptr<Core::Context>& context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::RecentFilesDialog");

            p.actions["IncreaseThumbnailSize"] = UI::Action::create();
            p.actions["IncreaseThumbnailSize"]->setIcon("djvIconAdd");
            p.actions["IncreaseThumbnailSize"]->setShortcut(GLFW_KEY_EQUAL);
            p.actions["DecreaseThumbnailSize"] = UI::Action::create();
            p.actions["DecreaseThumbnailSize"]->setIcon("djvIconSubtract");
            p.actions["DecreaseThumbnailSize"]->setShortcut(GLFW_KEY_MINUS);
            p.searchBox = UI::SearchBox::create(context);

            p.settingsPopupButton = UI::PopupButton::create(UI::MenuButtonStyle::Tool, context);
            p.settingsPopupButton->setIcon("djvIconSettings");

            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.searchBox);
            toolBar->addChild(p.settingsPopupButton);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.itemView);

            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setHAlign(UI::HAlign::Right);
            p.itemCountLabel->setVAlign(UI::VAlign::Bottom);
            p.itemCountLabel->setMargin(UI::MetricsRole::Margin);

            p.layout = UI::StackLayout::create(context);
            for (auto action : p.actions)
            {
                p.layout->addAction(action.second);
            }
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(toolBar);
            vLayout->addSeparator();
            vLayout->addChild(scrollWidget);
            vLayout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addChild(vLayout);
            p.layout->addChild(p.itemCountLabel);
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            _recentFilesUpdate();

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.settingsPopupButton->setOpenCallback(
                [weak, contextWeak]() -> std::shared_ptr<UI::Widget>
                {
                    std::shared_ptr<UI::Widget> out;
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            out = SettingsWidget::create(widget->_p->actions, context);
                        }
                    }
                    return out;
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

            p.actionObservers["IncreaseThumbnailSize"] = ValueObserver<bool>::create(
                p.actions["IncreaseThumbnailSize"]->observeClicked(),
                [contextWeak](bool value)
            {
                if (value)
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
                }
            });

            p.actionObservers["DecreaseThumbnailSize"] = ValueObserver<bool>::create(
                p.actions["DecreaseThumbnailSize"]->observeClicked(),
                [contextWeak](bool value)
            {
                if (value)
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
                }
            });

            auto shortcutsSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
            p.shortcutsObserver = MapObserver<std::string, UI::ShortcutDataPair>::create(
                shortcutsSettings->observeKeyShortcuts(),
                [weak](const std::map<std::string, UI::ShortcutDataPair>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto i = value.find("UIComponents/FileBrowser/IncreaseThumbnailSize");
                        if (i != value.end())
                        {
                            widget->_p->actions["IncreaseThumbnailSize"]->setShortcuts(i->second);
                        }
                        i = value.find("UIComponents/FileBrowser/DecreaseThumbnailSize");
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
                setTitle(_getText(DJV_TEXT("widget_recent_files_title")));

                p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("widget_recent_increase_thumbnail_size")));
                p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("widget_recent_increase_thumbnail_size_tooltip")));
                p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("widget_recent_decrease_thumbnail_size")));
                p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("widget_recent_decrease_thumbnail_size_tooltip")));

                p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));
                p.searchBox->setTooltip(_getText(DJV_TEXT("recent_files_search_tooltip")));
                p.settingsPopupButton->setTooltip(_getText(DJV_TEXT("recent_files_settings_tooltip")));
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


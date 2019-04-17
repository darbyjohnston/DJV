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

#include <djvViewApp/RecentFilesDialog.h>

#include <djvViewApp/FileSystemSettings.h>

#include <djvUIComponents/ActionButton.h>
#include <djvUIComponents/FileBrowserItemView.h>
#include <djvUIComponents/FileBrowserPrivate.h>
#include <djvUIComponents/SearchBox.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Menu.h>
#include <djvUI/MenuBar.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolBar.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

#include <GLFW/glfw3.h>

#include <regex>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct RecentFilesDialog::Private
        {
            std::vector<Core::FileSystem::FileInfo> recentFiles;
            std::string filter;
            size_t itemCount = 0;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::Menu> viewMenu;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::PopupWidget> thumbnailSizePopupWidget;
            std::shared_ptr<UI::Label> thumbnailSizeLabel;
            std::shared_ptr<UI::IntSlider> thumbnailSizeSlider;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::function<void(const Core::FileSystem::FileInfo &)> callback;

            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ValueObserver<bool> > increaseThumbnailSizeObserver;
            std::shared_ptr<ValueObserver<bool> > decreaseThumbnailSizeObserver;
            std::shared_ptr<ValueObserver<glm::ivec2> > thumbnailSizeSettingsObserver;
        };

        void RecentFilesDialog::_init(Context * context)
        {
            IDialog::_init(context);

            setClassName("djv::ViewApp::RecentFilesDialog");

            DJV_PRIVATE_PTR();
            p.actions["IncreaseThumbnailSize"] = UI::Action::create();
            p.actions["IncreaseThumbnailSize"]->setShortcut(GLFW_KEY_EQUAL);
            p.actions["DecreaseThumbnailSize"] = UI::Action::create();
            p.actions["DecreaseThumbnailSize"]->setShortcut(GLFW_KEY_MINUS);
            for (auto action : p.actions)
            {
                addAction(action.second);
            }

            p.viewMenu = UI::Menu::create(context);
            p.viewMenu->addAction(p.actions["IncreaseThumbnailSize"]);
            p.viewMenu->addAction(p.actions["DecreaseThumbnailSize"]);

            auto menuBar = UI::MenuBar::create(context);
            menuBar->addChild(p.viewMenu);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.itemView);

            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setTextHAlign(UI::TextHAlign::Right);
            p.itemCountLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.searchBox = UI::SearchBox::create(context);

            p.thumbnailSizeLabel = UI::Label::create(context);
            p.thumbnailSizeLabel->setTextHAlign(UI::TextHAlign::Left);
            p.thumbnailSizeLabel->setMargin(UI::MetricsRole::MarginSmall);
            p.thumbnailSizeSlider = UI::IntSlider::create(context);
            p.thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
            p.thumbnailSizeSlider->setDelay(Time::getMilliseconds(Time::TimerValue::Medium));
            p.thumbnailSizeSlider->setMargin(UI::MetricsRole::MarginSmall);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.thumbnailSizeLabel);
            vLayout->addSeparator();
            vLayout->addChild(p.thumbnailSizeSlider);
            p.thumbnailSizePopupWidget = UI::PopupWidget::create(context);
            p.thumbnailSizePopupWidget->setIcon("djvIconThumbnailSize");
            p.thumbnailSizePopupWidget->addChild(vLayout);

            auto bottomToolBar = UI::ToolBar::create(context);
            bottomToolBar->addExpander();
            bottomToolBar->addChild(p.itemCountLabel);
            bottomToolBar->addChild(p.searchBox);
            bottomToolBar->addChild(p.thumbnailSizePopupWidget);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(menuBar);
            p.layout->addSeparator();
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(bottomToolBar);
            addChild(p.layout);
            setStretch(p.layout, UI::RowStretch::Expand);

            _recentFilesUpdate();

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            p.itemView->setCallback(
                [weak](const Core::FileSystem::FileInfo & value)
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
                [weak](const std::string & value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->filter = value;
                    widget->_recentFilesUpdate();
                }
            });

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
            {
                if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                {
                    p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                        fileSystemSettings->observeRecentFiles(),
                        [weak](const std::vector<Core::FileSystem::FileInfo> & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->recentFiles = value;
                            widget->_recentFilesUpdate();
                        }
                    });

                    p.thumbnailSizeSettingsObserver = ValueObserver<glm::ivec2>::create(
                        fileSystemSettings->observeRecentThumbnailSize(),
                        [weak](const glm::ivec2 & value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->itemView->setThumbnailSize(value);
                            widget->_p->thumbnailSizeSlider->setValue(value.x);
                        }
                    });
                }
            }

            p.thumbnailSizeSlider->setValueCallback(
                [context](int value)
            {
                if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                {
                    if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                    {
                        fileSystemSettings->setRecentThumbnailSize(glm::ivec2(value, ceilf(value / 2.f)));
                    }
                }
            });

            p.increaseThumbnailSizeObserver = ValueObserver<bool>::create(
                p.actions["IncreaseThumbnailSize"]->observeClicked(),
                [context](bool value)
            {
                if (value)
                {
                    if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                    {
                        if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                        {
                            auto size = fileSystemSettings->observeRecentThumbnailSize()->get();
                            size.x = Math::clamp(static_cast<int>(size.x * 1.25f), UI::FileBrowser::thumbnailSizeRange.min, UI::FileBrowser::thumbnailSizeRange.max);
                            size.y = static_cast<int>(ceilf(size.x / 2.f));
                            fileSystemSettings->setRecentThumbnailSize(size);
                        }
                    }
                }
            });

            p.decreaseThumbnailSizeObserver = ValueObserver<bool>::create(
                p.actions["DecreaseThumbnailSize"]->observeClicked(),
                [context](bool value)
            {
                if (value)
                {
                    if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                    {
                        if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                        {
                            auto size = fileSystemSettings->observeRecentThumbnailSize()->get();
                            size.x = Math::clamp(static_cast<int>(size.x * .75f), UI::FileBrowser::thumbnailSizeRange.min, UI::FileBrowser::thumbnailSizeRange.max);
                            size.y = static_cast<int>(ceilf(size.x / 2.f));
                            fileSystemSettings->setRecentThumbnailSize(size);
                        }
                    }
                }
            });
        }

        RecentFilesDialog::RecentFilesDialog() :
            _p(new Private)
        {}

        RecentFilesDialog::~RecentFilesDialog()
        {}

        std::shared_ptr<RecentFilesDialog> RecentFilesDialog::create(Context * context)
        {
            auto out = std::shared_ptr<RecentFilesDialog>(new RecentFilesDialog);
            out->_init(context);
            return out;
        }

        void RecentFilesDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
        {
            _p->callback = value;
        }

        void RecentFilesDialog::_localeEvent(Event::Locale & event)
        {
            IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("RECENT FILES")));

            p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Increase Thumbnail Size")));
            p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("Recent files increase thumbnail size tooltip")));
            p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Decrease Thumbnail Size")));
            p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("Recent files decrease thumbnail size tooltip")));

            p.viewMenu->setText(_getText(DJV_TEXT("View")));

            auto context = getContext();
            p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));

            p.searchBox->setTooltip(_getText(DJV_TEXT("Recent files search tooltip")));

            p.thumbnailSizeLabel->setText(_getText(DJV_TEXT("Thumbnail Size")));
            p.thumbnailSizePopupWidget->setTooltip(_getText(DJV_TEXT("Recent files thumbnail size tooltip")));
        }

        std::string RecentFilesDialog::_getItemCountLabel(size_t size) const
        {
            std::stringstream ss;
            ss << size << " " << _getText(DJV_TEXT("Items"));
            return ss.str();
        }

        void RecentFilesDialog::_recentFilesUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<Core::FileSystem::FileInfo> recentFiles;
            std::regex r(p.filter);
            for (const auto & i : p.recentFiles)
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


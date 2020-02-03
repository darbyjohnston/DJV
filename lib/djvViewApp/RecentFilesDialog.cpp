//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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
#include <djvUI/PopupWidget.h>
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
        struct RecentFilesDialog::Private
        {
            std::vector<Core::FileSystem::FileInfo> recentFiles;
            std::string filter;
            size_t itemCount = 0;

            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::Label> thumbnailSizeLabel;
            std::shared_ptr<UI::IntSlider> thumbnailSizeSlider;
            std::shared_ptr<UI::PopupWidget> settingsPopupWidget;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::StackLayout> layout;

            std::function<void(const Core::FileSystem::FileInfo &)> callback;

            std::shared_ptr<ListObserver<Core::FileSystem::FileInfo> > recentFilesObserver;
            std::shared_ptr<ValueObserver<bool> > increaseThumbnailSizeObserver;
            std::shared_ptr<ValueObserver<bool> > decreaseThumbnailSizeObserver;
            std::shared_ptr<ValueObserver<AV::Image::Size> > thumbnailSizeSettingsObserver;
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

            auto increaseThumbnailSizeButton = UI::ActionButton::create(context);
            increaseThumbnailSizeButton->addAction(p.actions["IncreaseThumbnailSize"]);
            auto decreaseThumbnailSizeButton = UI::ActionButton::create(context);
            decreaseThumbnailSizeButton->addAction(p.actions["DecreaseThumbnailSize"]);

            p.thumbnailSizeLabel = UI::Label::create(context);
            p.thumbnailSizeLabel->setTextHAlign(UI::TextHAlign::Left);
            p.thumbnailSizeLabel->setBackgroundRole(UI::ColorRole::Trough);
            p.thumbnailSizeLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            p.thumbnailSizeSlider = UI::IntSlider::create(context);
            p.thumbnailSizeSlider->setRange(UI::FileBrowser::thumbnailSizeRange);
            p.thumbnailSizeSlider->setDelay(Time::getTime(Time::TimerValue::Medium));
            p.thumbnailSizeSlider->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
            vLayout->addChild(p.thumbnailSizeLabel);
            vLayout->addSeparator();
            auto vLayout2 = UI::VerticalLayout::create(context);
            vLayout2->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
            vLayout2->addChild(increaseThumbnailSizeButton);
            vLayout2->addChild(decreaseThumbnailSizeButton);
            vLayout2->addChild(p.thumbnailSizeSlider);
            vLayout->addChild(vLayout2);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setMinimumSizeRole(UI::MetricsRole::Menu);
            scrollWidget->addChild(vLayout);
            p.settingsPopupWidget = UI::PopupWidget::create(context);
            p.settingsPopupWidget->setIcon("djvIconSettings");
            p.settingsPopupWidget->addChild(scrollWidget);

            auto toolBar = UI::ToolBar::create(context);
            toolBar->addExpander();
            toolBar->addChild(p.searchBox);
            toolBar->addChild(p.settingsPopupWidget);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.itemView);

            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setHAlign(UI::HAlign::Right);
            p.itemCountLabel->setVAlign(UI::VAlign::Bottom);
            p.itemCountLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::Margin));

            p.layout = UI::StackLayout::create(context);
            for (auto action : p.actions)
            {
                p.layout->addAction(action.second);
            }
            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
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

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto fileSettings = settingsSystem->getSettingsT<FileSettings>();
            p.recentFilesObserver = ListObserver<Core::FileSystem::FileInfo>::create(
                fileSettings->observeRecentFiles(),
                [weak](const std::vector<Core::FileSystem::FileInfo> & value)
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
                    widget->_p->thumbnailSizeSlider->setValue(value.w);
                }
            });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.thumbnailSizeSlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto fileBrowserSettings = settingsSystem->getSettingsT<UI::Settings::FileBrowser>();
                        fileBrowserSettings->setThumbnailSize(AV::Image::Size(value, ceilf(value / 2.F)));
                    }
                });

            p.increaseThumbnailSizeObserver = ValueObserver<bool>::create(
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
                        size.w = Math::clamp(static_cast<int>(size.w * 1.25F), UI::FileBrowser::thumbnailSizeRange.min, UI::FileBrowser::thumbnailSizeRange.max);
                        size.h = static_cast<int>(ceilf(size.w / 2.F));
                        fileBrowserSettings->setThumbnailSize(size);
                    }
                }
            });

            p.decreaseThumbnailSizeObserver = ValueObserver<bool>::create(
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
                        size.w = Math::clamp(static_cast<int>(size.w * .75F), UI::FileBrowser::thumbnailSizeRange.min, UI::FileBrowser::thumbnailSizeRange.max);
                        size.h = static_cast<int>(ceilf(size.w / 2.F));
                        fileBrowserSettings->setThumbnailSize(size);
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

        void RecentFilesDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
        {
            _p->callback = value;
        }

        void RecentFilesDialog::_initEvent(Event::Init & event)
        {
            IDialog::_initEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Recent Files")));

            p.actions["IncreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Increase")));
            p.actions["IncreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("Recent files increase thumbnail size tooltip")));
            p.actions["DecreaseThumbnailSize"]->setText(_getText(DJV_TEXT("Decrease")));
            p.actions["DecreaseThumbnailSize"]->setTooltip(_getText(DJV_TEXT("Recent files decrease thumbnail size tooltip")));

            p.thumbnailSizeLabel->setText(_getText(DJV_TEXT("Thumbnail Size")));

            p.itemCountLabel->setText(_getItemCountLabel(p.itemCount));
            p.searchBox->setTooltip(_getText(DJV_TEXT("Recent files search tooltip")));
            p.settingsPopupWidget->setTooltip(_getText(DJV_TEXT("Recent files settings tooltip")));
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


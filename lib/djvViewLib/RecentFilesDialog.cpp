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

#include <djvViewLib/RecentFilesDialog.h>

#include <djvViewLib/FileSystemSettings.h>

#include <djvUIComponents/ActionButton.h>
#include <djvUIComponents/FileBrowserItemView.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/PopupWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SearchBox.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/Toolbar.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct RecentFilesDialog::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> viewTypeActionGroup;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            size_t itemCount = 0;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::PopupWidget> searchPopupWidget;
            std::shared_ptr<UI::PopupWidget> settingsPopupWidget;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
			std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(const Core::FileSystem::FileInfo &)> callback;
            std::shared_ptr<ValueObserver<UI::ViewType> > viewTypeObserver;

            std::string getItemCountLabel(size_t, Context *);
        };

        void RecentFilesDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            p.actions["LargeThumbnails"] = UI::Action::create();
            p.actions["LargeThumbnails"]->setIcon("djvIconThumbnailsLarge");
            p.actions["SmallThumbnails"] = UI::Action::create();
            p.actions["SmallThumbnails"]->setIcon("djvIconThumbnailsSmall");
            p.actions["ListView"] = UI::Action::create();
            p.actions["ListView"]->setIcon("djvIconListView");
            p.viewTypeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.viewTypeActionGroup->addAction(p.actions["LargeThumbnails"]);
            p.viewTypeActionGroup->addAction(p.actions["SmallThumbnails"]);
            p.viewTypeActionGroup->addAction(p.actions["ListView"]);
            for (auto action : p.actions)
            {
                addAction(action.second);
            }

            auto searchBox = UI::SearchBox::create(context);
            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setTextHAlign(UI::TextHAlign::Right);
            p.itemCountLabel->setMargin(UI::MetricsRole::MarginSmall);
            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(searchBox);
            vLayout->addWidget(p.itemCountLabel);
            p.searchPopupWidget = UI::PopupWidget::create(context);
            p.searchPopupWidget->setIcon("djvIconSearch");
            p.searchPopupWidget->setWidget(vLayout);
            p.searchPopupWidget->setCapturePointer(false);

            vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addWidget(UI::ActionButton::create(p.actions["LargeThumbnails"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["SmallThumbnails"], context));
            vLayout->addWidget(UI::ActionButton::create(p.actions["ListView"], context));
            p.settingsPopupWidget = UI::PopupWidget::create(context);
            p.settingsPopupWidget->setIcon("djvIconSettings");
            p.settingsPopupWidget->setWidget(vLayout);

            auto bottomToolBar = UI::Toolbar::create(context);
            bottomToolBar->addExpander();
            bottomToolBar->addWidget(p.searchPopupWidget);
            bottomToolBar->addWidget(p.settingsPopupWidget);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(p.itemView);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addWidget(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addWidget(bottomToolBar);
            addWidget(p.layout, UI::RowStretch::Expand);

            auto weak = std::weak_ptr<RecentFilesDialog>(std::dynamic_pointer_cast<RecentFilesDialog>(shared_from_this()));
            p.viewTypeActionGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    const auto viewType = static_cast<UI::ViewType>(value);
                    widget->setViewType(viewType);
                    if (auto settingsSystem = context->getSystemT<UI::Settings::System>().lock())
                    {
                        if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                        {
                            fileSystemSettings->setRecentViewType(viewType);
                        }
                    }
                }
            });

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

            if (auto settingsSystem = context->getSystemT<UI::Settings::System>().lock())
            {
                if (auto fileSystemSettings = settingsSystem->getSettingsT<FileSystemSettings>())
                {
                    p.viewTypeObserver = ValueObserver<UI::ViewType>::create(
                        fileSystemSettings->observeRecentViewType(),
                        [weak](UI::ViewType value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setViewType(value);
                        }
                    });
                }
            }
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

        void RecentFilesDialog::setRecentFiles(const std::vector<Core::FileSystem::FileInfo> & value)
        {
            DJV_PRIVATE_PTR();
            p.itemView->setItems(value);
            p.itemCount = value.size();
            p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, getContext()));
        }

        void RecentFilesDialog::setViewType(UI::ViewType value)
        {
            DJV_PRIVATE_PTR();
            p.viewTypeActionGroup->setChecked(static_cast<int>(value));
            p.itemView->setViewType(value);
        }

        void RecentFilesDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
        {
            _p->callback = value;
        }

        void RecentFilesDialog::_localeEvent(Event::Locale & event)
        {
			IDialog::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Recent Files")));

            p.actions["LargeThumbnails"]->setText(_getText(DJV_TEXT("Large thumbnails")));
            p.actions["LargeThumbnails"]->setTooltip(_getText(DJV_TEXT("Recent files large thumbnails tooltip")));
            p.actions["SmallThumbnails"]->setText(_getText(DJV_TEXT("Small thumbnails")));
            p.actions["SmallThumbnails"]->setTooltip(_getText(DJV_TEXT("Recent files small thumbnails tooltip")));
            p.actions["ListView"]->setText(_getText(DJV_TEXT("List view")));
            p.actions["ListView"]->setTooltip(_getText(DJV_TEXT("Recent files list view tooltip")));

            auto context = getContext();
            p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, context));
        }

        std::string RecentFilesDialog::Private::getItemCountLabel(size_t size, Context * context)
        {
            std::stringstream ss;
            ss << size << " " << context->getText(DJV_TEXT("items"));
            return ss.str();
        }

    } // namespace ViewLib
} // namespace djv


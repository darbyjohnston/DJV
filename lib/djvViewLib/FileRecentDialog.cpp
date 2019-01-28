//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvViewLib/FileRecentDialog.h>
#include <djvViewLib/FileSystem.h>
#include <djvViewLib/FileSystemSettings.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/FileBrowserItemView.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SearchBox.h>
#include <djvUI/Toolbar.h>

#include <djvCore/Context.h>
#include <djvCore/FileInfo.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewLib
    {
        struct FileRecentDialog::Private
        {
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::shared_ptr<UI::ActionGroup> viewTypeActionGroup;
            std::shared_ptr<UI::FileBrowser::ItemView> itemView;
            size_t itemCount = 0;
            std::shared_ptr<UI::Label> itemCountLabel;
            std::shared_ptr<UI::ScrollWidget> scrollWidget;
            std::shared_ptr<UI::Layout::Vertical> layout;
            std::function<void(const Core::FileSystem::FileInfo &)> callback;
            std::shared_ptr<ValueObserver<UI::ViewType> > viewTypeObserver;

            std::string getItemCountLabel(size_t, Context *);
        };

        void FileRecentDialog::_init(Context * context)
        {
            IDialog::_init(context);

            DJV_PRIVATE_PTR();
            p.actions["LargeThumbnails"] = UI::Action::create();
            p.actions["LargeThumbnails"]->setIcon("djvIconThumbnailsLarge");
            addAction(p.actions["LargeThumbnails"]);
            p.actions["SmallThumbnails"] = UI::Action::create();
            p.actions["SmallThumbnails"]->setIcon("djvIconThumbnailsSmall");
            addAction(p.actions["SmallThumbnails"]);
            p.actions["ListView"] = UI::Action::create();
            p.actions["ListView"]->setIcon("djvIconListView");
            addAction(p.actions["ListView"]);
            p.viewTypeActionGroup = UI::ActionGroup::create(UI::ButtonType::Radio);
            p.viewTypeActionGroup->addAction(p.actions["LargeThumbnails"]);
            p.viewTypeActionGroup->addAction(p.actions["SmallThumbnails"]);
            p.viewTypeActionGroup->addAction(p.actions["ListView"]);

            p.itemCountLabel = UI::Label::create(context);
            p.itemCountLabel->setMargin(UI::Style::MetricsRole::MarginSmall);

            auto searchBox = UI::SearchBox::create(context);

            auto bottomToolBar = UI::Toolbar::create(context);
            bottomToolBar->addExpander();
            bottomToolBar->addWidget(p.itemCountLabel);
            bottomToolBar->addAction(p.actions["LargeThumbnails"]);
            bottomToolBar->addAction(p.actions["SmallThumbnails"]);
            bottomToolBar->addAction(p.actions["ListView"]);

            p.itemView = UI::FileBrowser::ItemView::create(context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->addWidget(p.itemView);

            p.layout = UI::Layout::Vertical::create(context);
            p.layout->setSpacing(UI::Style::MetricsRole::None);
            p.layout->addWidget(p.itemView, UI::Layout::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addWidget(bottomToolBar);
            addWidget(p.layout, UI::Layout::RowStretch::Expand);

            auto weak = std::weak_ptr<FileRecentDialog>(std::dynamic_pointer_cast<FileRecentDialog>(shared_from_this()));
            p.viewTypeActionGroup->setRadioCallback(
                [weak, context](int value)
            {
                if (auto widget = weak.lock())
                {
                    const auto viewType = static_cast<UI::ViewType>(value);
                    widget->setViewType(viewType);
                    if (auto fileSystem = context->getSystemT<FileSystem>().lock())
                    {
                        fileSystem->getSettings()->setRecentViewType(viewType);
                    }
                }
            });

            p.itemView->setCallback(
                [weak](const Core::FileSystem::FileInfo & value)
            {
                if (auto dialog = weak.lock())
                {
                    if (dialog->_p->callback)
                    {
                        dialog->_p->callback(value);
                    }
                    dialog->_doCloseCallback();
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>().lock())
            {
                p.viewTypeObserver = ValueObserver<UI::ViewType>::create(
                    fileSystem->getSettings()->observeRecentViewType(),
                    [weak](UI::ViewType value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->setViewType(value);
                    }
                });
            }
        }

        FileRecentDialog::FileRecentDialog() :
            _p(new Private)
        {}

        FileRecentDialog::~FileRecentDialog()
        {}

        std::shared_ptr<FileRecentDialog> FileRecentDialog::create(Context * context)
        {
            auto out = std::shared_ptr<FileRecentDialog>(new FileRecentDialog);
            out->_init(context);
            return out;
        }

        void FileRecentDialog::setRecentFiles(const std::vector<Core::FileSystem::FileInfo> & value)
        {
            DJV_PRIVATE_PTR();
            p.itemView->setItems(value);
            p.itemCount = value.size();
            p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, getContext()));
        }

        void FileRecentDialog::setViewType(UI::ViewType value)
        {
            DJV_PRIVATE_PTR();
            p.viewTypeActionGroup->setChecked(static_cast<int>(value));
            p.itemView->setViewType(value);
        }

        void FileRecentDialog::setCallback(const std::function<void(const Core::FileSystem::FileInfo &)> & value)
        {
            _p->callback = value;
        }

        void FileRecentDialog::_localeEvent(Event::Locale &)
        {
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("djv::ViewLib::FileRecentDialog", "Recent Files")));

            auto context = getContext();
            p.itemCountLabel->setText(p.getItemCountLabel(p.itemCount, context));
        }

        std::string FileRecentDialog::Private::getItemCountLabel(size_t size, Context * context)
        {
            std::stringstream ss;
            ss << size << " " << context->getText(DJV_TEXT("djv::ViewLib::FileRecentDialog", "items"));
            return ss.str();
        }

    } // namespace ViewLib
} // namespace djv


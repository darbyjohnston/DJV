// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/FileInfo.h>
#include <djvCore/RecentFilesModel.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct RecentPathsWidget::Private
            {
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<VerticalLayout> itemLayout;
                std::shared_ptr<ScrollWidget> scrollWidget;

                std::function<void(const FileSystem::Path&)> callback;

                std::shared_ptr<ListObserver<FileSystem::FileInfo> > recentPathsObserver;
            };

            void RecentPathsWidget::_init(const std::shared_ptr<FileSystem::RecentFilesModel> & model, const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UI::FileBrowser::RecentPathsWidget");

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(TextHAlign::Left);
                p.titleLabel->setMargin(MetricsRole::MarginSmall);
                p.titleLabel->setBackgroundRole(ColorRole::Trough);

                auto layout = VerticalLayout::create(context);
                layout->setSpacing(MetricsRole::None);
                layout->addChild(p.titleLabel);
                layout->addSeparator();
                p.itemLayout = VerticalLayout::create(context);
                p.itemLayout->setSpacing(MetricsRole::None);
                layout->addChild(p.itemLayout);
                p.scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                p.scrollWidget->setMinimumSizeRole(MetricsRole::None);
                p.scrollWidget->setBorder(false);
                p.scrollWidget->addChild(layout);
                addChild(p.scrollWidget);

                auto weak = std::weak_ptr<RecentPathsWidget>(std::dynamic_pointer_cast<RecentPathsWidget>(shared_from_this()));
                auto contextWeak = std::weak_ptr<Context>(context);
                p.recentPathsObserver = ListObserver<FileSystem::FileInfo>::create(
                    model->observeFiles(),
                    [weak, contextWeak](const std::vector<FileSystem::FileInfo> & value)
                    {
                        if (auto context = contextWeak.lock())
                        {
                            if (auto widget = weak.lock())
                            {
                                widget->_p->itemLayout->clearChildren();
                                for (const auto& i : value)
                                {
                                    auto button = ListButton::create(context);
                                    const auto path = i.getPath();
                                    std::string s = path.getFileName();
                                    if (s.empty())
                                    {
                                        s = std::string(i);
                                    }
                                    button->setText(s);
                                    button->setTooltip(std::string(i));

                                    widget->_p->itemLayout->addChild(button);

                                    button->setClickedCallback(
                                        [weak, path]
                                        {
                                            if (auto widget = weak.lock())
                                            {
                                                if (widget->_p->callback)
                                                {
                                                    widget->_p->callback(path);
                                                }
                                            }
                                        });
                                }
                            }
                        }
                    });
            }

            RecentPathsWidget::RecentPathsWidget() :
                _p(new Private)
            {}

            RecentPathsWidget::~RecentPathsWidget()
            {}

            std::shared_ptr<RecentPathsWidget> RecentPathsWidget::create(const std::shared_ptr<FileSystem::RecentFilesModel> & model, const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<RecentPathsWidget>(new RecentPathsWidget);
                out->_init(model, context);
                return out;
            }

            void RecentPathsWidget::setCallback(const std::function<void(const FileSystem::Path &)> & value)
            {
                _p->callback = value;
            }

            void RecentPathsWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->scrollWidget->getMinimumSize());
            }

            void RecentPathsWidget::_layoutEvent(Event::Layout& event)
            {
                _p->scrollWidget->setGeometry(getGeometry());
            }

            void RecentPathsWidget::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.titleLabel->setText(_getText(DJV_TEXT("file_browser_recent_paths")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/FileBrowserPrivate.h>

#include <djvUI/ActionButton.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace FileBrowser
        {
            struct SortWidget::Private
            {
                std::shared_ptr<Label> titleLabel;
                std::shared_ptr<VerticalLayout> layout;
            };

            void SortWidget::_init(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                DJV_PRIVATE_PTR();

                setClassName("djv::UI::FileBrowser::SortWidget");

                p.titleLabel = Label::create(context);
                p.titleLabel->setTextHAlign(TextHAlign::Left);
                p.titleLabel->setMargin(MetricsRole::MarginSmall);
                p.titleLabel->setBackgroundRole(ColorRole::Trough);

                auto sortByNameButton = ActionButton::create(context);
                sortByNameButton->addAction(actions.at("SortByName"));
                auto sortBySizeButton = ActionButton::create(context);
                sortBySizeButton->addAction(actions.at("SortBySize"));
                auto sortByTimeButton = ActionButton::create(context);
                sortByTimeButton->addAction(actions.at("SortByTime"));
                auto reverseSortButton = ActionButton::create(context);
                reverseSortButton->addAction(actions.at("ReverseSort"));
                auto sortDirectoriesFirstButton = ActionButton::create(context);
                sortDirectoriesFirstButton->addAction(actions.at("SortDirectoriesFirst"));

                p.layout = VerticalLayout::create(context);
                p.layout->setSpacing(MetricsRole::None);
                p.layout->addChild(p.titleLabel);
                p.layout->addSeparator();
                auto vLayout = VerticalLayout::create(context);
                vLayout->setSpacing(MetricsRole::None);
                vLayout->addChild(sortByNameButton);
                vLayout->addChild(sortBySizeButton);
                vLayout->addChild(sortByTimeButton);
                vLayout->addSeparator();
                vLayout->addChild(reverseSortButton);
                vLayout->addChild(sortDirectoriesFirstButton);
                auto scrollWidget = ScrollWidget::create(ScrollType::Vertical, context);
                scrollWidget->setBorder(false);
                scrollWidget->setMinimumSizeRole(MetricsRole::None);
                scrollWidget->addChild(vLayout);
                p.layout->addChild(scrollWidget);
                p.layout->setStretch(scrollWidget, RowStretch::Expand);
                addChild(p.layout);
            }

            SortWidget::SortWidget() :
                _p(new Private)
            {}

            SortWidget::~SortWidget()
            {}

            std::shared_ptr<SortWidget> SortWidget::create(
                const std::map<std::string, std::shared_ptr<Action> >& actions,
                const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<SortWidget>(new SortWidget);
                out->_init(actions, context);
                return out;
            }

            void SortWidget::_preLayoutEvent(Event::PreLayout& event)
            {
                _setMinimumSize(_p->layout->getMinimumSize());
            }

            void SortWidget::_layoutEvent(Event::Layout& event)
            {
                _p->layout->setGeometry(getGeometry());
            }

            void SortWidget::_initEvent(Event::Init& event)
            {
                DJV_PRIVATE_PTR();
                if (event.getData().text)
                {
                    p.titleLabel->setText(_getText(DJV_TEXT("file_browser_sorting")));
                }
            }

        } // namespace FileBrowser
    } // namespace UI
} // namespace djv

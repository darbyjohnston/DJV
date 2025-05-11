// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/SetupDialog.h>

#include <djvApp/Tools/SettingsToolPrivate.h>
#include <djvApp/App.h>

#include <dtk/ui/ComboBox.h>
#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/StackLayout.h>
#include <dtk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct SetupStartWidget::Private
        {
            std::shared_ptr<dtk::VerticalLayout> layout;
        };

        void SetupStartWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            IWidget::_init(context, "djv::app::SetupStartWidget", parent);
            DTK_P();
            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            dtk::Label::create(context, dtk::Format("Welcome to DJV version {0}.").arg(DJV_VERSION), p.layout);
            dtk::Label::create(context, "Start by configuring some common settings.", p.layout);
            dtk::Label::create(context, "Changes can also be made later in the settings tool.", p.layout);
        }

        SetupStartWidget::SetupStartWidget() :
            _p(new Private)
        {}

        SetupStartWidget::~SetupStartWidget()
        {}

        std::shared_ptr<SetupStartWidget> SetupStartWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            auto out = std::shared_ptr<SetupStartWidget>(new SetupStartWidget);
            out->_init(context, app, parent);
            return out;
        }

        void SetupStartWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void SetupStartWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct SetupDialog::Private
        {
            std::shared_ptr<dtk::PushButton> nextButton;
            std::shared_ptr<dtk::PushButton> prevButton;
            std::shared_ptr<dtk::PushButton> closeButton;
            std::shared_ptr<dtk::StackLayout> stackLayout;
            std::vector<std::shared_ptr<dtk::IWidget> > widgets;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<bool> > nextObserver;
            std::shared_ptr<dtk::ValueObserver<bool> > prevObserver;
        };

        void SetupDialog::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IDialog::_init(
                context,
                "djv::app::SetupDialog",
                parent);
            DTK_P();

            auto label = dtk::Label::create(
                context,
                dtk::Format("Setup").arg(DJV_VERSION));
            label->setFontRole(dtk::FontRole::Title);
            label->setMarginRole(dtk::SizeRole::Margin);

            p.nextButton = dtk::PushButton::create(context, "Next");
            p.prevButton = dtk::PushButton::create(context, "Previous");
            p.closeButton = dtk::PushButton::create(context, "Close");

            p.stackLayout = dtk::StackLayout::create(context);
            p.stackLayout->setMarginRole(dtk::SizeRole::MarginDialog);
            p.widgets.push_back(SetupStartWidget::create(context, app, p.stackLayout));
            auto vLayout = dtk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingLarge);
            dtk::Label::create(context, "Configure the style:", vLayout);
            p.widgets.push_back(StyleSettingsWidget::create(context, app, vLayout));
            vLayout = dtk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingLarge);
            dtk::Label::create(context, "Configure the cache:", vLayout);
            p.widgets.push_back(CacheSettingsWidget::create(context, app, vLayout));
            vLayout = dtk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingLarge);
            dtk::Label::create(context, "Configure the time settings:", vLayout);
            p.widgets.push_back(TimeSettingsWidget::create(context, app, vLayout));

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            label->setParent(p.layout);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            p.stackLayout->setParent(p.layout);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(dtk::SizeRole::Margin);
            p.prevButton->setParent(hLayout);
            p.nextButton->setParent(hLayout);
            hLayout->addSpacer(dtk::SizeRole::Spacing, dtk::Stretch::Expanding);
            p.closeButton->setParent(hLayout);

            p.nextButton->setClickedCallback(
                [this]
                {
                    _p->stackLayout->nextIndex();
                });

            p.prevButton->setClickedCallback(
                [this]
                {
                    _p->stackLayout->prevIndex();
                });

            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
                });

            p.nextObserver = dtk::ValueObserver<bool>::create(
                p.stackLayout->observeHasNextIndex(),
                [this](bool value)
                {
                    _p->nextButton->setEnabled(value);
                });

            p.prevObserver = dtk::ValueObserver<bool>::create(
                p.stackLayout->observeHasPrevIndex(),
                [this](bool value)
                {
                    _p->prevButton->setEnabled(value);
                });
        }

        SetupDialog::SetupDialog() :
            _p(new Private)
        {}

        SetupDialog::~SetupDialog()
        {}

        std::shared_ptr<SetupDialog> SetupDialog::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SetupDialog>(new SetupDialog);
            out->_init(context, app, parent);
            return out;
        }
    }
}

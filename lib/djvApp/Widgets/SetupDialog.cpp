// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/SetupDialog.h>

#include <djvApp/Tools/SettingsToolPrivate.h>
#include <djvApp/App.h>

#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/StackLayout.h>
#include <feather-tk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct SetupStartWidget::Private
        {
            std::shared_ptr<feather_tk::VerticalLayout> layout;
        };

        void SetupStartWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            IWidget::_init(context, "djv::app::SetupStartWidget", parent);
            FEATHER_TK_P();
            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            feather_tk::Label::create(context, feather_tk::Format("Welcome to DJV version {0}.").arg(DJV_VERSION), p.layout);
            feather_tk::Label::create(context, "Start by configuring some common settings.", p.layout);
            feather_tk::Label::create(context, "Changes can also be made later in the settings tool.", p.layout);
        }

        SetupStartWidget::SetupStartWidget() :
            _p(new Private)
        {}

        SetupStartWidget::~SetupStartWidget()
        {}

        std::shared_ptr<SetupStartWidget> SetupStartWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            auto out = std::shared_ptr<SetupStartWidget>(new SetupStartWidget);
            out->_init(context, app, parent);
            return out;
        }

        void SetupStartWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void SetupStartWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct SetupDialog::Private
        {
            std::shared_ptr<feather_tk::PushButton> nextButton;
            std::shared_ptr<feather_tk::PushButton> prevButton;
            std::shared_ptr<feather_tk::PushButton> closeButton;
            std::shared_ptr<feather_tk::StackLayout> stackLayout;
            std::vector<std::shared_ptr<feather_tk::IWidget> > widgets;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<bool> > nextObserver;
            std::shared_ptr<feather_tk::ValueObserver<bool> > prevObserver;
        };

        void SetupDialog::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IDialog::_init(
                context,
                "djv::app::SetupDialog",
                parent);
            FEATHER_TK_P();

            auto label = feather_tk::Label::create(
                context,
                feather_tk::Format("Setup").arg(DJV_VERSION));
            label->setFontRole(feather_tk::FontRole::Title);
            label->setMarginRole(feather_tk::SizeRole::Margin);

            p.nextButton = feather_tk::PushButton::create(context, "Next");
            p.prevButton = feather_tk::PushButton::create(context, "Previous");
            p.closeButton = feather_tk::PushButton::create(context, "Close");

            p.stackLayout = feather_tk::StackLayout::create(context);
            p.stackLayout->setMarginRole(feather_tk::SizeRole::MarginDialog);
            p.widgets.push_back(SetupStartWidget::create(context, app, p.stackLayout));
            auto vLayout = feather_tk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingLarge);
            feather_tk::Label::create(context, "Configure the style:", vLayout);
            p.widgets.push_back(StyleSettingsWidget::create(context, app, vLayout));
            vLayout = feather_tk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingLarge);
            feather_tk::Label::create(context, "Configure the cache:", vLayout);
            p.widgets.push_back(CacheSettingsWidget::create(context, app, vLayout));
            vLayout = feather_tk::VerticalLayout::create(context, p.stackLayout);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingLarge);
            feather_tk::Label::create(context, "Configure the time settings:", vLayout);
            p.widgets.push_back(TimeSettingsWidget::create(context, app, vLayout));

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            label->setParent(p.layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            p.stackLayout->setParent(p.layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(feather_tk::SizeRole::Margin);
            p.prevButton->setParent(hLayout);
            p.nextButton->setParent(hLayout);
            hLayout->addSpacer(feather_tk::SizeRole::Spacing, feather_tk::Stretch::Expanding);
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

            p.nextObserver = feather_tk::ValueObserver<bool>::create(
                p.stackLayout->observeHasNextIndex(),
                [this](bool value)
                {
                    _p->nextButton->setEnabled(value);
                });

            p.prevObserver = feather_tk::ValueObserver<bool>::create(
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SetupDialog>(new SetupDialog);
            out->_init(context, app, parent);
            return out;
        }
    }
}

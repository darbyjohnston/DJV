// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/AboutDialog.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/core/Format.h>
#include <dtk/core/OS.h>

namespace djv
{
    namespace app
    {
        struct AboutDialog::Private
        {
            std::shared_ptr<dtk::PushButton> licenseButton;
            std::shared_ptr<dtk::PushButton> creditsButton;
            std::shared_ptr<dtk::PushButton> closeButton;
            std::shared_ptr<dtk::VerticalLayout> layout;
        };

        void AboutDialog::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IDialog::_init(
                context,
                "djv::app::AboutDialog",
                parent);
            DTK_P();

            auto titleLabel = dtk::Label::create(context, "About", p.layout);
            titleLabel->setFontRole(dtk::FontRole::Title);
            titleLabel->setMarginRole(dtk::SizeRole::Margin);

            auto copyrightLabel = dtk::Label::create(
                context,
                dtk::Format(
                    "DJV\n"
                    "Version {0}\n"
                    "Copyright (c) 2004-2025 Darby Johnston\n"
                    "All rights reserved."
                ).arg(DJV_VERSION));

            p.licenseButton = dtk::PushButton::create(context, "License");
            p.creditsButton = dtk::PushButton::create(context, "Credits");
            p.closeButton = dtk::PushButton::create(context, "Close");

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            titleLabel->setParent(p.layout);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto vLayout = dtk::VerticalLayout::create(context, p.layout);
            vLayout->setMarginRole(dtk::SizeRole::MarginDialog);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingLarge);
            copyrightLabel->setParent(vLayout);
            auto vLayout2 = dtk::VerticalLayout::create(context, vLayout);
            vLayout2->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.licenseButton->setParent(vLayout2);
            p.creditsButton->setParent(vLayout2);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(dtk::SizeRole::Margin);
            hLayout->addSpacer(dtk::SizeRole::Spacing, dtk::Stretch::Expanding);
            p.closeButton->setParent(hLayout);

            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
                });

            p.licenseButton->setClickedCallback(
                [this]
                {
                    dtk::openURL("https://darbyjohnston.github.io/DJV/legal.html");
                });

            p.creditsButton->setClickedCallback(
                [this]
                {
                    dtk::openURL("https://darbyjohnston.github.io/DJV/credits.html");
                });
        }

        AboutDialog::AboutDialog() :
            _p(new Private)
        {}

        AboutDialog::~AboutDialog()
        {}

        std::shared_ptr<AboutDialog> AboutDialog::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AboutDialog>(new AboutDialog);
            out->_init(context, app, parent);
            return out;
        }
    }
}

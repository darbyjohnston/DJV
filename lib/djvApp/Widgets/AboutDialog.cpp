// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/AboutDialog.h>

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/core/Format.h>
#include <feather-tk/core/OS.h>

namespace djv
{
    namespace app
    {
        struct AboutDialog::Private
        {
            std::shared_ptr<feather_tk::PushButton> licenseButton;
            std::shared_ptr<feather_tk::PushButton> creditsButton;
            std::shared_ptr<feather_tk::PushButton> closeButton;
            std::shared_ptr<feather_tk::VerticalLayout> layout;
        };

        void AboutDialog::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IDialog::_init(
                context,
                "djv::app::AboutDialog",
                parent);
            FEATHER_TK_P();

            auto titleLabel = feather_tk::Label::create(context, "About", p.layout);
            titleLabel->setFontRole(feather_tk::FontRole::Title);
            titleLabel->setMarginRole(feather_tk::SizeRole::Margin);

            auto copyrightLabel = feather_tk::Label::create(
                context,
                feather_tk::Format(
                    "DJV\n"
                    "Version {0}\n"
                    "Copyright (c) 2004-2025 Darby Johnston\n"
                    "All rights reserved."
                ).arg(DJV_VERSION));

            p.licenseButton = feather_tk::PushButton::create(context, "License");
            p.creditsButton = feather_tk::PushButton::create(context, "Credits");
            p.closeButton = feather_tk::PushButton::create(context, "Close");

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            titleLabel->setParent(p.layout);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto vLayout = feather_tk::VerticalLayout::create(context, p.layout);
            vLayout->setMarginRole(feather_tk::SizeRole::MarginDialog);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingLarge);
            copyrightLabel->setParent(vLayout);
            auto vLayout2 = feather_tk::VerticalLayout::create(context, vLayout);
            vLayout2->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.licenseButton->setParent(vLayout2);
            p.creditsButton->setParent(vLayout2);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(feather_tk::SizeRole::Margin);
            hLayout->addSpacer(feather_tk::SizeRole::Spacing, feather_tk::Stretch::Expanding);
            p.closeButton->setParent(hLayout);

            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
                });

            p.licenseButton->setClickedCallback(
                [this]
                {
                    feather_tk::openURL("https://darbyjohnston.github.io/DJV/legal.html");
                });

            p.creditsButton->setClickedCallback(
                [this]
                {
                    feather_tk::openURL("https://darbyjohnston.github.io/DJV/credits.html");
                });
        }

        AboutDialog::AboutDialog() :
            _p(new Private)
        {}

        AboutDialog::~AboutDialog()
        {}

        std::shared_ptr<AboutDialog> AboutDialog::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AboutDialog>(new AboutDialog);
            out->_init(context, app, parent);
            return out;
        }
    }
}

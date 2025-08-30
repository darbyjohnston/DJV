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
            std::shared_ptr<ftk::PushButton> licenseButton;
            std::shared_ptr<ftk::PushButton> closeButton;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void AboutDialog::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IDialog::_init(
                context,
                "djv::app::AboutDialog",
                parent);
            FTK_P();

            auto titleLabel = ftk::Label::create(context, "About", p.layout);
            titleLabel->setFontRole(ftk::FontRole::Title);
            titleLabel->setMarginRole(ftk::SizeRole::Margin);

            auto copyrightLabel = ftk::Label::create(
                context,
                ftk::Format(
                    "DJV\n"
                    "Version {0}\n"
                    "Copyright (c) 2004-2025 Darby Johnston\n"
                    "All rights reserved."
                ).arg(DJV_VERSION));

            p.licenseButton = ftk::PushButton::create(context, "License");
            p.closeButton = ftk::PushButton::create(context, "Close");

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            titleLabel->setParent(p.layout);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto vLayout = ftk::VerticalLayout::create(context, p.layout);
            vLayout->setMarginRole(ftk::SizeRole::MarginDialog);
            vLayout->setSpacingRole(ftk::SizeRole::SpacingLarge);
            copyrightLabel->setParent(vLayout);
            auto vLayout2 = ftk::VerticalLayout::create(context, vLayout);
            vLayout2->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.licenseButton->setParent(vLayout2);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            auto hLayout = ftk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(ftk::SizeRole::Margin);
            hLayout->addSpacer(ftk::SizeRole::Spacing, ftk::Stretch::Expanding);
            p.closeButton->setParent(hLayout);

            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
                });

            p.licenseButton->setClickedCallback(
                [this]
                {
                    ftk::openURL("https://github.com/darbyjohnston/DJV/blob/main/LICENSE.txt");
                });
        }

        AboutDialog::AboutDialog() :
            _p(new Private)
        {}

        AboutDialog::~AboutDialog()
        {}

        std::shared_ptr<AboutDialog> AboutDialog::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<AboutDialog>(new AboutDialog);
            out->_init(context, app, parent);
            return out;
        }
    }
}

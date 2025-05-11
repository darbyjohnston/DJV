// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/AboutDialog.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/core/Format.h>

namespace djv
{
    namespace app
    {
        struct AboutDialog::Private
        {
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

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);

            auto label = dtk::Label::create(context, "About", p.layout);
            label->setFontRole(dtk::FontRole::Title);
            label->setMarginRole(dtk::SizeRole::Margin);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);

            auto vLayout = dtk::VerticalLayout::create(context, p.layout);
            vLayout->setMarginRole(dtk::SizeRole::MarginDialog);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            label = dtk::Label::create(context, "DJV", vLayout);
            label = dtk::Label::create(
                context,
                dtk::Format("Version {0}").arg(DJV_VERSION),
                vLayout);
            label = dtk::Label::create(
                context,
                "Copyright (c) 2004-2025 Darby Johnston",
                vLayout);
            label = dtk::Label::create(
                context,
                "All rights reserved.",
                vLayout);

            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(dtk::SizeRole::Margin);
            hLayout->addSpacer(dtk::SizeRole::Spacing, dtk::Stretch::Expanding);
            p.closeButton = dtk::PushButton::create(context, "Close", hLayout);

            p.closeButton->setClickedCallback(
                [this]
                {
                    close();
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

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/SeparateAudioDialog.h>

#include <dtk/ui/Divider.h>
#include <dtk/ui/FileEdit.h>
#include <dtk/ui/GroupBox.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/PushButton.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/Spacer.h>

namespace djv
{
    namespace app
    {
        struct SeparateAudioWidget::Private
        {
            std::shared_ptr<dtk::FileEdit> videoFileEdit;
            std::shared_ptr<dtk::FileEdit> audioFileEdit;
            std::shared_ptr<dtk::PushButton> okButton;
            std::shared_ptr<dtk::PushButton> cancelButton;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::function<void(const tl::file::Path&, const tl::file::Path&)> callback;
            std::function<void(void)> cancelCallback;
        };

        void SeparateAudioWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::SeparateAudioWidget",
                parent);
            DTK_P();

            setHStretch(dtk::Stretch::Expanding);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.videoFileEdit = dtk::FileEdit::create(context);

            p.audioFileEdit = dtk::FileEdit::create(context);

            p.okButton = dtk::PushButton::create(context, "OK");
            p.cancelButton = dtk::PushButton::create(context, "Cancel");

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(dtk::SizeRole::None);
            auto label = dtk::Label::create(context, "Open Separate Audio", p.layout);
            label->setMarginRole(dtk::SizeRole::MarginSmall);
            label->setBackgroundRole(dtk::ColorRole::Button);
            auto vLayout = dtk::VerticalLayout::create(context, p.layout);
            vLayout->setVStretch(dtk::Stretch::Expanding);
            vLayout->setMarginRole(dtk::SizeRole::MarginSmall);
            vLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            auto groupBox = dtk::GroupBox::create(context, "Video", vLayout);
            p.videoFileEdit->setParent(groupBox);
            groupBox = dtk::GroupBox::create(context, "Audio", vLayout);
            p.audioFileEdit->setParent(groupBox);
            auto spacer = dtk::Spacer::create(context, dtk::Orientation::Vertical, vLayout);
            spacer->setSpacingRole(dtk::SizeRole::None);
            spacer->setVStretch(dtk::Stretch::Expanding);
            dtk::Divider::create(context, dtk::Orientation::Vertical, p.layout);
            auto hLayout = dtk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(dtk::SizeRole::MarginSmall);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            spacer = dtk::Spacer::create(context, dtk::Orientation::Horizontal, hLayout);
            spacer->setSpacingRole(dtk::SizeRole::None);
            spacer->setHStretch(dtk::Stretch::Expanding);
            p.okButton->setParent(hLayout);
            p.cancelButton->setParent(hLayout);

            p.okButton->setClickedCallback(
                [this]
                {
                    if (_p->callback)
                    {
                        _p->callback(
                            tl::file::Path(_p->videoFileEdit->getPath().u8string()),
                            tl::file::Path(_p->audioFileEdit->getPath().u8string()));
                    }
                });

            p.cancelButton->setClickedCallback(
                [this]
                {
                    if (_p->cancelCallback)
                    {
                        _p->cancelCallback();
                    }
                });
        }

        SeparateAudioWidget::SeparateAudioWidget() :
            _p(new Private)
        {}

        SeparateAudioWidget::~SeparateAudioWidget()
        {}

        std::shared_ptr<SeparateAudioWidget> SeparateAudioWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SeparateAudioWidget>(new SeparateAudioWidget);
            out->_init(context, parent);
            return out;
        }

        void SeparateAudioWidget::setCallback(const std::function<void(
            const tl::file::Path&,
            const tl::file::Path&)>& value)
        {
            _p->callback = value;
        }

        void SeparateAudioWidget::setCancelCallback(const std::function<void(void)>& value)
        {
            _p->cancelCallback = value;
        }

        void SeparateAudioWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void SeparateAudioWidget::sizeHintEvent(const dtk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }
    }
}

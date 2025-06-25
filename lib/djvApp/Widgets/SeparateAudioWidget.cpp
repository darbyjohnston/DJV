// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Widgets/SeparateAudioDialog.h>

#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/FileEdit.h>
#include <feather-tk/ui/GroupBox.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/PushButton.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/Spacer.h>

namespace djv
{
    namespace app
    {
        struct SeparateAudioWidget::Private
        {
            std::shared_ptr<feather_tk::FileEdit> videoFileEdit;
            std::shared_ptr<feather_tk::FileEdit> audioFileEdit;
            std::shared_ptr<feather_tk::PushButton> okButton;
            std::shared_ptr<feather_tk::PushButton> cancelButton;
            std::shared_ptr<feather_tk::VerticalLayout> layout;

            std::function<void(const tl::file::Path&, const tl::file::Path&)> callback;
            std::function<void(void)> cancelCallback;
        };

        void SeparateAudioWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(
                context,
                "djv::app::SeparateAudioWidget",
                parent);
            FEATHER_TK_P();

            setHStretch(feather_tk::Stretch::Expanding);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.videoFileEdit = feather_tk::FileEdit::create(context);

            p.audioFileEdit = feather_tk::FileEdit::create(context);

            p.okButton = feather_tk::PushButton::create(context, "OK");
            p.cancelButton = feather_tk::PushButton::create(context, "Cancel");

            p.layout = feather_tk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(feather_tk::SizeRole::None);
            auto label = feather_tk::Label::create(context, "Open Separate Audio", p.layout);
            label->setMarginRole(feather_tk::SizeRole::MarginSmall);
            label->setBackgroundRole(feather_tk::ColorRole::Button);
            auto vLayout = feather_tk::VerticalLayout::create(context, p.layout);
            vLayout->setVStretch(feather_tk::Stretch::Expanding);
            vLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto groupBox = feather_tk::GroupBox::create(context, "Video", vLayout);
            p.videoFileEdit->setParent(groupBox);
            groupBox = feather_tk::GroupBox::create(context, "Audio", vLayout);
            p.audioFileEdit->setParent(groupBox);
            auto spacer = feather_tk::Spacer::create(context, feather_tk::Orientation::Vertical, vLayout);
            spacer->setSpacingRole(feather_tk::SizeRole::None);
            spacer->setVStretch(feather_tk::Stretch::Expanding);
            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, p.layout);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            spacer = feather_tk::Spacer::create(context, feather_tk::Orientation::Horizontal, hLayout);
            spacer->setSpacingRole(feather_tk::SizeRole::None);
            spacer->setHStretch(feather_tk::Stretch::Expanding);
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
            const std::shared_ptr<feather_tk::Context>& context,
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

        void SeparateAudioWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void SeparateAudioWidget::sizeHintEvent(const feather_tk::SizeHintEvent& event)
        {
            IWidget::sizeHintEvent(event);
            _setSizeHint(_p->layout->getSizeHint());
        }
    }
}

//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/IDialog.h>

#include <djvUI/Border.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ToolButton.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class DialogLayout : public VerticalLayout
            {
                DJV_NON_COPYABLE(DialogLayout);

            protected:
                void _init(Context * context)
                {
                    Vertical::_init(context);
                    setPointerEnabled(true);
                }
                
                DialogLayout()
                {}

            public:
                static std::shared_ptr<DialogLayout> create(Context * context)
                {
                    auto out = std::shared_ptr<DialogLayout>(new DialogLayout);
                    out->_init(context);
                    return out;
                }

            protected:
                void _buttonPressEvent(Event::ButtonPress & event) override
                {
                    event.accept();
                }

                void _buttonReleaseEvent(Event::ButtonRelease & event) override
                {
                    event.accept();
                }
            };

        } // namespace

        struct IDialog::Private
        {
            std::shared_ptr<Label> titleLabel;
            std::shared_ptr<VerticalLayout> childLayout;
            std::shared_ptr<Border> border;
            std::shared_ptr<Layout::Overlay> overlay;
            std::function<void(void)> closeCallback;
        };

        void IDialog::_init(Context * context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            p.titleLabel = Label::create(context);
            p.titleLabel->setFontSizeRole(UI::Style::MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(TextHAlign::Left);
            p.titleLabel->setTextColorRole(Style::ColorRole::HeaderForeground);
            p.titleLabel->setMargin(UI::Layout::Margin(
                UI::Style::MetricsRole::MarginLarge,
                UI::Style::MetricsRole::None,
                UI::Style::MetricsRole::Margin,
                UI::Style::MetricsRole::Margin));

            auto closeButton = ToolButton::create(context);
            closeButton->setIcon("djvIconClose");
            closeButton->setForegroundColorRole(UI::Style::ColorRole::HeaderForeground);
            closeButton->setHoveredColorRole(UI::Style::ColorRole::HeaderHovered);
            closeButton->setPressedColorRole(UI::Style::ColorRole::HeaderPressed);
            closeButton->setCheckedColorRole(UI::Style::ColorRole::HeaderChecked);
            closeButton->setDisabledColorRole(UI::Style::ColorRole::HeaderDisabled);
            closeButton->setInsideMargin(Style::MetricsRole::MarginSmall);

            p.childLayout = VerticalLayout::create(context);
            p.childLayout->setSpacing(Style::MetricsRole::None);
            p.childLayout->setBackgroundRole(Style::ColorRole::Background);

            auto layout = DialogLayout::create(context);
            layout->setSpacing(Style::MetricsRole::None);
            auto hLayout = HorizontalLayout::create(context);
            hLayout->setBackgroundRole(Style::ColorRole::HeaderBackground);
            hLayout->addWidget(p.titleLabel, Layout::RowStretch::Expand);
            hLayout->addWidget(closeButton);
            layout->addWidget(hLayout);
            layout->addWidget(p.childLayout, Layout::RowStretch::Expand);

            p.border = Border::create(context);
            p.border->addWidget(layout);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->setMargin(Style::MetricsRole::MarginDialog);
            p.overlay->addWidget(p.border);
            p.overlay->setParent(shared_from_this());

            auto weak = std::weak_ptr<IDialog>(std::dynamic_pointer_cast<IDialog>(shared_from_this()));
            closeButton->setClickedCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                }
            });
            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                }
            });
        }

        IDialog::IDialog() :
            _p(new Private)
        {}

        IDialog::~IDialog()
        {}

        void IDialog::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void IDialog::setFillLayout(bool value)
        {
            DJV_PRIVATE_PTR();
            p.border->setHAlign(value ? HAlign::Fill : HAlign::Center);
            p.border->setVAlign(value ? VAlign::Fill : VAlign::Center);
        }

        void IDialog::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        void IDialog::addWidget(const std::shared_ptr<Widget>& value, Layout::RowStretch stretch)
        {
            _p->childLayout->addWidget(value, stretch);
        }

        void IDialog::removeWidget(const std::shared_ptr<Widget>& value)
        {
            _p->childLayout->removeWidget(value);
        }

        void IDialog::clearWidgets()
        {
            _p->childLayout->clearWidgets();
        }

        void IDialog::setVisible(bool value)
        {
            Widget::setVisible(value);
            _p->overlay->setVisible(value);
        }

        float IDialog::getHeightForWidth(float value) const
        {
            return _p->overlay->getHeightForWidth(value);
        }

        void IDialog::_doCloseCallback()
        {
            if (_p->closeCallback)
            {
                _p->closeCallback();
            }
        }

        void IDialog::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->overlay->getMinimumSize());
        }

        void IDialog::_layoutEvent(Event::Layout&)
        {
            _p->overlay->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv

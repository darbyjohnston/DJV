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

#include <djvUI/DialogSystem.h>

#include <djvUI/Action.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Separator.h>
#include <djvUI/Shortcut.h>
#include <djvUI/StackLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvAV/Render2DSystem.h>

#include <djvCore/Animation.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class DialogWidget : public Layout::IContainer
            {
                DJV_NON_COPYABLE(DialogWidget);

            protected:
                void _init(Context * context)
                {
                    IContainer::_init(context);

                    setPointerEnabled(true);
                    setHAlign(HAlign::Center);
                    setVAlign(VAlign::Center);

                    _titleLabel = Label::create(context);
                    _titleLabel->setFontSizeRole(Style::MetricsRole::FontLarge);
                    _titleLabel->setTextHAlign(TextHAlign::Left);
                    _titleLabel->setMargin(Layout::Margin(
                        Style::MetricsRole::Margin,
                        Style::MetricsRole::None,
                        Style::MetricsRole::MarginSmall,
                        Style::MetricsRole::MarginSmall));
                    _titleLabel->setBackgroundRole(Style::ColorRole::BackgroundHeader);
                    
                    _childLayout = Layout::Vertical::create(context);
                    _childLayout->setSpacing(Style::MetricsRole::None);

                    _layout = Layout::Vertical::create(context);
                    _layout->setSpacing(Style::MetricsRole::None);
                    _layout->addWidget(_titleLabel);
                    _layout->addSeparator();
                    _layout->addWidget(_childLayout, Layout::RowStretch::Expand);
                    IContainer::addWidget(_layout);
                }

                DialogWidget()
                {}

            public:
                static std::shared_ptr<DialogWidget> create(Context * context)
                {
                    auto out = std::shared_ptr<DialogWidget>(new DialogWidget);
                    out->_init(context);
                    return out;
                }

                void setTitle(const std::string & text)
                {
                    _titleLabel->setText(text);
                }

                void addWidget(const std::shared_ptr<Widget>& value) override
                {
                    _childLayout->addWidget(value);
                }

                void removeWidget(const std::shared_ptr<Widget>& value) override
                {
                    _childLayout->removeWidget(value);
                }

                void clearWidgets() override
                {
                    _childLayout->clearWidgets();
                }

            protected:
                void _preLayoutEvent(Event::PreLayout&) override
                {
                    _setMinimumSize(_layout->getMinimumSize());
                }

                void _layoutEvent(Event::Layout&) override
                {
                    _layout->setGeometry(getGeometry());
                }

            private:
                std::shared_ptr<Label> _titleLabel;
                std::shared_ptr<Layout::Vertical> _childLayout;
                std::shared_ptr<Layout::Vertical> _layout;
            };

            class MessageDialog : public DialogWidget
            {
                DJV_NON_COPYABLE(MessageDialog);

            protected:
                void _init(Context * context)
                {
                    DialogWidget::_init(context);

                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(Style::MetricsRole::MarginLarge);

                    _closeButton = Button::Push::create(context);

                    auto layout = Layout::Vertical::create(context);
                    layout->setMargin(Style::MetricsRole::Margin);
                    layout->setBackgroundRole(Style::ColorRole::Background);
                    layout->addWidget(_textBlock);
                    layout->addWidget(_closeButton);
                    addWidget(layout);
                }

                MessageDialog()
                {}

            public:
                static std::shared_ptr<MessageDialog> create(Context * context)
                {
                    auto out = std::shared_ptr<MessageDialog>(new MessageDialog);
                    out->_init(context);
                    return out;
                }
                
                void setText(const std::string & text)
                {
                    _textBlock->setText(text);
                }

                void setCloseText(const std::string & text)
                {
                    _closeButton->setText(text);
                }

                void setCloseCallback(const std::function<void(void)> & value)
                {
                    _closeButton->setClickedCallback(value);
                }

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<Button::Push> _closeButton;
            };

            class ConfirmationDialog : public DialogWidget
            {
                DJV_NON_COPYABLE(ConfirmationDialog);

            protected:
                void _init(Context * context)
                {
                    DialogWidget::_init(context);
                    
                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(Style::MetricsRole::MarginLarge);

                    _acceptButton = Button::Push::create(context);
                    _cancelButton = Button::Push::create(context);

                    auto layout = Layout::Vertical::create(context);
                    layout->setMargin(Style::MetricsRole::Margin);
                    layout->setBackgroundRole(Style::ColorRole::Background);
                    layout->addWidget(_textBlock, Layout::RowStretch::Expand);
                    auto hLayout = Layout::Horizontal::create(context);
                    hLayout->addWidget(_acceptButton, Layout::RowStretch::Expand);
                    hLayout->addWidget(_cancelButton, Layout::RowStretch::Expand);
                    layout->addWidget(hLayout);
                    addWidget(layout);
                }

                ConfirmationDialog()
                {}

            public:
                static std::shared_ptr<ConfirmationDialog> create(Context * context)
                {
                    auto out = std::shared_ptr<ConfirmationDialog>(new ConfirmationDialog);
                    out->_init(context);
                    return out;
                }

                void setText(const std::string & text)
                {
                    _textBlock->setText(text);
                }

                void setAcceptText(const std::string & text)
                {
                    _acceptButton->setText(text);
                }

                void setCancelText(const std::string & text)
                {
                    _cancelButton->setText(text);
                }

                void setAcceptCallback(const std::function<void(void)> & value)
                {
                    _acceptButton->setClickedCallback(value);
                }

                void setCancelCallback(const std::function<void(void)> & value)
                {
                    _cancelButton->setClickedCallback(value);
                }

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<Button::Push> _acceptButton;
                std::shared_ptr<Button::Push> _cancelButton;
            };

        } // namespace

        struct DialogSystem::Private
        {
            std::shared_ptr<MessageDialog> messageDialog;
            std::shared_ptr<Layout::Overlay> messageOverlay;
            std::shared_ptr<ConfirmationDialog> confirmationDialog;
            std::shared_ptr<Layout::Overlay> confirmationOverlay;
        };

        void DialogSystem::_init(Context * context)
        {
            ISystem::_init("djv::UI::DialogSystem", context);
        }

        DialogSystem::DialogSystem() :
            _p(new Private)
        {}

        DialogSystem::~DialogSystem()
        {}

        std::shared_ptr<DialogSystem> DialogSystem::create(Context * context)
        {
            auto out = std::shared_ptr<DialogSystem>(new DialogSystem);
            out->_init(context);
            return out;
        }

        void DialogSystem::message(
            const std::string & title,
            const std::string & text,
            const std::string & closeText)
        {
            auto context = getContext();
            DJV_PRIVATE_PTR();
            if (!p.messageDialog)
            {
                p.messageDialog = MessageDialog::create(context);
                p.messageOverlay = Layout::Overlay::create(context);
                p.messageOverlay->addWidget(p.messageDialog);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    p.messageDialog->setTitle(title);
                    p.messageDialog->setText(text);
                    p.messageDialog->setCloseText(closeText);
                    window->addWidget(p.messageOverlay);
                    p.messageOverlay->show();
                    auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                    p.messageDialog->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->messageOverlay);
                        }
                    });
                    p.messageOverlay->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->messageOverlay);
                        }
                    });
                }
            }
        }

        void DialogSystem::confirmation(
            const std::string & title,
            const std::string & text,
            const std::string & acceptText,
            const std::string & cancelText,
            const std::function<void(bool)> & callback)
        {
            auto context = getContext();
            DJV_PRIVATE_PTR();
            if (!p.confirmationDialog)
            {
                p.confirmationDialog = ConfirmationDialog::create(context);
                p.confirmationOverlay = Layout::Overlay::create(context);
                p.confirmationOverlay->addWidget(p.confirmationDialog);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    p.confirmationDialog->setTitle(title);
                    p.confirmationDialog->setText(text);
                    p.confirmationDialog->setAcceptText(acceptText);
                    p.confirmationDialog->setCancelText(cancelText);
                    window->addWidget(p.confirmationOverlay);
                    p.confirmationOverlay->show();
                    auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                    p.confirmationDialog->setAcceptCallback(
                        [weak, window, callback]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->confirmationOverlay);
                            callback(true);
                        }
                    });
                    p.confirmationDialog->setCancelCallback(
                        [weak, window, callback]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->confirmationOverlay);
                            callback(false);
                        }
                    });
                    p.confirmationOverlay->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->confirmationOverlay);
                        }
                    });
                }
            }
        }

    } // namespace UI
} // namespace djv

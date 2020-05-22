// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/DialogSystem.h>

#include <djvUI/IconSystem.h>
#include <djvUI/IDialog.h>
#include <djvUI/Label.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/Window.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace
        {
            class MessageDialog : public IDialog
            {
                DJV_NON_COPYABLE(MessageDialog);

            protected:
                void _init(const std::shared_ptr<Core::Context>& context)
                {
                    IDialog::_init(context);

                    setClassName("djv::UI::MessageDialog");
                    setFillLayout(false);

                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(MetricsRole::MarginLarge);

                    _closeButton = PushButton::create(context);

                    auto layout = VerticalLayout::create(context);
                    layout->setMargin(MetricsRole::Margin);
                    layout->addChild(_textBlock);
                    layout->addChild(_closeButton);
                    addChild(layout);
                    setStretch(layout, RowStretch::Expand);

                    auto weak = std::weak_ptr<MessageDialog>(std::dynamic_pointer_cast<MessageDialog>(shared_from_this()));
                    _closeButton->setClickedCallback(
                        [weak]
                    {
                        if (auto dialog = weak.lock())
                        {
                            dialog->_doCloseCallback();
                        }
                    });
                }

                MessageDialog()
                {}

            public:
                static std::shared_ptr<MessageDialog> create(const std::shared_ptr<Core::Context>& context)
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

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<PushButton> _closeButton;
            };

            class ConfirmationDialog : public IDialog
            {
                DJV_NON_COPYABLE(ConfirmationDialog);

            protected:
                void _init(const std::shared_ptr<Core::Context>& context)
                {
                    IDialog::_init(context);

                    setClassName("djv::UI::ConfirmationDialog");
                    setFillLayout(false);
                    
                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(MetricsRole::MarginLarge);

                    _acceptButton = PushButton::create(context);
                    _cancelButton = PushButton::create(context);

                    auto layout = VerticalLayout::create(context);
                    layout->setMargin(MetricsRole::Margin);
                    layout->addChild(_textBlock);
                    layout->setStretch(_textBlock, RowStretch::Expand);
                    auto hLayout = HorizontalLayout::create(context);
                    hLayout->addChild(_acceptButton);
                    hLayout->setStretch(_acceptButton, RowStretch::Expand);
                    hLayout->addChild(_cancelButton);
                    hLayout->setStretch(_cancelButton, RowStretch::Expand);
                    layout->addChild(hLayout);
                    addChild(layout);
                    setStretch(layout, RowStretch::Expand);

                    auto weak = std::weak_ptr<ConfirmationDialog>(std::dynamic_pointer_cast<ConfirmationDialog>(shared_from_this()));
                    _acceptButton->setClickedCallback(
                        [weak]
                    {
                        if (auto dialog = weak.lock())
                        {
                            if (dialog->_acceptCallback)
                            {
                                dialog->_acceptCallback();
                            }
                            dialog->_doCloseCallback();
                        }
                    });
                    _cancelButton->setClickedCallback(
                        [weak]
                    {
                        if (auto dialog = weak.lock())
                        {
                            if (dialog->_cancelCallback)
                            {
                                dialog->_cancelCallback();
                            }
                            dialog->_doCloseCallback();
                        }
                    });
                }

                ConfirmationDialog()
                {}

            public:
                static std::shared_ptr<ConfirmationDialog> create(const std::shared_ptr<Core::Context>& context)
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
                    _acceptCallback = value;
                }

                void setCancelCallback(const std::function<void(void)> & value)
                {
                    _cancelCallback = value;
                }

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<PushButton> _acceptButton;
                std::shared_ptr<PushButton> _cancelButton;
                std::function<void(void)> _acceptCallback;
                std::function<void(void)> _cancelCallback;
            };

        } // namespace

        struct DialogSystem::Private
        {
            std::shared_ptr<Window> messageWindow;
            std::shared_ptr<Window> confirmationWindow;
        };

        void DialogSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            ISystem::_init("djv::UI::DialogSystem", context);

            addDependency(context->getSystemT<IconSystem>());
        }

        DialogSystem::DialogSystem() :
            _p(new Private)
        {}

        DialogSystem::~DialogSystem()
        {
            DJV_PRIVATE_PTR();
            if (p.messageWindow)
            {
                p.messageWindow->close();
            }
            if (p.confirmationWindow)
            {
                p.confirmationWindow->close();
            }
        }

        std::shared_ptr<DialogSystem> DialogSystem::create(const std::shared_ptr<Core::Context>& context)
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
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.messageWindow)
                {
                    p.messageWindow->close();
                    p.messageWindow.reset();
                }
                auto messageDialog = MessageDialog::create(context);
                messageDialog->setTitle(title);
                messageDialog->setText(text);
                messageDialog->setCloseText(closeText);
                auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                messageDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->messageWindow->close();
                            system->_p->messageWindow.reset();
                        }
                    });
                p.messageWindow = Window::create(context);
                p.messageWindow->setBackgroundRole(ColorRole::None);
                p.messageWindow->addChild(messageDialog);
                p.messageWindow->show();
            }
        }

        void DialogSystem::confirmation(
            const std::string & title,
            const std::string & text,
            const std::string & acceptText,
            const std::string & cancelText,
            const std::function<void(bool)> & callback)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                if (p.confirmationWindow)
                {
                    p.confirmationWindow->close();
                    p.confirmationWindow.reset();
                }
                auto confirmationDialog = ConfirmationDialog::create(context);
                confirmationDialog->setTitle(title);
                confirmationDialog->setText(text);
                confirmationDialog->setAcceptText(acceptText);
                confirmationDialog->setCancelText(cancelText);
                confirmationDialog->setAcceptCallback(
                    [callback]
                {
                    callback(true);
                });
                confirmationDialog->setCancelCallback(
                    [callback]
                {
                    callback(false);
                });
                auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                confirmationDialog->setCloseCallback(
                    [weak]
                    {
                        if (auto system = weak.lock())
                        {
                            system->_p->confirmationWindow->close();
                            system->_p->confirmationWindow.reset();
                        }
                    });
                p.confirmationWindow = Window::create(context);
                p.confirmationWindow->setBackgroundRole(ColorRole::None);
                p.confirmationWindow->addChild(confirmationDialog);
                p.confirmationWindow->show();
            }
        }

    } // namespace UI
} // namespace djv

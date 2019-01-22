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

#include <djvUI/FileBrowser.h>
#include <djvUI/IDialog.h>
#include <djvUI/IWindowSystem.h>
#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/PushButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/TextBlock.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

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
                void _init(Context * context)
                {
                    IDialog::_init(context);

                    setFillLayout(false);

                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(Style::MetricsRole::MarginLarge);

                    _closeButton = Button::Push::create(context);

                    auto layout = Layout::Vertical::create(context);
                    layout->setMargin(Style::MetricsRole::Margin);
                    layout->addWidget(_textBlock);
                    layout->addWidget(_closeButton);
                    addWidget(layout, Layout::RowStretch::Expand);

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

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<Button::Push> _closeButton;
            };

            class ConfirmationDialog : public IDialog
            {
                DJV_NON_COPYABLE(ConfirmationDialog);

            protected:
                void _init(Context * context)
                {
                    IDialog::_init(context);

                    setFillLayout(false);
                    
                    _textBlock = TextBlock::create(context);
                    _textBlock->setTextHAlign(TextHAlign::Center);
                    _textBlock->setMargin(Style::MetricsRole::MarginLarge);

                    _acceptButton = Button::Push::create(context);
                    _cancelButton = Button::Push::create(context);

                    auto layout = Layout::Vertical::create(context);
                    layout->setMargin(Style::MetricsRole::Margin);
                    layout->addWidget(_textBlock, Layout::RowStretch::Expand);
                    auto hLayout = Layout::Horizontal::create(context);
                    hLayout->addWidget(_acceptButton, Layout::RowStretch::Expand);
                    hLayout->addWidget(_cancelButton, Layout::RowStretch::Expand);
                    layout->addWidget(hLayout);
                    addWidget(layout, Layout::RowStretch::Expand);

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
                    _acceptCallback = value;
                }

                void setCancelCallback(const std::function<void(void)> & value)
                {
                    _cancelCallback = value;
                }

            private:
                std::shared_ptr<TextBlock> _textBlock;
                std::shared_ptr<Button::Push> _acceptButton;
                std::shared_ptr<Button::Push> _cancelButton;
                std::function<void(void)> _acceptCallback;
                std::function<void(void)> _cancelCallback;
            };

            class FileBrowserDialog : public IDialog
            {
                DJV_NON_COPYABLE(FileBrowserDialog);

            protected:
                void _init(Context * context)
                {
                    IDialog::_init(context);

                    _widget = FileBrowser::Widget::create(context);
                    _widget->setPath(FileSystem::Path("."));
                    _widget->setBackgroundRole(Style::ColorRole::Background);
                    addWidget(_widget, Layout::RowStretch::Expand);

                    auto weak = std::weak_ptr<FileBrowserDialog>(std::dynamic_pointer_cast<FileBrowserDialog>(shared_from_this()));
                    _widget->setCallback(
                        [weak](const FileSystem::FileInfo & value)
                    {
                        if (auto dialog = weak.lock())
                        {
                            if (dialog->_callback)
                            {
                                dialog->_callback(value);
                            }
                            dialog->_doCloseCallback();
                        }
                    });
                }

                FileBrowserDialog()
                {}

            public:
                static std::shared_ptr<FileBrowserDialog> create(Context * context)
                {
                    auto out = std::shared_ptr<FileBrowserDialog>(new FileBrowserDialog);
                    out->_init(context);
                    return out;
                }

                void setCallback(const std::function<void(const FileSystem::FileInfo &)> & value)
                {
                    _callback = value;
                }

            private:
                std::shared_ptr<FileBrowser::Widget> _widget;
                std::function<void(const FileSystem::FileInfo &)> _callback;
            };

        } // namespace

        struct DialogSystem::Private
        {
            std::shared_ptr<MessageDialog> messageDialog;
            std::shared_ptr<ConfirmationDialog> confirmationDialog;
            std::shared_ptr<FileBrowserDialog> fileBrowserDialog;
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
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    p.messageDialog->setTitle(title);
                    p.messageDialog->setText(text);
                    p.messageDialog->setCloseText(closeText);

                    auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                    p.messageDialog->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->messageDialog);
                        }
                    });

                    window->addWidget(p.messageDialog);
                    p.messageDialog->show();
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
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    p.confirmationDialog->setTitle(title);
                    p.confirmationDialog->setText(text);
                    p.confirmationDialog->setAcceptText(acceptText);
                    p.confirmationDialog->setCancelText(cancelText);

                    auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                    p.confirmationDialog->setAcceptCallback(
                        [callback]
                    {
                        callback(true);
                    });
                    p.confirmationDialog->setCancelCallback(
                        [callback]
                    {
                        callback(false);
                    });
                    p.confirmationDialog->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->confirmationDialog);
                        }
                    });

                    window->addWidget(p.confirmationDialog);
                    p.confirmationDialog->show();
                }
            }
        }

        void DialogSystem::fileBrowser(
            const std::string & title,
            const std::function<void(const Core::FileSystem::FileInfo &)> & callback)
        {
            auto context = getContext();
            DJV_PRIVATE_PTR();
            if (!p.fileBrowserDialog)
            {
                p.fileBrowserDialog = FileBrowserDialog::create(context);
            }
            if (auto windowSystem = context->getSystemT<UI::IWindowSystem>().lock())
            {
                if (auto window = windowSystem->observeCurrentWindow()->get())
                {
                    p.fileBrowserDialog->setTitle(title);

                    auto weak = std::weak_ptr<DialogSystem>(std::dynamic_pointer_cast<DialogSystem>(shared_from_this()));
                    p.fileBrowserDialog->setCallback(
                        [callback](const Core::FileSystem::FileInfo & value)
                    {
                        callback(value);
                    });
                    p.fileBrowserDialog->setCloseCallback(
                        [weak, window]
                    {
                        if (auto system = weak.lock())
                        {
                            window->removeWidget(system->_p->fileBrowserDialog);
                        }
                    });

                    window->addWidget(p.fileBrowserDialog);
                    p.fileBrowserDialog->show();
                }
            }
        }

    } // namespace UI
} // namespace djv

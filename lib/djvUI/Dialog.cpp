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

#include <djvUI/Dialog.h>

#include <djvUI/Action.h>
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
        struct Dialog::Private
        {
            std::shared_ptr<StackLayout> layout;
            std::shared_ptr<Animation> fadeAnimation;
        };

        void Dialog::_init(Context * context)
        {
            IContainerWidget::_init(context);
            
            setClassName("djv::UI::Dialog");
            setVisible(false);
            setOpacity(0.f);
            setBackgroundRole(ColorRole::Overlay);
            setPointerEnabled(true);

            auto closeShortcut = Shortcut::create(GLFW_KEY_ESCAPE);
            auto closeAction = Action::create();
            closeAction->setShortcut(closeShortcut);
            addAction(closeAction);

            _p->layout = StackLayout::create(context);
            IContainerWidget::addWidget(_p->layout);

            _p->fadeAnimation = Animation::create(context);

            auto weak = std::weak_ptr<Dialog>(std::dynamic_pointer_cast<Dialog>(shared_from_this()));
            closeShortcut->setCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->close();
                }
            });
        }

        Dialog::Dialog() :
            _p(new Private)
        {}

        Dialog::~Dialog()
        {}

        std::shared_ptr<Dialog> Dialog::create(Context * context)
        {
            auto out = std::shared_ptr<Dialog>(new Dialog);
            out->_init(context);
            return out;
        }

        void Dialog::close()
        {
            if (auto window = getWindow().lock())
            {
                window->removeWidget(std::dynamic_pointer_cast<Widget>(shared_from_this()));
            }
        }

        void Dialog::addWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->addWidget(value);
        }

        void Dialog::removeWidget(const std::shared_ptr<Widget>& value)
        {
            _p->layout->removeWidget(value);
        }

        void Dialog::clearWidgets()
        {
            _p->layout->clearWidgets();
        }

        void Dialog::setVisible(bool value)
        {
            IContainerWidget::setVisible(value);
            if (value && _p->fadeAnimation)
            {
                auto weak = std::weak_ptr<Dialog>(std::dynamic_pointer_cast<Dialog>(shared_from_this()));
                _p->fadeAnimation->start(
                    getOpacity(),
                    1.f,
                    std::chrono::milliseconds(100),
                    [weak](float value)
                {
                    if (auto dialog = weak.lock())
                    {
                        dialog->setOpacity(value);
                    }
                },
                    [weak](float value)
                {
                    if (auto dialog = weak.lock())
                    {
                        dialog->setOpacity(value);
                    }
                });
            }
        }

        float Dialog::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void Dialog::preLayoutEvent(PreLayoutEvent& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void Dialog::layoutEvent(LayoutEvent& event)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void Dialog::buttonPressEvent(ButtonPressEvent&)
        {
            close();
        }

        void Dialog::keyPressEvent(Core::KeyPressEvent& event)
        {
            Widget::keyPressEvent(event);
            event.accept();
        }

        void Dialog::keyReleaseEvent(Core::KeyReleaseEvent& event)
        {
            event.accept();
        }

        namespace
        {
            class DialogWidget : public StackLayout
            {
                DJV_NON_COPYABLE(DialogWidget);

            protected:
                void _init(Context * context)
                {
                    StackLayout::_init(context);
                    
                    setBackgroundRole(ColorRole::Background);
                    setHAlign(HAlign::Center);
                    setVAlign(VAlign::Center);
                    setMargin(MetricsRole::Margin);
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

                void preLayoutEvent(Core::PreLayoutEvent& event) override
                {
                    StackLayout::preLayoutEvent(event);
                    if (auto style = _getStyle().lock())
                    {
                        const auto size = style->getMetric(MetricsRole::Dialog);
                        const auto minimumSize = getMinimumSize();
                        _setMinimumSize(glm::vec2(std::max(size, minimumSize.x), minimumSize.y));
                    }
                }
            };

        } // namespace

        void messageDialog(
            const std::string & text,
            const std::string & closeText,
            const std::shared_ptr<Window> & window)
        {
            auto context = window->getContext();

            auto textBlock = TextBlock::create(context);
            textBlock->setText(text);
            textBlock->setTextHAlign(TextHAlign::Center);
            textBlock->setMargin(MetricsRole::Margin);

            auto closeButton = PushButton::create(context);
            closeButton->setText(closeText);
            
            auto layout = VerticalLayout::create(context);
            layout->setBackgroundRole(ColorRole::Background);
            layout->setMargin(MetricsRole::Margin);
            layout->setVAlign(VAlign::Center);
            layout->addWidget(textBlock);
            layout->addWidget(closeButton);
            
            auto dialogWidget = DialogWidget::create(context);
            dialogWidget->addWidget(layout);

            auto dialog = Dialog::create(context);
            dialog->addWidget(dialogWidget);
            window->addWidget(dialog);

            dialog->show();

            closeButton->setClickedCallback(
                [dialog]
            {
                dialog->close();
            });
        }

        void confirmationDialog(
            const std::string & text,
            const std::string & acceptText,
            const std::string & cancelText,
            const std::shared_ptr<Window> & window,
            const std::function<void(bool)> & callback)
        {
            auto context = window->getContext();

            auto textBlock = TextBlock::create(context);
            textBlock->setText(text);
            textBlock->setTextHAlign(TextHAlign::Center);
            textBlock->setMargin(MetricsRole::Margin);

            auto acceptButton = PushButton::create(context);
            acceptButton->setText(acceptText);

            auto cancelButton = PushButton::create(context);
            cancelButton->setText(cancelText);

            auto layout = VerticalLayout::create(context);
            layout->addWidget(textBlock);
            auto hLayout = HorizontalLayout::create(context);
            hLayout->setHAlign(HAlign::Center);
            hLayout->addWidget(acceptButton);
            hLayout->addWidget(cancelButton);
            layout->addWidget(hLayout);

            auto dialogWidget = DialogWidget::create(context);
            dialogWidget->addWidget(layout);

            auto dialog = Dialog::create(context);
            dialog->addWidget(dialogWidget);
            window->addWidget(dialog);

            dialog->show();

            acceptButton->setClickedCallback(
                [dialog, callback]
            {
                dialog->close();
                callback(true);
            });
            cancelButton->setClickedCallback(
                [dialog, callback]
            {
                dialog->close();
                callback(false);
            });
        }

    } // namespace UI
} // namespace djv

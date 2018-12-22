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

                void preLayoutEvent(Event::PreLayout& event) override
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

            auto overlay = Overlay::create(context);
            overlay->setBackgroundRole(ColorRole::Overlay);
            overlay->addWidget(dialogWidget);
            window->addWidget(overlay);

            overlay->show();

            closeButton->setClickedCallback(
                [overlay]
            {
                overlay->close();
            });
            overlay->setCloseCallback(
                [window, overlay]
            {
                window->removeWidget(overlay);
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

            auto overlay = Overlay::create(context);
            overlay->setBackgroundRole(ColorRole::Overlay);
            overlay->addWidget(dialogWidget);
            window->addWidget(overlay);

            overlay->show();

            acceptButton->setClickedCallback(
                [overlay, callback]
            {
                overlay->close();
                callback(true);
            });
            cancelButton->setClickedCallback(
                [overlay, callback]
            {
                overlay->close();
                callback(false);
            });
            overlay->setCloseCallback(
                [window, overlay]
            {
                window->removeWidget(overlay);
            });
        }

    } // namespace UI
} // namespace djv

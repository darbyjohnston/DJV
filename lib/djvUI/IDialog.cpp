// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/IDialog.h>

#include <djvUI/Label.h>
#include <djvUI/Overlay.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>
#include <djvUI/Window.h>

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
                void _init(const std::shared_ptr<System::Context>& context)
                {
                    Vertical::_init(context);
                    setPointerEnabled(true);
                }
                
                DialogLayout()
                {}

            public:
                static std::shared_ptr<DialogLayout> create(const std::shared_ptr<System::Context>& context)
                {
                    auto out = std::shared_ptr<DialogLayout>(new DialogLayout);
                    out->_init(context);
                    return out;
                }

            protected:
                void _buttonPressEvent(System::Event::ButtonPress& event) override
                {
                    event.accept();
                }

                void _buttonReleaseEvent(System::Event::ButtonRelease& event) override
                {
                    event.accept();
                }
            };

        } // namespace

        struct IDialog::Private
        {
            std::shared_ptr<Text::Label> titleLabel;
            std::shared_ptr<ToolButton> closeButton;
            std::shared_ptr<HorizontalLayout> titleBarLayout;
            std::shared_ptr<VerticalLayout> childLayout;
            std::shared_ptr<VerticalLayout> layout;
            std::shared_ptr<Layout::Overlay> overlay;
            std::shared_ptr<StackLayout> overlayLayout;
            std::function<void(void)> closeCallback;
        };

        void IDialog::_init(const std::shared_ptr<System::Context>& context)
        {
            Window::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::IDialog");
            setBackgroundRole(ColorRole::None);

            p.titleLabel = Text::Label::create(context);
            p.titleLabel->setFontSizeRole(MetricsRole::FontHeader);
            p.titleLabel->setTextHAlign(TextHAlign::Left);
            p.titleLabel->setMargin(Layout::Margin(MetricsRole::Margin, MetricsRole::Margin, MetricsRole::MarginSmall, MetricsRole::MarginSmall));
            p.titleLabel->hide();

            p.closeButton = ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.titleBarLayout = HorizontalLayout::create(context);
            p.titleBarLayout->setSpacing(MetricsRole::None);

            p.childLayout = VerticalLayout::create(context);
            p.childLayout->setSpacing(MetricsRole::None);

            p.layout = VerticalLayout::create(context);
            p.layout->setMargin(MetricsRole::MarginDialog);
            auto layout = DialogLayout::create(context);
            layout->setSpacing(MetricsRole::None);
            layout->setBackgroundRole(ColorRole::Background);
            auto hLayout = HorizontalLayout::create(context);
            hLayout->setSpacing(MetricsRole::None);
            hLayout->setBackgroundRole(ColorRole::BackgroundHeader);
            hLayout->addChild(p.titleLabel);
            hLayout->addExpander();
            hLayout->addChild(p.titleBarLayout);
            hLayout->addChild(p.closeButton);
            layout->addChild(hLayout);
            layout->addSeparator();
            layout->addChild(p.childLayout);
            layout->setStretch(p.childLayout);
            p.layout->addChild(layout);
            p.layout->setStretch(layout);

            p.overlay = Layout::Overlay::create(context);
            p.overlay->addChild(p.layout);
            Window::addChild(p.overlay);
            p.overlayLayout = StackLayout::create(context);
            p.overlayLayout->addChild(p.overlay);
            Window::addChild(p.overlayLayout);

            auto weak = std::weak_ptr<IDialog>(std::dynamic_pointer_cast<IDialog>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                    dialog->hide();
                }
            });
            p.overlay->setCloseCallback(
                [weak]
            {
                if (auto dialog = weak.lock())
                {
                    dialog->_doCloseCallback();
                    dialog->hide();
                }
            });
        }

        IDialog::IDialog() :
            _p(new Private)
        {}

        IDialog::~IDialog()
        {}

        void IDialog::setTitle(const std::string& text)
        {
            DJV_PRIVATE_PTR();
            p.titleLabel->setText(text);
            p.titleLabel->setVisible(!text.empty());
        }

        void IDialog::addTitleBarWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->titleBarLayout->addChild(widget);
        }

        void IDialog::removeTitleBarWidget(const std::shared_ptr<Widget>& widget)
        {
            _p->titleBarLayout->removeChild(widget);
        }

        void IDialog::clearTitleBarWidgets()
        {
            _p->titleBarLayout->clearChildren();
        }

        void IDialog::setStretch(const std::shared_ptr<Widget>& widget, RowStretch value)
        {
            _p->childLayout->setStretch(widget, value);
        }

        void IDialog::setFillLayout(bool value)
        {
            DJV_PRIVATE_PTR();
            p.layout->setHAlign(value ? HAlign::Fill : HAlign::Center);
            p.layout->setVAlign(value ? VAlign::Fill : VAlign::Center);
        }

        void IDialog::setCloseCallback(const std::function<void(void)>& value)
        {
            _p->closeCallback = value;
        }

        void IDialog::setVisible(bool value)
        {
            Window::setVisible(value);
            DJV_PRIVATE_PTR();
            moveToFront();
            if (p.overlay)
            {
                p.overlay->setVisible(value);
            }
        }

        float IDialog::getHeightForWidth(float value) const
        {
            return _p->overlay->getHeightForWidth(value);
        }

        void IDialog::addChild(const std::shared_ptr<IObject>& value)
        {
            if (auto overlay = std::dynamic_pointer_cast<Layout::Overlay>(value))
            {
                _p->overlayLayout->addChild(value);
            }
            else
            {
                _p->childLayout->addChild(value);
            }
        }

        void IDialog::removeChild(const std::shared_ptr<IObject>& value)
        {
            if (auto overlay = std::dynamic_pointer_cast<Layout::Overlay>(value))
            {
                _p->overlayLayout->removeChild(value);
            }
            else
            {
                _p->childLayout->removeChild(value);
            }
        }

        void IDialog::clearChildren()
        {
            _p->childLayout->clearChildren();
            _p->overlayLayout->clearChildren();
        }

        void IDialog::_doCloseCallback()
        {
            if (_p->closeCallback)
            {
                _p->closeCallback();
            }
        }

        void IDialog::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->overlay->getMinimumSize());
        }

        void IDialog::_layoutEvent(System::Event::Layout&)
        {
            _p->overlay->setGeometry(getGeometry());
            _p->overlayLayout->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv

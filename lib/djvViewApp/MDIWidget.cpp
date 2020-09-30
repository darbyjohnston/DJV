// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/Label.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>
#include <djvUI/ToolButton.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MDIWidget::Private
        {
            std::shared_ptr<UI::Label> titleLabel;
            std::shared_ptr<UI::ToolButton> closeButton;
            std::shared_ptr<UI::HorizontalLayout> titleBar;
            std::shared_ptr<UI::VerticalLayout> childLayout;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::function<void(void)> closeCallback;

            std::shared_ptr<Observer::Value<float> > fadeObserver;
        };

        void MDIWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            IWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MDIWidget");

            p.titleLabel = UI::Label::create(context);
            p.titleLabel->setTextHAlign(UI::TextHAlign::Left);
            p.titleLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall, UI::MetricsRole::MarginSmall, UI::MetricsRole::None, UI::MetricsRole::None));

            p.closeButton = UI::ToolButton::create(context);
            p.closeButton->setIcon("djvIconClose");

            p.titleBar = UI::HorizontalLayout::create(context);
            p.titleBar->setBackgroundRole(UI::ColorRole::BackgroundHeader);
            p.titleBar->setSpacing(UI::MetricsRole::None);
            p.titleBar->addChild(p.titleLabel);
            p.titleBar->setStretch(p.titleLabel, UI::RowStretch::Expand);
            p.titleBar->addChild(p.closeButton);

            p.childLayout = UI::VerticalLayout::create(context);

            auto layout = UI::VerticalLayout::create(context);
            layout->setBackgroundRole(UI::ColorRole::OverlayLight);
            layout->setSpacing(UI::MetricsRole::None);
            layout->addChild(p.titleBar);
            layout->addSeparator();
            layout->addChild(p.childLayout);
            layout->setStretch(p.childLayout, UI::RowStretch::Expand);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::Shadow);
            p.layout->addChild(layout);
            p.layout->setStretch(layout, UI::RowStretch::Expand);
            IWidget::addChild(p.layout);

            auto weak = std::weak_ptr<MDIWidget>(std::dynamic_pointer_cast<MDIWidget>(shared_from_this()));
            p.closeButton->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    widget->close();
                }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.fadeObserver = Observer::Value<float>::create(
                    windowSystem->observeFade(),
                    [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->setOpacity(value);
                        }
                    });
            }
        }

        MDIWidget::MDIWidget() :
            _p(new Private)
        {}

        MDIWidget::~MDIWidget()
        {}

        const std::string & MDIWidget::getTitle() const
        {
            return _p->titleLabel->getText();
        }

        void MDIWidget::setTitle(const std::string & text)
        {
            _p->titleLabel->setText(text);
        }

        void MDIWidget::close()
        {
            DJV_PRIVATE_PTR();
            hide();
            if (p.closeCallback)
            {
                p.closeCallback();
            }
        }

        void MDIWidget::setCloseCallback(const std::function<void(void)> & value)
        {
            _p->closeCallback = value;
        }

        float MDIWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void MDIWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            DJV_PRIVATE_PTR();
            p.childLayout->addChild(value);
            if (auto widget = std::dynamic_pointer_cast<Widget>(value))
            {
                p.childLayout->setStretch(widget, UI::RowStretch::Expand);
            }
        }

        void MDIWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void MDIWidget::clearChildren()
        {
            _p->childLayout->clearChildren();
        }

        std::map<UI::MDI::Handle, std::vector<Math::BBox2f> > MDIWidget::_getHandles() const
        {
            DJV_PRIVATE_PTR();
            auto out = IWidget::_getHandles();
            out[UI::MDI::Handle::Move][0] = p.titleBar->getGeometry();
            return out;
        }

        void MDIWidget::_setActiveWidget(bool value)
        {
            IWidget::_setActiveWidget(value);
            DJV_PRIVATE_PTR();
            p.titleLabel->setTextColorRole(value ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
            p.closeButton->setForegroundColorRole(value ? UI::ColorRole::Foreground : UI::ColorRole::ForegroundDim);
        }

        void MDIWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void MDIWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void MDIWidget::_initEvent(System::Event::Init & event)
        {
            IWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.closeButton->setTooltip(_getText(DJV_TEXT("widget_mdi_close_tooltip")));
            }
        }

    } // namespace ViewApp
} // namespace djv


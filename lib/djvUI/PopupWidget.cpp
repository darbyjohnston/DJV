// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/PopupWidget.h>

#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PopupWidget::Private
        {
            std::shared_ptr<VerticalLayout> layout;
        };

        void PopupWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::PopupWidget");
            setBackgroundRole(ColorRole::Background);
            setPointerEnabled(true);

            p.layout = VerticalLayout::create(context);
            Widget::addChild(p.layout);
        }

        PopupWidget::PopupWidget() :
            _p(new Private)
        {}

        PopupWidget::~PopupWidget()
        {}

        std::shared_ptr<PopupWidget> PopupWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<PopupWidget>(new PopupWidget);
            out->_init(context);
            return out;
        }

        void PopupWidget::addChild(const std::shared_ptr<IObject>& value)
        {
            _p->layout->addChild(value);
        }

        void PopupWidget::removeChild(const std::shared_ptr<IObject>& value)
        {
            _p->layout->removeChild(value);
        }

        void PopupWidget::clearChildren()
        {
            _p->layout->clearChildren();
        }

        float PopupWidget::getHeightForWidth(float value) const
        {
            return _p->layout->getHeightForWidth(value);
        }

        void PopupWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void PopupWidget::_layoutEvent(Event::Layout& event)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void PopupWidget::_buttonPressEvent(Event::ButtonPress& event)
        {
            event.accept();
        }

        void PopupWidget::_buttonReleaseEvent(Event::ButtonRelease& event)
        {
            event.accept();
        }

    } // namespace UI
} // namespace djv

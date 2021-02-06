// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvUI/PopupWidget.h>

#include <djvUI/Border.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct PopupWidget::Private
        {
            std::shared_ptr<Border> border;
        };

        void PopupWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            
            setClassName("djv::UI::PopupWidget");
            setBackgroundColorRole(ColorRole::Background);
            setPointerEnabled(true);

            p.border = Border::create(context);
            Widget::addChild(p.border);
        }

        PopupWidget::PopupWidget() :
            _p(new Private)
        {}

        PopupWidget::~PopupWidget()
        {}

        std::shared_ptr<PopupWidget> PopupWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<PopupWidget>(new PopupWidget);
            out->_init(context);
            return out;
        }

        void PopupWidget::addChild(const std::shared_ptr<IObject>& value)
        {
            _p->border->addChild(value);
        }

        void PopupWidget::removeChild(const std::shared_ptr<IObject>& value)
        {
            _p->border->removeChild(value);
        }

        void PopupWidget::clearChildren()
        {
            _p->border->clearChildren();
        }

        float PopupWidget::getHeightForWidth(float value) const
        {
            return _p->border->getHeightForWidth(value);
        }

        void PopupWidget::_preLayoutEvent(System::Event::PreLayout& event)
        {
            _setMinimumSize(_p->border->getMinimumSize());
        }

        void PopupWidget::_layoutEvent(System::Event::Layout& event)
        {
            _p->border->setGeometry(getGeometry());
        }

        void PopupWidget::_buttonPressEvent(System::Event::ButtonPress& event)
        {
            event.accept();
        }

        void PopupWidget::_buttonReleaseEvent(System::Event::ButtonRelease& event)
        {
            event.accept();
        }

    } // namespace UI
} // namespace djv

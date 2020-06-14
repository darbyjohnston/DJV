// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/ISettingsWidget.h>

#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ISettingsWidget::Private
        {
            std::shared_ptr<VerticalLayout> childLayout;
        };

        void ISettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::UI::ISettingsWidget");

            p.childLayout = VerticalLayout::create(context);
            //p.childLayout->setMargin(MetricsRole::MarginLarge);
            //p.childLayout->setSpacing(MetricsRole::SpacingLarge);
            Widget::addChild(p.childLayout);
        }

        ISettingsWidget::ISettingsWidget() :
            _p(new Private)
        {}

        ISettingsWidget::~ISettingsWidget()
        {}

        float ISettingsWidget::getHeightForWidth(float value) const
        {
            return _p->childLayout->getHeightForWidth(value);
        }

        void ISettingsWidget::addChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->addChild(value);
        }

        void ISettingsWidget::removeChild(const std::shared_ptr<IObject> & value)
        {
            _p->childLayout->removeChild(value);
        }

        void ISettingsWidget::clearChildren()
        {
            _p->childLayout->clearChildren();
        }

        void ISettingsWidget::_preLayoutEvent(Event::PreLayout& event)
        {
            _setMinimumSize(_p->childLayout->getMinimumSize());
        }

        void ISettingsWidget::_layoutEvent(Event::Layout&)
        {
            _p->childLayout->setGeometry(getGeometry());
        }

    } // namespace UI
} // namespace djv


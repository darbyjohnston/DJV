// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUIComponents/SettingsIWidget.h>

#include <djvUI/RowLayout.h>

using namespace djv::Core;

namespace djv
{
    namespace UIComponents
    {
        namespace Settings
        {
            struct IWidget::Private
            {
                std::shared_ptr<UI::VerticalLayout> childLayout;
            };

            void IWidget::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);

                DJV_PRIVATE_PTR();
                setClassName("djv::UIComponents::Settings::IWidget");

                p.childLayout = UI::VerticalLayout::create(context);
                p.childLayout->setSpacing(UI::MetricsRole::SpacingSmall);
                Widget::addChild(p.childLayout);
            }

            IWidget::IWidget() :
                _p(new Private)
            {}

            IWidget::~IWidget()
            {}

            std::string IWidget::getSettingsName() const
            {
                return std::string();
            }

            float IWidget::getHeightForWidth(float value) const
            {
                return _p->childLayout->getHeightForWidth(value);
            }

            void IWidget::addChild(const std::shared_ptr<IObject>& value)
            {
                _p->childLayout->addChild(value);
            }

            void IWidget::removeChild(const std::shared_ptr<IObject>& value)
            {
                _p->childLayout->removeChild(value);
            }

            void IWidget::clearChildren()
            {
                _p->childLayout->clearChildren();
            }

            void IWidget::_preLayoutEvent(System::Event::PreLayout& event)
            {
                _setMinimumSize(_p->childLayout->getMinimumSize());
            }

            void IWidget::_layoutEvent(System::Event::Layout&)
            {
                _p->childLayout->setGeometry(getGeometry());
            }

        } // namespace Settings
    } // namespace UIComponents
} // namespace djv


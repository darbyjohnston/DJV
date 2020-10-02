// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/StackLayout.h>

#include <djvUI/StackLayoutFunc.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Stack::Private
            {};

            void Stack::_init(const std::shared_ptr<System::Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Stack");
            }

            Stack::Stack() :
                _p(new Private)
            {}

            Stack::~Stack()
            {}

            std::shared_ptr<Stack> Stack::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Stack>(new Stack);
                out->_init(context);
                return out;
            }

            float Stack::getHeightForWidth(float value) const
            {
                return stackHeightForWidth(value, getChildWidgets(), getMargin(), _getStyle());
            }

            void Stack::_preLayoutEvent(System::Event::PreLayout&)
            {
                _setMinimumSize(stackMinimumSize(getChildWidgets(), getMargin(), _getStyle()));
            }

            void Stack::_layoutEvent(System::Event::Layout&)
            {
                stackLayout(getGeometry(), getChildWidgets(), getMargin(), _getStyle());
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

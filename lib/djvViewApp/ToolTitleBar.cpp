// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ToolTitleBar.h>

#include <djvUI/Label.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ToolTitleBar::Private
        {
            std::string text;
            std::shared_ptr<UI::Text::Label> label;
        };

        void ToolTitleBar::_init(
            const std::string& text,
            const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ToolTitleBar");
            setBackgroundRole(UI::ColorRole::BackgroundToolBar);

            p.text = text;

            p.label = UI::Text::Label::create(context);
            p.label->setTextHAlign(UI::TextHAlign::Left);
            p.label->setMargin(UI::MetricsRole::MarginSmall);
            addChild(p.label);
        }

        ToolTitleBar::ToolTitleBar() :
            _p(new Private)
        {}

        ToolTitleBar::~ToolTitleBar()
        {}

        std::shared_ptr<ToolTitleBar> ToolTitleBar::create(
            const std::string& text,
            const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ToolTitleBar>(new ToolTitleBar);
            out->_init(text, context);
            return out;
        }

        void ToolTitleBar::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->label->getMinimumSize());
        }

        void ToolTitleBar::_layoutEvent(System::Event::Layout&)
        {
            _p->label->setGeometry(getGeometry());
        }

        void ToolTitleBar::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                p.label->setText(_getText(p.text));
            }
        }

    } // namespace ViewApp
} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUI/SoloLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            std::shared_ptr<ColorSpaceMainWidget> mainWidget;
            std::shared_ptr<ColorSpaceEditConfigsWidget> editConfigsWidget;
            std::shared_ptr<ColorSpaceFileFormatsWidget> fileFormatsWidget;
            std::shared_ptr<UI::SoloLayout> layout;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.mainWidget = ColorSpaceMainWidget::create(context);
            p.editConfigsWidget = ColorSpaceEditConfigsWidget::create(context);
            p.fileFormatsWidget = ColorSpaceFileFormatsWidget::create(context);

            p.layout = UI::SoloLayout::create(context);
            p.layout->addChild(p.mainWidget);
            p.layout->addChild(p.editConfigsWidget);
            p.layout->addChild(p.fileFormatsWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.mainWidget->setEditConfigsCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->editConfigsWidget, UI::Side::Left);
                    }
                });
            p.mainWidget->setFileFormatsCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->fileFormatsWidget, UI::Side::Left);
                    }
                });

            p.editConfigsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainWidget, UI::Side::Right);
                    }
                });

            p.fileFormatsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainWidget, UI::Side::Right);
                    }
                });
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {}

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_space")));
            }
        }

    } // namespace ViewApp
} // namespace djv


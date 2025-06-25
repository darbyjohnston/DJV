// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ColorPickerTool.h>

#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <feather-tk/ui/ColorWidget.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>

namespace djv
{
    namespace app
    {
        struct ColorPickerTool::Private
        {
            std::shared_ptr<feather_tk::ColorWidget> colorWidget;

            std::shared_ptr<feather_tk::ValueObserver<feather_tk::Color4F> > colorPickerObserver;
        };

        void ColorPickerTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::ColorPicker,
                "djv::app::ColorPickerTool",
                parent);
            FEATHER_TK_P();

            p.colorWidget = feather_tk::ColorWidget::create(context);
            p.colorWidget->setColor(feather_tk::Color4F(0.F, 0.F, 0.F));

            auto layout = feather_tk::VerticalLayout::create(context);
            layout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.colorWidget->setParent(layout);

            auto scrollWidget = feather_tk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            p.colorPickerObserver = feather_tk::ValueObserver<feather_tk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const feather_tk::Color4F& value)
                {
                    _p->colorWidget->setColor(value);
                });
        }

        ColorPickerTool::ColorPickerTool() :
            _p(new Private)
        {}

        ColorPickerTool::~ColorPickerTool()
        {}

        std::shared_ptr<ColorPickerTool> ColorPickerTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorPickerTool>(new ColorPickerTool);
            out->_init(context, app, parent);
            return out;
        }

        void ColorPickerTool::_widgetUpdate()
        {
            FEATHER_TK_P();
        }
    }
}

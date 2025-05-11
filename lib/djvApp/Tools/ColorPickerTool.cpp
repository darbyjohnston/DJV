// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ColorPickerTool.h>

#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <dtk/ui/ColorWidget.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

namespace djv
{
    namespace app
    {
        struct ColorPickerTool::Private
        {
            std::shared_ptr<dtk::ColorWidget> colorWidget;

            std::shared_ptr<dtk::ValueObserver<dtk::Color4F> > colorPickerObserver;
        };

        void ColorPickerTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::ColorPicker,
                "djv::app::ColorPickerTool",
                parent);
            DTK_P();

            p.colorWidget = dtk::ColorWidget::create(context);
            p.colorWidget->setColor(dtk::Color4F(0.F, 0.F, 0.F));

            auto layout = dtk::VerticalLayout::create(context);
            layout->setMarginRole(dtk::SizeRole::MarginSmall);
            layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.colorWidget->setParent(layout);
            dtk::Label::create(context, "Click in the viewport to pick colors", layout);

            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            p.colorPickerObserver = dtk::ValueObserver<dtk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const dtk::Color4F& value)
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
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorPickerTool>(new ColorPickerTool);
            out->_init(context, app, parent);
            return out;
        }

        void ColorPickerTool::_widgetUpdate()
        {
            DTK_P();
        }
    }
}

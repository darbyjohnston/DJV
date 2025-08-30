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
            std::shared_ptr<ftk::ColorWidget> colorWidget;

            std::shared_ptr<ftk::ValueObserver<ftk::Color4F> > colorPickerObserver;
        };

        void ColorPickerTool::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::ColorPicker,
                "djv::app::ColorPickerTool",
                parent);
            FTK_P();

            p.colorWidget = ftk::ColorWidget::create(context);
            p.colorWidget->setColor(ftk::Color4F(0.F, 0.F, 0.F));

            auto layout = ftk::VerticalLayout::create(context);
            layout->setMarginRole(ftk::SizeRole::MarginSmall);
            layout->setSpacingRole(ftk::SizeRole::SpacingSmall);
            p.colorWidget->setParent(layout);

            auto scrollWidget = ftk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            p.colorPickerObserver = ftk::ValueObserver<ftk::Color4F>::create(
                app->getViewportModel()->observeColorPicker(),
                [this](const ftk::Color4F& value)
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
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorPickerTool>(new ColorPickerTool);
            out->_init(context, app, parent);
            return out;
        }

        void ColorPickerTool::_widgetUpdate()
        {
            FTK_P();
        }
    }
}

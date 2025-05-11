// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ViewToolPrivate.h>

#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <dtk/ui/Bellows.h>
#include <dtk/ui/CheckBox.h>
#include <dtk/ui/ColorSwatch.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/GroupBox.h>
#include <dtk/ui/IntEditSlider.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/LineEdit.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        struct ViewOptionsWidget::Private
        {
            std::vector<dtk::ImageType> colorBuffers;

            std::shared_ptr<dtk::ComboBox> minifyComboBox;
            std::shared_ptr<dtk::ComboBox> magnifyComboBox;
            std::shared_ptr<dtk::ComboBox> videoLevelsComboBox;
            std::shared_ptr<dtk::ComboBox> alphaBlendComboBox;
            std::shared_ptr<dtk::ComboBox> colorBufferComboBox;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<dtk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<dtk::ValueObserver<dtk::ImageType> > colorBufferObserver;
        };

        void ViewOptionsWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::ViewOptionsWidget", parent);
            DTK_P();

            p.colorBuffers.push_back(dtk::ImageType::RGBA_U8);
            p.colorBuffers.push_back(dtk::ImageType::RGBA_F16);
            p.colorBuffers.push_back(dtk::ImageType::RGBA_F32);

            p.minifyComboBox = dtk::ComboBox::create(
                context,
                dtk::getImageFilterLabels());
            p.minifyComboBox->setHStretch(dtk::Stretch::Expanding);

            p.magnifyComboBox = dtk::ComboBox::create(
                context,
                dtk::getImageFilterLabels());
            p.magnifyComboBox->setHStretch(dtk::Stretch::Expanding);

            p.videoLevelsComboBox = dtk::ComboBox::create(
                context,
                dtk::getInputVideoLevelsLabels());
            p.videoLevelsComboBox->setHStretch(dtk::Stretch::Expanding);

            p.alphaBlendComboBox = dtk::ComboBox::create(
                context,
                dtk::getAlphaBlendLabels());
            p.videoLevelsComboBox->setHStretch(dtk::Stretch::Expanding);

            std::vector<std::string> items;
            for (size_t i = 0; i < p.colorBuffers.size(); ++i)
            {
                std::stringstream ss;
                ss << p.colorBuffers[i];
                items.push_back(ss.str());
            }
            p.colorBufferComboBox = dtk::ComboBox::create(context, items);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Minify:", p.minifyComboBox);
            p.layout->addRow("Magnify:", p.magnifyComboBox);
            p.layout->addRow("Video levels:", p.videoLevelsComboBox);
            p.layout->addRow("Alpha blend:", p.alphaBlendComboBox);
            p.layout->addRow("Color buffer:", p.colorBufferComboBox);

            p.imageOptionsObserver = dtk::ValueObserver<dtk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const dtk::ImageOptions& value)
                {
                    _p->alphaBlendComboBox->setCurrentIndex(static_cast<int>(value.alphaBlend));
                });

            p.displayOptionsObserver = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    DTK_P();
                    p.minifyComboBox->setCurrentIndex(static_cast<int>(value.imageFilters.minify));
                    p.magnifyComboBox->setCurrentIndex(static_cast<int>(value.imageFilters.magnify));
                    p.videoLevelsComboBox->setCurrentIndex(static_cast<int>(value.videoLevels));
                });

            p.colorBufferObserver = dtk::ValueObserver<dtk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](dtk::ImageType value)
                {
                    DTK_P();
                    int index = -1;
                    const auto i = std::find(p.colorBuffers.begin(), p.colorBuffers.end(), value);
                    if (i != p.colorBuffers.end())
                    {
                        index = i - p.colorBuffers.begin();
                    }
                    _p->colorBufferComboBox->setCurrentIndex(index);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.minifyComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.imageFilters.minify = static_cast<dtk::ImageFilter>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.magnifyComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.imageFilters.magnify = static_cast<dtk::ImageFilter>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.videoLevelsComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.videoLevels = static_cast<dtk::VideoLevels>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.alphaBlendComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getImageOptions();
                        options.alphaBlend = static_cast<dtk::AlphaBlend>(value);
                        app->getViewportModel()->setImageOptions(options);
                    }
                });

            p.colorBufferComboBox->setIndexCallback(
                [this, appWeak](int value)
                {
                    DTK_P();
                    if (auto app = appWeak.lock())
                    {
                        if (value >= 0 && value < p.colorBuffers.size())
                        {
                            app->getViewportModel()->setColorBuffer(p.colorBuffers[value]);
                        }
                    }
                });
        }

        ViewOptionsWidget::ViewOptionsWidget() :
            _p(new Private)
        {}

        ViewOptionsWidget::~ViewOptionsWidget()
        {}

        std::shared_ptr<ViewOptionsWidget> ViewOptionsWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewOptionsWidget>(new ViewOptionsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ViewOptionsWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ViewOptionsWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct BackgroundWidget::Private
        {
            std::shared_ptr<dtk::ComboBox> typeComboBox;
            std::shared_ptr<dtk::ColorSwatch> solidSwatch;
            std::pair< std::shared_ptr<dtk::ColorSwatch>, std::shared_ptr<dtk::ColorSwatch> > checkersSwatch;
            std::shared_ptr<dtk::IntEditSlider> checkersSizeSlider;
            std::pair< std::shared_ptr<dtk::ColorSwatch>, std::shared_ptr<dtk::ColorSwatch> > gradientSwatch;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::BackgroundOptions> > backgroundOptionsObserver;
        };

        void BackgroundWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::BackgroundWidget", parent);
            DTK_P();

            p.typeComboBox = dtk::ComboBox::create(
                context,
                tl::timeline::getBackgroundLabels());
            p.typeComboBox->setHStretch(dtk::Stretch::Expanding);

            p.solidSwatch = dtk::ColorSwatch::create(context);
            p.solidSwatch->setEditable(true);
            p.solidSwatch->setHAlign(dtk::HAlign::Left);

            p.checkersSwatch.first = dtk::ColorSwatch::create(context);
            p.checkersSwatch.first->setEditable(true);
            p.checkersSwatch.second = dtk::ColorSwatch::create(context);
            p.checkersSwatch.second->setEditable(true);
            p.checkersSizeSlider = dtk::IntEditSlider::create(context);
            p.checkersSizeSlider->setRange(dtk::RangeI(10, 100));

            p.gradientSwatch.first = dtk::ColorSwatch::create(context);
            p.gradientSwatch.first->setEditable(true);
            p.gradientSwatch.second = dtk::ColorSwatch::create(context);
            p.gradientSwatch.second->setEditable(true);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Type:", p.typeComboBox);
            p.layout->addRow("Color:", p.solidSwatch);
            p.layout->addRow("Color 1:", p.checkersSwatch.first);
            p.layout->addRow("Color 2:", p.checkersSwatch.second);
            p.layout->addRow("Size:", p.checkersSizeSlider);
            p.layout->addRow("Color 1:", p.gradientSwatch.first);
            p.layout->addRow("Color 2:", p.gradientSwatch.second);

            p.backgroundOptionsObserver = dtk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    _optionsUpdate(value);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.typeComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.type = static_cast<tl::timeline::Background>(value);
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.solidSwatch->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.solidColor = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.checkersSwatch.first->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.checkersColor.first = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.checkersSwatch.second->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.checkersColor.second = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.checkersSizeSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.checkersSize.w = value;
                        options.checkersSize.h = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.gradientSwatch.first->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.gradientColor.first = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.gradientSwatch.second->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.gradientColor.second = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });
        }

        BackgroundWidget::BackgroundWidget() :
            _p(new Private)
        {}

        BackgroundWidget::~BackgroundWidget()
        {}

        std::shared_ptr<BackgroundWidget> BackgroundWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<BackgroundWidget>(new BackgroundWidget);
            out->_init(context, app, parent);
            return out;
        }

        void BackgroundWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void BackgroundWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void BackgroundWidget::_optionsUpdate(const tl::timeline::BackgroundOptions& value)
        {
            DTK_P();
            p.typeComboBox->setCurrentIndex(static_cast<int>(value.type));
            p.solidSwatch->setColor(value.solidColor);
            p.checkersSwatch.first->setColor(value.checkersColor.first);
            p.checkersSwatch.second->setColor(value.checkersColor.second);
            p.checkersSizeSlider->setValue(value.checkersSize.w);
            p.gradientSwatch.first->setColor(value.gradientColor.first);
            p.gradientSwatch.second->setColor(value.gradientColor.second);

            p.layout->setRowVisible(p.solidSwatch, value.type == tl::timeline::Background::Solid);
            p.layout->setRowVisible(p.checkersSwatch.first, value.type == tl::timeline::Background::Checkers);
            p.layout->setRowVisible(p.checkersSwatch.second, value.type == tl::timeline::Background::Checkers);
            p.layout->setRowVisible(p.checkersSizeSlider, value.type == tl::timeline::Background::Checkers);
            p.layout->setRowVisible(p.gradientSwatch.first, value.type == tl::timeline::Background::Gradient);
            p.layout->setRowVisible(p.gradientSwatch.second, value.type == tl::timeline::Background::Gradient);
        }

        struct OutlineWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::shared_ptr<dtk::IntEditSlider> widthSlider;
            std::shared_ptr<dtk::ColorSwatch> colorSwatch;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::BackgroundOptions> > optionsObservers;
        };

        void OutlineWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::OutlineWidget", parent);
            DTK_P();

            p.enabledCheckBox = dtk::CheckBox::create(context);
            p.enabledCheckBox->setHStretch(dtk::Stretch::Expanding);

            p.widthSlider = dtk::IntEditSlider::create(context);
            p.widthSlider->setRange(dtk::RangeI(1, 100));

            p.colorSwatch = dtk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);
            p.colorSwatch->setHAlign(dtk::HAlign::Left);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Width:", p.widthSlider);
            p.layout->addRow("Color:", p.colorSwatch);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const tl::timeline::BackgroundOptions& value)
                {
                    _optionsUpdate(value);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.outline.enabled = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.widthSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.outline.width = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.colorSwatch->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.outline.color = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });
        }

        OutlineWidget::OutlineWidget() :
            _p(new Private)
        {}

        OutlineWidget::~OutlineWidget()
        {}

        std::shared_ptr<OutlineWidget> OutlineWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<OutlineWidget>(new OutlineWidget);
            out->_init(context, app, parent);
            return out;
        }

        void OutlineWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void OutlineWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void OutlineWidget::_optionsUpdate(const tl::timeline::BackgroundOptions& value)
        {
            DTK_P();
            p.enabledCheckBox->setChecked(value.outline.enabled);
            p.widthSlider->setValue(value.outline.width);
            p.colorSwatch->setColor(value.outline.color);
        }

        struct GridWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::shared_ptr<dtk::IntEditSlider> sizeSlider;
            std::shared_ptr<dtk::IntEditSlider> lineWidthSlider;
            std::shared_ptr<dtk::ColorSwatch> colorSwatch;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::ForegroundOptions> > optionsObserver;
        };

        void GridWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::GridWidget", parent);
            DTK_P();

            p.enabledCheckBox = dtk::CheckBox::create(context);
            p.enabledCheckBox->setHStretch(dtk::Stretch::Expanding);

            p.sizeSlider = dtk::IntEditSlider::create(context);
            p.sizeSlider->setRange(dtk::RangeI(1, 1000));

            p.lineWidthSlider = dtk::IntEditSlider::create(context);
            p.lineWidthSlider->setRange(dtk::RangeI(1, 10));

            p.colorSwatch = dtk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);
            p.colorSwatch->setHAlign(dtk::HAlign::Left);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Size:", p.sizeSlider);
            p.layout->addRow("Line width:", p.lineWidthSlider);
            p.layout->addRow("Color:", p.colorSwatch);

            p.optionsObserver = dtk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                app->getViewportModel()->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    DTK_P();
                    p.enabledCheckBox->setChecked(value.grid.enabled);
                    p.sizeSlider->setValue(value.grid.size);
                    p.lineWidthSlider->setValue(value.grid.lineWidth);
                    p.colorSwatch->setColor(value.grid.color);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getForegroundOptions();
                        options.grid.enabled = value;
                        app->getViewportModel()->setForegroundOptions(options);
                    }
                });

            p.sizeSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getForegroundOptions();
                        options.grid.size = value;
                        app->getViewportModel()->setForegroundOptions(options);
                    }
                });

            p.lineWidthSlider->setCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getForegroundOptions();
                        options.grid.lineWidth = value;
                        app->getViewportModel()->setForegroundOptions(options);
                    }
                });

            p.colorSwatch->setColorCallback(
                [appWeak](const dtk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getForegroundOptions();
                        options.grid.color = value;
                        app->getViewportModel()->setForegroundOptions(options);
                    }
                });
        }

        GridWidget::GridWidget() :
            _p(new Private)
        {}

        GridWidget::~GridWidget()
        {}

        std::shared_ptr<GridWidget> GridWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<GridWidget>(new GridWidget);
            out->_init(context, app, parent);
            return out;
        }

        void GridWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void GridWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ViewTool::Private
        {
            std::shared_ptr<ViewOptionsWidget> viewOptionsWidget;
            std::shared_ptr<BackgroundWidget> backgroundWidget;
            std::shared_ptr<OutlineWidget> outlineWidget;
            std::shared_ptr<GridWidget> gridWidget;
            std::map<std::string, std::shared_ptr<dtk::Bellows> > bellows;
        };

        void ViewTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::View,
                "djv::app::ViewTool",
                parent);
            DTK_P();

            p.viewOptionsWidget = ViewOptionsWidget::create(context, app);
            p.backgroundWidget = BackgroundWidget::create(context, app);
            p.outlineWidget = OutlineWidget::create(context, app);
            p.gridWidget = GridWidget::create(context, app);

            auto layout = dtk::VerticalLayout::create(context);
            layout->setSpacingRole(dtk::SizeRole::None);
            p.bellows["Options"] = dtk::Bellows::create(context, "Options", layout);
            p.bellows["Options"]->setWidget(p.viewOptionsWidget);
            p.bellows["Background"] = dtk::Bellows::create(context, "Background", layout);
            p.bellows["Background"]->setWidget(p.backgroundWidget);
            p.bellows["Outline"] = dtk::Bellows::create(context, "Outline", layout);
            p.bellows["Outline"]->setWidget(p.outlineWidget);
            p.bellows["Grid"] = dtk::Bellows::create(context, "Grid", layout);
            p.bellows["Grid"]->setWidget(p.gridWidget);
            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            _loadSettings(p.bellows);
        }

        ViewTool::ViewTool() :
            _p(new Private)
        {}

        ViewTool::~ViewTool()
        {
            _saveSettings(_p->bellows);
        }

        std::shared_ptr<ViewTool> ViewTool::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewTool>(new ViewTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ViewToolPrivate.h>

#include <djvApp/Models/ViewportModel.h>
#include <djvApp/Widgets/Viewport.h>
#include <djvApp/App.h>
#include <djvApp/MainWindow.h>

#include <feather-tk/ui/Bellows.h>
#include <feather-tk/ui/CheckBox.h>
#include <feather-tk/ui/ColorSwatch.h>
#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/DoubleEdit.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/GroupBox.h>
#include <feather-tk/ui/IntEdit.h>
#include <feather-tk/ui/IntEditSlider.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/LineEdit.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        struct ViewPosZoomWidget::Private
        {
            std::shared_ptr<feather_tk::IntEdit> posXEdit;
            std::shared_ptr<feather_tk::IntResetButton> posXReset;
            std::shared_ptr<feather_tk::IntEdit> posYEdit;
            std::shared_ptr<feather_tk::IntResetButton> posYReset;
            std::shared_ptr<feather_tk::DoubleEdit> zoomEdit;
            std::shared_ptr<feather_tk::DoubleResetButton> zoomReset;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> > > posZoomObserver;
            bool updating = false;
        };

        void ViewPosZoomWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            feather_tk::IWidget::_init(context, "djv::app::ViewPosZoomWidget", parent);
            FEATHER_TK_P();

            p.posXEdit = feather_tk::IntEdit::create(context);
            p.posXEdit->setRange(-100000000, 100000000);
            p.posXEdit->setDefaultValue(0.0);
            p.posXReset = feather_tk::IntResetButton::create(context, p.posXEdit->getModel());

            p.posYEdit = feather_tk::IntEdit::create(context);
            p.posYEdit->setRange(-100000000, 100000000);
            p.posYEdit->setDefaultValue(0.0);
            p.posYReset = feather_tk::IntResetButton::create(context, p.posYEdit->getModel());

            p.zoomEdit = feather_tk::DoubleEdit::create(context);
            p.zoomEdit->setRange(0.000001, 100000.0);
            p.zoomEdit->setStep(1.0);
            p.zoomEdit->setLargeStep(2.0);
            p.zoomEdit->setDefaultValue(1.0);
            p.zoomReset = feather_tk::DoubleResetButton::create(context, p.zoomEdit->getModel());

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            auto hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            p.posXEdit->setParent(hLayout);
            p.posXReset->setParent(hLayout);
            p.layout->addRow("X:", hLayout);
            hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            p.posYEdit->setParent(hLayout);
            p.posYReset->setParent(hLayout);
            p.layout->addRow("Y:", hLayout);
            hLayout = feather_tk::HorizontalLayout::create(context, p.layout);
            hLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);
            p.zoomEdit->setParent(hLayout);
            p.zoomReset->setParent(hLayout);
            p.layout->addRow("Zoom:", hLayout);

            std::weak_ptr<MainWindow> mainWindowWeak(mainWindow);
            p.posXEdit->setCallback(
                [this, mainWindowWeak](int value)
                {
                    if (!_p->updating)
                    {
                        if (auto mainWindow = mainWindowWeak.lock())
                        {
                            auto viewport = mainWindow->getViewport();
                            const feather_tk::V2I& pos = viewport->getViewPos();
                            const double zoom = viewport->getViewZoom();
                            viewport->setViewPosAndZoom(feather_tk::V2I(value, pos.y), zoom);
                        }
                    }
                });

            p.posYEdit->setCallback(
                [this, mainWindowWeak](int value)
                {
                    if (!_p->updating)
                    {
                        if (auto mainWindow = mainWindowWeak.lock())
                        {
                            auto viewport = mainWindow->getViewport();
                            const feather_tk::V2I& pos = viewport->getViewPos();
                            const double zoom = viewport->getViewZoom();
                            viewport->setViewPosAndZoom(feather_tk::V2I(pos.x, value), zoom);
                        }
                    }
                });

            p.zoomEdit->setCallback(
                [this, mainWindowWeak](double value)
                {
                    if (!_p->updating)
                    {
                        if (auto mainWindow = mainWindowWeak.lock())
                        {
                            auto viewport = mainWindow->getViewport();
                            const feather_tk::Box2I& g = viewport->getGeometry();
                            const feather_tk::V2I focus(g.w() / 2, g.h() / 2);
                            mainWindow->getViewport()->setViewZoom(value, focus);
                        }
                    }
                });

            p.posZoomObserver = feather_tk::ValueObserver<std::pair<feather_tk::V2I, double> >::create(
                mainWindow->getViewport()->observeViewPosAndZoom(),
                [this](const std::pair<feather_tk::V2I, double>& value)
                {
                    FEATHER_TK_P();
                    p.updating = true;
                    p.posXEdit->setValue(value.first.x);
                    p.posYEdit->setValue(value.first.y);
                    p.zoomEdit->setValue(value.second);
                    p.updating = false;
                });
        }

        ViewPosZoomWidget::ViewPosZoomWidget() :
            _p(new Private)
        {}

        ViewPosZoomWidget::~ViewPosZoomWidget()
        {}

        std::shared_ptr<ViewPosZoomWidget> ViewPosZoomWidget::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewPosZoomWidget>(new ViewPosZoomWidget);
            out->_init(context, app, mainWindow, parent);
            return out;
        }

        void ViewPosZoomWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ViewPosZoomWidget::sizeHintEvent(const feather_tk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ViewOptionsWidget::Private
        {
            std::vector<feather_tk::ImageType> colorBuffers;

            std::shared_ptr<feather_tk::ComboBox> minifyComboBox;
            std::shared_ptr<feather_tk::ComboBox> magnifyComboBox;
            std::shared_ptr<feather_tk::ComboBox> videoLevelsComboBox;
            std::shared_ptr<feather_tk::ComboBox> alphaBlendComboBox;
            std::shared_ptr<feather_tk::ComboBox> colorBufferComboBox;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<feather_tk::ValueObserver<feather_tk::ImageType> > colorBufferObserver;
        };

        void ViewOptionsWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            feather_tk::IWidget::_init(context, "djv::app::ViewOptionsWidget", parent);
            FEATHER_TK_P();

            p.minifyComboBox = feather_tk::ComboBox::create(
                context,
                feather_tk::getImageFilterLabels());
            p.minifyComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.magnifyComboBox = feather_tk::ComboBox::create(
                context,
                feather_tk::getImageFilterLabels());
            p.magnifyComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.videoLevelsComboBox = feather_tk::ComboBox::create(
                context,
                feather_tk::getInputVideoLevelsLabels());
            p.videoLevelsComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.alphaBlendComboBox = feather_tk::ComboBox::create(
                context,
                feather_tk::getAlphaBlendLabels());
            p.videoLevelsComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.colorBuffers.push_back(feather_tk::ImageType::RGBA_U8);
#if defined(FEATHER_TK_API_GL_4_1)
            p.colorBuffers.push_back(feather_tk::ImageType::RGBA_F16);
            p.colorBuffers.push_back(feather_tk::ImageType::RGBA_F32);
#endif // FEATHER_TK_API_GL_4_1
            std::vector<std::string> items;
            for (size_t i = 0; i < p.colorBuffers.size(); ++i)
            {
                std::stringstream ss;
                ss << p.colorBuffers[i];
                items.push_back(ss.str());
            }
            p.colorBufferComboBox = feather_tk::ComboBox::create(context, items);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Minify:", p.minifyComboBox);
            p.layout->addRow("Magnify:", p.magnifyComboBox);
            p.layout->addRow("Video levels:", p.videoLevelsComboBox);
            p.layout->addRow("Alpha blend:", p.alphaBlendComboBox);
            p.layout->addRow("Color buffer:", p.colorBufferComboBox);

            p.imageOptionsObserver = feather_tk::ValueObserver<feather_tk::ImageOptions>::create(
                app->getViewportModel()->observeImageOptions(),
                [this](const feather_tk::ImageOptions& value)
                {
                    _p->alphaBlendComboBox->setCurrentIndex(static_cast<int>(value.alphaBlend));
                });

            p.displayOptionsObserver = feather_tk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    FEATHER_TK_P();
                    p.minifyComboBox->setCurrentIndex(static_cast<int>(value.imageFilters.minify));
                    p.magnifyComboBox->setCurrentIndex(static_cast<int>(value.imageFilters.magnify));
                    p.videoLevelsComboBox->setCurrentIndex(static_cast<int>(value.videoLevels));
                });

            p.colorBufferObserver = feather_tk::ValueObserver<feather_tk::ImageType>::create(
                app->getViewportModel()->observeColorBuffer(),
                [this](feather_tk::ImageType value)
                {
                    FEATHER_TK_P();
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
                        options.imageFilters.minify = static_cast<feather_tk::ImageFilter>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.magnifyComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.imageFilters.magnify = static_cast<feather_tk::ImageFilter>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.videoLevelsComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.videoLevels = static_cast<feather_tk::VideoLevels>(value);
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.alphaBlendComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getImageOptions();
                        options.alphaBlend = static_cast<feather_tk::AlphaBlend>(value);
                        app->getViewportModel()->setImageOptions(options);
                    }
                });

            p.colorBufferComboBox->setIndexCallback(
                [this, appWeak](int value)
                {
                    FEATHER_TK_P();
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewOptionsWidget>(new ViewOptionsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ViewOptionsWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ViewOptionsWidget::sizeHintEvent(const feather_tk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct BackgroundWidget::Private
        {
            std::shared_ptr<feather_tk::ComboBox> typeComboBox;
            std::shared_ptr<feather_tk::ColorSwatch> solidSwatch;
            std::pair< std::shared_ptr<feather_tk::ColorSwatch>, std::shared_ptr<feather_tk::ColorSwatch> > checkersSwatch;
            std::shared_ptr<feather_tk::IntEditSlider> checkersSizeSlider;
            std::pair< std::shared_ptr<feather_tk::ColorSwatch>, std::shared_ptr<feather_tk::ColorSwatch> > gradientSwatch;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::BackgroundOptions> > backgroundOptionsObserver;
        };

        void BackgroundWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            feather_tk::IWidget::_init(context, "djv::app::BackgroundWidget", parent);
            FEATHER_TK_P();

            p.typeComboBox = feather_tk::ComboBox::create(
                context,
                tl::timeline::getBackgroundLabels());
            p.typeComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.solidSwatch = feather_tk::ColorSwatch::create(context);
            p.solidSwatch->setEditable(true);
            p.solidSwatch->setHAlign(feather_tk::HAlign::Left);

            p.checkersSwatch.first = feather_tk::ColorSwatch::create(context);
            p.checkersSwatch.first->setEditable(true);
            p.checkersSwatch.second = feather_tk::ColorSwatch::create(context);
            p.checkersSwatch.second->setEditable(true);
            p.checkersSizeSlider = feather_tk::IntEditSlider::create(context);
            p.checkersSizeSlider->setRange(10, 100);

            p.gradientSwatch.first = feather_tk::ColorSwatch::create(context);
            p.gradientSwatch.first->setEditable(true);
            p.gradientSwatch.second = feather_tk::ColorSwatch::create(context);
            p.gradientSwatch.second->setEditable(true);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Type:", p.typeComboBox);
            p.layout->addRow("Color:", p.solidSwatch);
            p.layout->addRow("Color 1:", p.checkersSwatch.first);
            p.layout->addRow("Color 2:", p.checkersSwatch.second);
            p.layout->addRow("Size:", p.checkersSizeSlider);
            p.layout->addRow("Color 1:", p.gradientSwatch.first);
            p.layout->addRow("Color 2:", p.gradientSwatch.second);

            p.backgroundOptionsObserver = feather_tk::ValueObserver<tl::timeline::BackgroundOptions>::create(
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
                [appWeak](const feather_tk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.solidColor = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.checkersSwatch.first->setColorCallback(
                [appWeak](const feather_tk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.checkersColor.first = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.checkersSwatch.second->setColorCallback(
                [appWeak](const feather_tk::Color4F& value)
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
                [appWeak](const feather_tk::Color4F& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getBackgroundOptions();
                        options.gradientColor.first = value;
                        app->getViewportModel()->setBackgroundOptions(options);
                    }
                });

            p.gradientSwatch.second->setColorCallback(
                [appWeak](const feather_tk::Color4F& value)
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<BackgroundWidget>(new BackgroundWidget);
            out->_init(context, app, parent);
            return out;
        }

        void BackgroundWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void BackgroundWidget::sizeHintEvent(const feather_tk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void BackgroundWidget::_optionsUpdate(const tl::timeline::BackgroundOptions& value)
        {
            FEATHER_TK_P();
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
            std::shared_ptr<feather_tk::CheckBox> enabledCheckBox;
            std::shared_ptr<feather_tk::IntEditSlider> widthSlider;
            std::shared_ptr<feather_tk::ColorSwatch> colorSwatch;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::BackgroundOptions> > optionsObservers;
        };

        void OutlineWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            feather_tk::IWidget::_init(context, "djv::app::OutlineWidget", parent);
            FEATHER_TK_P();

            p.enabledCheckBox = feather_tk::CheckBox::create(context);
            p.enabledCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.widthSlider = feather_tk::IntEditSlider::create(context);
            p.widthSlider->setRange(1, 100);

            p.colorSwatch = feather_tk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);
            p.colorSwatch->setHAlign(feather_tk::HAlign::Left);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Width:", p.widthSlider);
            p.layout->addRow("Color:", p.colorSwatch);

            p.optionsObservers = feather_tk::ValueObserver<tl::timeline::BackgroundOptions>::create(
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
                [appWeak](const feather_tk::Color4F& value)
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<OutlineWidget>(new OutlineWidget);
            out->_init(context, app, parent);
            return out;
        }

        void OutlineWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void OutlineWidget::sizeHintEvent(const feather_tk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        void OutlineWidget::_optionsUpdate(const tl::timeline::BackgroundOptions& value)
        {
            FEATHER_TK_P();
            p.enabledCheckBox->setChecked(value.outline.enabled);
            p.widthSlider->setValue(value.outline.width);
            p.colorSwatch->setColor(value.outline.color);
        }

        struct GridWidget::Private
        {
            std::shared_ptr<feather_tk::CheckBox> enabledCheckBox;
            std::shared_ptr<feather_tk::IntEditSlider> sizeSlider;
            std::shared_ptr<feather_tk::IntEditSlider> lineWidthSlider;
            std::shared_ptr<feather_tk::ColorSwatch> colorSwatch;
            std::shared_ptr<feather_tk::FormLayout> layout;

            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::ForegroundOptions> > optionsObserver;
        };

        void GridWidget::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<feather_tk::IWidget>& parent)
        {
            feather_tk::IWidget::_init(context, "djv::app::GridWidget", parent);
            FEATHER_TK_P();

            p.enabledCheckBox = feather_tk::CheckBox::create(context);
            p.enabledCheckBox->setHStretch(feather_tk::Stretch::Expanding);

            p.sizeSlider = feather_tk::IntEditSlider::create(context);
            p.sizeSlider->setRange(1, 1000);

            p.lineWidthSlider = feather_tk::IntEditSlider::create(context);
            p.lineWidthSlider->setRange(1, 10);

            p.colorSwatch = feather_tk::ColorSwatch::create(context);
            p.colorSwatch->setEditable(true);
            p.colorSwatch->setHAlign(feather_tk::HAlign::Left);

            p.layout = feather_tk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(feather_tk::SizeRole::Margin);
            p.layout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Size:", p.sizeSlider);
            p.layout->addRow("Line width:", p.lineWidthSlider);
            p.layout->addRow("Color:", p.colorSwatch);

            p.optionsObserver = feather_tk::ValueObserver<tl::timeline::ForegroundOptions>::create(
                app->getViewportModel()->observeForegroundOptions(),
                [this](const tl::timeline::ForegroundOptions& value)
                {
                    FEATHER_TK_P();
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
                [appWeak](const feather_tk::Color4F& value)
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<GridWidget>(new GridWidget);
            out->_init(context, app, parent);
            return out;
        }

        void GridWidget::setGeometry(const feather_tk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void GridWidget::sizeHintEvent(const feather_tk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ViewTool::Private
        {
            std::shared_ptr<ViewPosZoomWidget> viewPosZoomWidget;
            std::shared_ptr<ViewOptionsWidget> viewOptionsWidget;
            std::shared_ptr<BackgroundWidget> backgroundWidget;
            std::shared_ptr<OutlineWidget> outlineWidget;
            std::shared_ptr<GridWidget> gridWidget;
            std::map<std::string, std::shared_ptr<feather_tk::Bellows> > bellows;
        };

        void ViewTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::View,
                "djv::app::ViewTool",
                parent);
            FEATHER_TK_P();

            p.viewPosZoomWidget = ViewPosZoomWidget::create(context, app, mainWindow);
            p.viewOptionsWidget = ViewOptionsWidget::create(context, app);
            p.backgroundWidget = BackgroundWidget::create(context, app);
            p.outlineWidget = OutlineWidget::create(context, app);
            p.gridWidget = GridWidget::create(context, app);

            auto layout = feather_tk::VerticalLayout::create(context);
            layout->setSpacingRole(feather_tk::SizeRole::None);
            p.bellows["PosZoom"] = feather_tk::Bellows::create(context, "Position and Zoom", layout);
            p.bellows["PosZoom"]->setWidget(p.viewPosZoomWidget);
            p.bellows["Options"] = feather_tk::Bellows::create(context, "Options", layout);
            p.bellows["Options"]->setWidget(p.viewOptionsWidget);
            p.bellows["Background"] = feather_tk::Bellows::create(context, "Background", layout);
            p.bellows["Background"]->setWidget(p.backgroundWidget);
            p.bellows["Outline"] = feather_tk::Bellows::create(context, "Outline", layout);
            p.bellows["Outline"]->setWidget(p.outlineWidget);
            p.bellows["Grid"] = feather_tk::Bellows::create(context, "Grid", layout);
            p.bellows["Grid"]->setWidget(p.gridWidget);
            auto scrollWidget = feather_tk::ScrollWidget::create(context);
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ViewTool>(new ViewTool);
            out->_init(context, app, mainWindow, parent);
            return out;
        }
    }
}

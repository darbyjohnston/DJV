// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ViewControlsWidget.h>

#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/View.h>
#include <djvViewApp/ViewSettings.h>
#include <djvViewApp/ViewWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUIComponents/ColorPicker.h>

#include <djvUI/Bellows.h>
#include <djvUI/CheckBox.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Context.h>
#include <djvCore/NumericValueModels.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ViewControlsWidget::Private
        {
            glm::vec2 viewPos = glm::vec2(0.F, 0.F);
            float viewZoom = 1.F;
            GridOptions gridOptions;
            HUDOptions hudOptions;
            ViewBackgroundOptions backgroundOptions;

            std::shared_ptr<MediaWidget> activeWidget;

            std::shared_ptr<UI::FloatEdit> viewPosEdit[2];
            std::shared_ptr<UI::ToolButton> viewPosResetButton[2];
            std::shared_ptr<UI::FloatEdit> viewZoomEdit;
            std::shared_ptr<UI::ToolButton> viewZoomResetButton;
            std::shared_ptr<UI::HorizontalLayout> viewPosLayout[2];
            std::shared_ptr<UI::HorizontalLayout> viewZoomLayout;

            std::shared_ptr<UI::ToolButton> gridEnabledButton;
            std::shared_ptr<UI::IntSlider> gridSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> gridColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> gridLabelsComboBox;
            std::shared_ptr<UI::ColorPickerSwatch> gridLabelsColorPickerSwatch;

            std::shared_ptr<UI::ToolButton> hudEnabledButton;
            std::shared_ptr<UI::ColorPickerSwatch> hudColorPickerSwatch;
            std::shared_ptr<UI::ComboBox> hudBackgroundComboBox;

            std::shared_ptr<UI::ComboBox> backgroundComboBox;
            std::shared_ptr<UI::ColorPickerSwatch> backgroundColorPickerSwatch;
            std::shared_ptr<UI::FloatSlider> checkersSizeSlider;
            std::shared_ptr<UI::ColorPickerSwatch> checkersColorPickerSwatches[2];
            std::shared_ptr<UI::HorizontalLayout> checkersColorsLayout;
            std::shared_ptr<UI::CheckBox> borderCheckBox;
            std::shared_ptr<UI::FloatSlider> borderWidthSlider;
            std::shared_ptr<UI::ColorPickerSwatch> borderColorPickerSwatch;

            std::shared_ptr<UI::LabelSizeGroup> sizeGroup;
            std::map<std::string, std::shared_ptr<UI::FormLayout> > formLayouts;
            std::map<std::string, std::shared_ptr<UI::Bellows> > bellows;

            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<glm::vec2> > viewPosObserver;
            std::shared_ptr<ValueObserver<float> > viewZoomObserver;
            std::shared_ptr<ValueObserver<GridOptions> > gridOptionsObserver;
            std::shared_ptr<ValueObserver<HUDOptions> > hudOptionsObserver;
            std::shared_ptr<ValueObserver<ViewBackgroundOptions> > backgroundOptionsObserver;
        };

        void ViewControlsWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);

            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::ViewControlsWidget");

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
            p.gridOptions = viewSettings->observeGridOptions()->get();
            p.hudOptions = viewSettings->observeHUDOptions()->get();
            p.backgroundOptions = viewSettings->observeBackgroundOptions()->get();

            for (size_t i = 0; i < 2; ++i)
            {
                p.viewPosEdit[i] = UI::FloatEdit::create(context);
                auto model = FloatValueModel::create();
                model->setRange(FloatRange(-1000000.F, 1000000.F));
                model->setSmallIncrement(1.F);
                model->setLargeIncrement(10.F);
                p.viewPosEdit[i]->setModel(model);

                p.viewPosResetButton[i] = UI::ToolButton::create(context);
                p.viewPosResetButton[i]->setIcon("djvIconClear");
            }
            p.viewZoomEdit = UI::FloatEdit::create(context);
            auto model = FloatValueModel::create();
            model->setRange(FloatRange(.1F, 1000.F));
            model->setSmallIncrement(.1F);
            model->setLargeIncrement(1.F);
            p.viewZoomEdit->setModel(model);
            p.viewZoomResetButton = UI::ToolButton::create(context);
            p.viewZoomResetButton->setIcon("djvIconClear");

            p.gridEnabledButton = UI::ToolButton::create(context);
            p.gridEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.gridEnabledButton->setIcon("djvIconHidden");
            p.gridEnabledButton->setCheckedIcon("djvIconVisible");
            p.gridEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.gridSizeSlider = UI::IntSlider::create(context);
            p.gridSizeSlider->setRange(IntRange(1, 500));
            p.gridColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.gridLabelsComboBox = UI::ComboBox::create(context);
            p.gridLabelsColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.gridLabelsColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.hudEnabledButton = UI::ToolButton::create(context);
            p.hudEnabledButton->setButtonType(UI::ButtonType::Toggle);
            p.hudEnabledButton->setIcon("djvIconHidden");
            p.hudEnabledButton->setCheckedIcon("djvIconVisible");
            p.hudEnabledButton->setInsideMargin(UI::MetricsRole::None);
            p.hudColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.hudColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.hudBackgroundComboBox = UI::ComboBox::create(context);

            p.backgroundComboBox = UI::ComboBox::create(context);
            p.backgroundColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.backgroundColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.checkersSizeSlider = UI::FloatSlider::create(context);
            p.checkersSizeSlider->setRange(FloatRange(10.F, 100.F));
            p.checkersColorPickerSwatches[0] = UI::ColorPickerSwatch::create(context);
            p.checkersColorPickerSwatches[0]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.checkersColorPickerSwatches[0]->setHAlign(UI::HAlign::Fill);
            p.checkersColorPickerSwatches[1] = UI::ColorPickerSwatch::create(context);
            p.checkersColorPickerSwatches[1]->setHAlign(UI::HAlign::Fill);
            p.checkersColorPickerSwatches[1]->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);
            p.borderCheckBox = UI::CheckBox::create(context);
            p.borderWidthSlider = UI::FloatSlider::create(context);
            p.borderColorPickerSwatch = UI::ColorPickerSwatch::create(context);
            p.borderColorPickerSwatch->setSwatchSizeRole(UI::MetricsRole::SwatchSmall);

            p.sizeGroup = UI::LabelSizeGroup::create();

            p.formLayouts["View"] = UI::FormLayout::create(context);
            for (size_t i = 0; i < 2; ++i)
            {
                p.viewPosLayout[i] = UI::HorizontalLayout::create(context);
                p.viewPosLayout[i]->setSpacing(UI::MetricsRole::None);
                p.viewPosLayout[i]->addChild(p.viewPosEdit[i]);
                p.viewPosLayout[i]->addChild(p.viewPosResetButton[i]);
                p.formLayouts["View"]->addChild(p.viewPosLayout[i]);
            }
            p.viewZoomLayout = UI::HorizontalLayout::create(context);
            p.viewZoomLayout->setSpacing(UI::MetricsRole::None);
            p.viewZoomLayout->addChild(p.viewZoomEdit);
            p.viewZoomLayout->addChild(p.viewZoomResetButton);
            p.formLayouts["View"]->addChild(p.viewZoomLayout);
            p.bellows["View"] = UI::Bellows::create(context);
            p.bellows["View"]->addChild(p.formLayouts["View"]);

            p.formLayouts["Grid"] = UI::FormLayout::create(context);
            p.formLayouts["Grid"]->addChild(p.gridSizeSlider);
            p.formLayouts["Grid"]->addChild(p.gridColorPickerSwatch);
            p.formLayouts["Grid"]->addChild(p.gridLabelsComboBox);
            p.formLayouts["Grid"]->addChild(p.gridLabelsColorPickerSwatch);
            p.bellows["Grid"] = UI::Bellows::create(context);
            p.bellows["Grid"]->addWidget(p.gridEnabledButton);
            p.bellows["Grid"]->addChild(p.formLayouts["Grid"]);

            p.formLayouts["HUD"] = UI::FormLayout::create(context);
            p.formLayouts["HUD"]->addChild(p.hudColorPickerSwatch);
            p.formLayouts["HUD"]->addChild(p.hudBackgroundComboBox);
            p.bellows["HUD"] = UI::Bellows::create(context);
            p.bellows["HUD"]->addWidget(p.hudEnabledButton);
            p.bellows["HUD"]->addChild(p.formLayouts["HUD"]);

            p.formLayouts["Background"] = UI::FormLayout::create(context);
            p.formLayouts["Background"]->addChild(p.backgroundComboBox);
            p.formLayouts["Background"]->addChild(p.backgroundColorPickerSwatch);
            p.formLayouts["Background"]->addChild(p.checkersSizeSlider);
            p.checkersColorsLayout = UI::HorizontalLayout::create(context);
            p.checkersColorsLayout->addChild(p.checkersColorPickerSwatches[0]);
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[0], UI::RowStretch::Expand);
            p.checkersColorsLayout->addChild(p.checkersColorPickerSwatches[1]);
            p.checkersColorsLayout->setStretch(p.checkersColorPickerSwatches[1], UI::RowStretch::Expand);
            p.formLayouts["Background"]->addChild(p.checkersColorsLayout);
            p.formLayouts["Background"]->addChild(p.borderCheckBox);
            p.formLayouts["Background"]->addChild(p.borderWidthSlider);
            p.formLayouts["Background"]->addChild(p.borderColorPickerSwatch);
            p.bellows["Background"] = UI::Bellows::create(context);
            p.bellows["Background"]->addChild(p.formLayouts["Background"]);
            
            for (const auto& i : p.formLayouts)
            {
                i.second->setMargin(UI::MetricsRole::MarginSmall);
                i.second->setSpacing(UI::MetricsRole::SpacingSmall);
                i.second->setLabelSizeGroup(p.sizeGroup);
            }
            for (const auto& i : p.bellows)
            {
                i.second->close();
            }

            auto vLayout = UI::VerticalLayout::create(context);
            vLayout->setSpacing(UI::MetricsRole::None);
            vLayout->addChild(p.bellows["View"]);
            vLayout->addChild(p.bellows["Grid"]);
            vLayout->addChild(p.bellows["HUD"]);
            vLayout->addChild(p.bellows["Background"]);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBackgroundRole(UI::ColorRole::Background);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(vLayout);
            addChild(scrollWidget);

            _widgetUpdate();

            auto weak = std::weak_ptr<ViewControlsWidget>(std::dynamic_pointer_cast<ViewControlsWidget>(shared_from_this()));
            p.viewPosEdit[0]->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPos(glm::vec2(value, widget->_p->viewPos.y));
                    }
                });
            p.viewPosEdit[1]->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setPos(glm::vec2(widget->_p->viewPos.x, value));
                    }
                });

            p.viewPosResetButton[0]->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    glm::vec2 pos = widget->_p->viewPos;
                    pos.x = 0.F;
                    widget->_setPos(pos);
                }
            });
            p.viewPosResetButton[1]->setClickedCallback(
                [weak]
            {
                if (auto widget = weak.lock())
                {
                    glm::vec2 pos = widget->_p->viewPos;
                    pos.y = 0.F;
                    widget->_setPos(pos);
                }
            });

            p.viewZoomEdit->setValueCallback(
                [weak](float value, UI::TextEditReason)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setZoom(value);
                    }
                });

            p.viewZoomResetButton->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_setZoom(1.F);
                    }
                });

            auto contextWeak = std::weak_ptr<Context>(context);
            p.gridEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.enabled = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridSizeSlider->setValueCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.size = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.color = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridLabelsComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.labels = static_cast<GridLabels>(value);
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });
            p.gridLabelsColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->gridOptions.labelsColor = value;
                            widget->_widgetUpdate();
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->activeWidget->getViewWidget()->setGridOptions(widget->_p->gridOptions);
                            }
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                            viewSettings->setGridOptions(widget->_p->gridOptions);
                        }
                    }
                });

            p.hudEnabledButton->setCheckedCallback(
                [weak, contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions.enabled = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
                    }
                }
            });
            p.hudColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions.color = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
                    }
                }
            });
            p.hudBackgroundComboBox->setCallback(
                [weak, contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->hudOptions.background = static_cast<HUDBackground>(value);
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->setHUDOptions(widget->_p->hudOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setHUDOptions(widget->_p->hudOptions);
                    }
                }
            });

            p.backgroundComboBox->setCallback(
                [weak, contextWeak](int value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.background = static_cast<ViewBackground>(value);
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.backgroundColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.color = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.checkersSizeSlider->setValueCallback(
                [weak, contextWeak](float value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.checkersSize = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.checkersColorPickerSwatches[0]->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.checkersColors[0] = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.checkersColorPickerSwatches[1]->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.checkersColors[1] = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.borderCheckBox->setCheckedCallback(
                [weak, contextWeak](bool value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.border = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.borderWidthSlider->setValueCallback(
                [weak, contextWeak](float value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.borderWidth = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });
            p.borderColorPickerSwatch->setColorCallback(
                [weak, contextWeak](const AV::Image::Color& value)
            {
                if (auto context = contextWeak.lock())
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->backgroundOptions.borderColor = value;
                        widget->_widgetUpdate();
                        if (widget->_p->activeWidget)
                        {
                            widget->_p->activeWidget->getViewWidget()->setBackgroundOptions(widget->_p->backgroundOptions);
                        }
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                        viewSettings->setBackgroundOptions(widget->_p->backgroundOptions);
                    }
                }
            });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                widget->_p->viewPosObserver = ValueObserver<glm::vec2>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImagePos(),
                                    [weak](const glm::vec2& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewPos = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->viewZoomObserver = ValueObserver<float>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeImageZoom(),
                                    [weak](float value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->viewZoom = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->gridOptionsObserver = ValueObserver<GridOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeGridOptions(),
                                    [weak](const GridOptions& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->gridOptions = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_p->hudOptionsObserver = ValueObserver<HUDOptions>::create(
                                    widget->_p->activeWidget->observeHUDOptions(),
                                    [weak](const HUDOptions& value)
                                {
                                    if (auto widget = weak.lock())
                                    {
                                        widget->_p->hudOptions = value;
                                        widget->_widgetUpdate();
                                    }
                                });
                                widget->_p->backgroundOptionsObserver = ValueObserver<ViewBackgroundOptions>::create(
                                    widget->_p->activeWidget->getViewWidget()->observeBackgroundOptions(),
                                    [weak](const ViewBackgroundOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->backgroundOptions = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->viewPosObserver.reset();
                                widget->_p->viewZoomObserver.reset();
                                widget->_p->gridOptionsObserver.reset();
                                widget->_p->hudOptionsObserver.reset();
                                widget->_p->backgroundOptionsObserver.reset();
                            }
                        }
                    });
            }
        }

        ViewControlsWidget::ViewControlsWidget() :
            _p(new Private)
        {}

        ViewControlsWidget::~ViewControlsWidget()
        {}

        std::shared_ptr<ViewControlsWidget> ViewControlsWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ViewControlsWidget>(new ViewControlsWidget);
            out->_init(context);
            return out;
        }

        std::map<std::string, bool> ViewControlsWidget::getBellowsState() const
        {
            DJV_PRIVATE_PTR();
            std::map<std::string, bool> out;
            for (const auto& i : p.bellows)
            {
                out[i.first] = i.second->isOpen();
            }
            return out;
        }

        void ViewControlsWidget::setBellowsState(const std::map<std::string, bool>& value)
        {
            DJV_PRIVATE_PTR();
            for (const auto& i : value)
            {
                const auto j = p.bellows.find(i.first);
                if (j != p.bellows.end())
                {
                    j->second->setOpen(i.second);
                }
            }
        }

        void ViewControlsWidget::_initLayoutEvent(Event::InitLayout&)
        {
            _p->sizeGroup->calcMinimumSize();
        }

        void ViewControlsWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("view_controls")));

                for (size_t i = 0; i < 2; ++i)
                {
                    p.viewPosResetButton[i]->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                }
                p.viewZoomResetButton->setTooltip(_getText(DJV_TEXT("reset_the_value")));
                p.formLayouts["View"]->setText(p.viewPosLayout[0], _getText(DJV_TEXT("position_x")) + ":");
                p.formLayouts["View"]->setText(p.viewPosLayout[1], _getText(DJV_TEXT("position_y")) + ":");
                p.formLayouts["View"]->setText(p.viewZoomLayout, _getText(DJV_TEXT("zoom")) + ":");

                p.gridEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_grid_enabled")));
                p.formLayouts["Grid"]->setText(p.gridSizeSlider, _getText(DJV_TEXT("widget_view_grid_size")) + ":");
                p.formLayouts["Grid"]->setText(p.gridColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_color")) + ":");
                p.formLayouts["Grid"]->setText(p.gridLabelsComboBox, _getText(DJV_TEXT("widget_view_grid_labels")) + ":");
                p.formLayouts["Grid"]->setText(p.gridLabelsColorPickerSwatch, _getText(DJV_TEXT("widget_view_grid_labels_color")) + ":");

                p.hudEnabledButton->setTooltip(_getText(DJV_TEXT("widget_view_hud_enabled_tooltip")));
                p.formLayouts["HUD"]->setText(p.hudColorPickerSwatch, _getText(DJV_TEXT("widget_view_hud_color")) + ":");
                p.formLayouts["HUD"]->setText(p.hudBackgroundComboBox, _getText(DJV_TEXT("widget_view_hud_background")) + ":");

                p.formLayouts["Background"]->setText(p.backgroundComboBox, _getText(DJV_TEXT("widget_view_background")) + ":");
                p.formLayouts["Background"]->setText(p.backgroundColorPickerSwatch, _getText(DJV_TEXT("widget_view_background_color")) + ":");
                p.formLayouts["Background"]->setText(p.checkersSizeSlider, _getText(DJV_TEXT("widget_view_background_checkers_size")) + ":");
                p.formLayouts["Background"]->setText(p.checkersColorsLayout, _getText(DJV_TEXT("widget_view_background_checkers_colors")) + ":");
                p.formLayouts["Background"]->setText(p.borderCheckBox, _getText(DJV_TEXT("widget_view_border")) + ":");
                p.formLayouts["Background"]->setText(p.borderWidthSlider, _getText(DJV_TEXT("widget_view_border_width")) + ":");
                p.formLayouts["Background"]->setText(p.borderColorPickerSwatch, _getText(DJV_TEXT("widget_view_border_color")) + ":");

                p.bellows["View"]->setText(_getText(DJV_TEXT("view")));
                p.bellows["Grid"]->setText(_getText(DJV_TEXT("view_grid")));
                p.bellows["HUD"]->setText(_getText(DJV_TEXT("view_hud")));
                p.bellows["Background"]->setText(_getText(DJV_TEXT("view_background")));

                _widgetUpdate();
            }
        }

        void ViewControlsWidget::_setPos(const glm::vec2& value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ViewLock::None);
                p.viewPos = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getViewWidget()->setImagePos(p.viewPos);
                }
            }
        }

        void ViewControlsWidget::_setZoom(float value)
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                auto settingsSystem = context->getSystemT<UI::Settings::System>();
                auto viewSettings = settingsSystem->getSettingsT<ViewSettings>();
                viewSettings->setLock(ViewLock::None);
                p.viewZoom = value;
                _widgetUpdate();
                if (p.activeWidget)
                {
                    p.activeWidget->getViewWidget()->setImageZoomFocus(value);
                }
            }
        }

        void ViewControlsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.viewPosEdit[0]->setValue(p.viewPos.x);
            p.viewPosEdit[1]->setValue(p.viewPos.y);
            p.viewPosResetButton[0]->setEnabled(p.viewPos.x != 0.F);
            p.viewPosResetButton[1]->setEnabled(p.viewPos.y != 0.F);
            p.viewZoomEdit->setValue(p.viewZoom);
            p.viewZoomResetButton->setEnabled(p.viewZoom != 1.F);

            p.gridEnabledButton->setChecked(p.gridOptions.enabled);
            p.gridSizeSlider->setValue(p.gridOptions.size);
            p.gridColorPickerSwatch->setColor(p.gridOptions.color);
            std::vector<std::string> items;
            for (auto i : getGridLabelsEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.gridLabelsComboBox->setItems(items);
            p.gridLabelsComboBox->setCurrentItem(static_cast<int>(p.gridOptions.labels));
            p.gridLabelsColorPickerSwatch->setColor(p.gridOptions.labelsColor);

            p.hudEnabledButton->setChecked(p.hudOptions.enabled);
            p.hudColorPickerSwatch->setColor(p.hudOptions.color);
            items.clear();
            for (auto i : getHUDBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.hudBackgroundComboBox->setItems(items);
            p.hudBackgroundComboBox->setCurrentItem(static_cast<int>(p.hudOptions.background));

            items.clear();
            for (auto i : getViewBackgroundEnums())
            {
                std::stringstream ss;
                ss << i;
                items.push_back(_getText(ss.str()));
            }
            p.backgroundComboBox->setItems(items);
            p.backgroundComboBox->setCurrentItem(static_cast<int>(p.backgroundOptions.background));
            p.backgroundColorPickerSwatch->setColor(p.backgroundOptions.color);
            p.checkersSizeSlider->setValue(p.backgroundOptions.checkersSize);
            p.checkersColorPickerSwatches[0]->setColor(p.backgroundOptions.checkersColors[0]);
            p.checkersColorPickerSwatches[1]->setColor(p.backgroundOptions.checkersColors[1]);
            p.borderCheckBox->setChecked(p.backgroundOptions.border);
            p.borderWidthSlider->setValue(p.backgroundOptions.borderWidth);
            p.borderWidthSlider->setRange(FloatRange(1.F, 20.F));
            p.borderColorPickerSwatch->setColor(p.backgroundOptions.borderColor);
        }

    } // namespace ViewApp
} // namespace djv


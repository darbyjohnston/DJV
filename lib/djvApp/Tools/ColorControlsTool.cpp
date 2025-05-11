// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/ColorControlsPrivate.h>

#include <djvApp/Models/ColorModel.h>
#include <djvApp/Models/ViewportModel.h>
#include <djvApp/App.h>

#include <dtk/ui/Bellows.h>
#include <dtk/ui/ButtonGroup.h>
#include <dtk/ui/CheckBox.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/FileEdit.h>
#include <dtk/ui/FloatEdit.h>
#include <dtk/ui/FloatEditSlider.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>
#include <dtk/ui/Settings.h>
#include <dtk/ui/StackLayout.h>

namespace djv
{
    namespace app
    {
        struct OCIOWidget::Private
        {
            std::shared_ptr<OCIOModel> ocioModel;

            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::shared_ptr<dtk::FileEdit> fileEdit;
            std::shared_ptr<dtk::ComboBox> inputComboBox;
            std::shared_ptr<dtk::ComboBox> displayComboBox;
            std::shared_ptr<dtk::ComboBox> viewComboBox;
            std::shared_ptr<dtk::ComboBox> lookComboBox;
            std::shared_ptr<dtk::VerticalLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::OCIOOptions> > optionsObserver;
            std::shared_ptr<dtk::ValueObserver<tl::timeline::OCIOOptions> > optionsObserver2;
            std::shared_ptr<dtk::ValueObserver<OCIOModelData> > dataObserver;
        };

        void OCIOWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::OCIOWidget", parent);
            DTK_P();
            
#if !defined(TLRENDER_OCIO)
            setEnabled(false);
#endif // TLRENDER_OCIO

            p.ocioModel = OCIOModel::create(context);

            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.fileEdit = dtk::FileEdit::create(context);

            p.inputComboBox = dtk::ComboBox::create(context);
            p.inputComboBox->setHStretch(dtk::Stretch::Expanding);

            p.displayComboBox = dtk::ComboBox::create(context);
            p.displayComboBox->setHStretch(dtk::Stretch::Expanding);

            p.viewComboBox = dtk::ComboBox::create(context);
            p.viewComboBox->setHStretch(dtk::Stretch::Expanding);

            p.lookComboBox = dtk::ComboBox::create(context);
            p.lookComboBox->setHStretch(dtk::Stretch::Expanding);

            p.layout = dtk::VerticalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            auto formLayout = dtk::FormLayout::create(context, p.layout);
            formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            formLayout->addRow("Enabled:", p.enabledCheckBox);
            formLayout->addRow("File name:", p.fileEdit);
            formLayout->addRow("Input:", p.inputComboBox);
            formLayout->addRow("Display:", p.displayComboBox);
            formLayout->addRow("View:", p.viewComboBox);
            formLayout->addRow("Look:", p.lookComboBox);

            p.optionsObserver = dtk::ValueObserver<tl::timeline::OCIOOptions>::create(
                app->getColorModel()->observeOCIOOptions(),
                [this](const tl::timeline::OCIOOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.enabled);
                    _p->fileEdit->setPath(std::filesystem::u8path(value.fileName));
                    _p->ocioModel->setOptions(value);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.optionsObserver2 = dtk::ValueObserver<tl::timeline::OCIOOptions>::create(
                p.ocioModel->observeOptions(),
                [appWeak](const tl::timeline::OCIOOptions& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getColorModel()->setOCIOOptions(value);
                    }
                });

            p.dataObserver = dtk::ValueObserver<OCIOModelData>::create(
                p.ocioModel->observeData(),
                [this](const OCIOModelData& value)
                {
                    _p->enabledCheckBox->setChecked(value.enabled);
                    _p->fileEdit->setPath(std::filesystem::u8path(value.fileName));
                    _p->inputComboBox->setItems(value.inputs);
                    _p->inputComboBox->setCurrentIndex(value.inputIndex);
                    _p->displayComboBox->setItems(value.displays);
                    _p->displayComboBox->setCurrentIndex(value.displayIndex);
                    _p->viewComboBox->setItems(value.views);
                    _p->viewComboBox->setCurrentIndex(value.viewIndex);
                    _p->lookComboBox->setItems(value.looks);
                    _p->lookComboBox->setCurrentIndex(value.lookIndex);
                });

            p.enabledCheckBox->setCheckedCallback(
                [this](bool value)
                {
                    _p->ocioModel->setEnabled(value);
                });

            p.fileEdit->setCallback(
                [this](const std::filesystem::path& value)
                {
                    _p->ocioModel->setConfig(value.u8string());
                });

            p.inputComboBox->setIndexCallback(
                [this](int index)
                {
                    _p->ocioModel->setInputIndex(index);
                });
            p.displayComboBox->setIndexCallback(
                [this](int index)
                {
                    _p->ocioModel->setDisplayIndex(index);
                });
            p.viewComboBox->setIndexCallback(
                [this](int index)
                {
                    _p->ocioModel->setViewIndex(index);
                });
            p.lookComboBox->setIndexCallback(
                [this](int index)
                {
                    _p->ocioModel->setLookIndex(index);
                });
        }

        OCIOWidget::OCIOWidget() :
            _p(new Private)
        {}

        OCIOWidget::~OCIOWidget()
        {}

        std::shared_ptr<OCIOWidget> OCIOWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<OCIOWidget>(new OCIOWidget);
            out->_init(context, app, parent);
            return out;
        }

        void OCIOWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void OCIOWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct LUTWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::shared_ptr<dtk::FileEdit> fileEdit;
            std::shared_ptr<dtk::ComboBox> orderComboBox;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::LUTOptions> > optionsObservers;
        };

        void LUTWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::LUTWidget", parent);
            DTK_P();
            
#if !defined(TLRENDER_OCIO)
            setEnabled(false);
#endif // TLRENDER_OCIO

            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.fileEdit = dtk::FileEdit::create(context);

            p.orderComboBox = dtk::ComboBox::create(context, tl::timeline::getLUTOrderLabels());
            p.orderComboBox->setHStretch(dtk::Stretch::Expanding);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("File name:", p.fileEdit);
            p.layout->addRow("Order:", p.orderComboBox);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const tl::timeline::LUTOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.enabled);
                    _p->fileEdit->setPath(std::filesystem::u8path(value.fileName));
                    _p->orderComboBox->setCurrentIndex(static_cast<size_t>(value.order));
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getColorModel()->getLUTOptions();
                        options.enabled = value;
                        app->getColorModel()->setLUTOptions(options);
                    }
                });

            p.fileEdit->setCallback(
                [appWeak](const std::filesystem::path& value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getColorModel()->getLUTOptions();
                        options.enabled = true;
                        options.fileName = value.u8string();
                        app->getColorModel()->setLUTOptions(options);
                    }
                });

            p.orderComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getColorModel()->getLUTOptions();
                        options.enabled = true;
                        options.order = static_cast<tl::timeline::LUTOrder>(value);
                        app->getColorModel()->setLUTOptions(options);
                    }
                });
        }

        LUTWidget::LUTWidget() :
            _p(new Private)
        {}

        LUTWidget::~LUTWidget()
        {}

        std::shared_ptr<LUTWidget> LUTWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<LUTWidget>(new LUTWidget);
            out->_init(context, app, parent);
            return out;
        }

        void LUTWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void LUTWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ColorWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::map<std::string, std::shared_ptr<dtk::FloatEditSlider> > sliders;
            std::shared_ptr<dtk::CheckBox> invertCheckBox;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > optionsObservers;
        };

        void ColorWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::ColorWidget", parent);
            DTK_P();

            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.sliders["Add"] = dtk::FloatEditSlider::create(context);
            p.sliders["Add"]->setRange(dtk::RangeF(-1.F, 1.F));
            p.sliders["Add"]->setDefaultValue(0.F);
            p.sliders["Brightness"] = dtk::FloatEditSlider::create(context);
            p.sliders["Brightness"]->setRange(dtk::RangeF(0.F, 4.F));
            p.sliders["Brightness"]->setDefaultValue(1.F);
            p.sliders["Contrast"] = dtk::FloatEditSlider::create(context);
            p.sliders["Contrast"]->setRange(dtk::RangeF(0.F, 4.F));
            p.sliders["Contrast"]->setDefaultValue(1.F);
            p.sliders["Saturation"] = dtk::FloatEditSlider::create(context);
            p.sliders["Saturation"]->setRange(dtk::RangeF(0.F, 4.F));
            p.sliders["Saturation"]->setDefaultValue(1.F);
            p.sliders["Tint"] = dtk::FloatEditSlider::create(context);
            p.sliders["Tint"]->setDefaultValue(1.F);

            p.invertCheckBox = dtk::CheckBox::create(context);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Add:", p.sliders["Add"]);
            p.layout->addRow("Brightness:", p.sliders["Brightness"]);
            p.layout->addRow("Contrast:", p.sliders["Contrast"]);
            p.layout->addRow("Saturation:", p.sliders["Saturation"]);
            p.layout->addRow("Tint:", p.sliders["Tint"]);
            p.layout->addRow("Invert:", p.invertCheckBox);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.color.enabled);
                    _p->sliders["Add"]->setValue(value.color.add.x);
                    _p->sliders["Brightness"]->setValue(value.color.brightness.x);
                    _p->sliders["Contrast"]->setValue(value.color.contrast.x);
                    _p->sliders["Saturation"]->setValue(value.color.saturation.x);
                    _p->sliders["Tint"]->setValue(value.color.tint);
                    _p->invertCheckBox->setChecked(value.color.invert);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Add"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.add.x = value;
                        options.color.add.y = value;
                        options.color.add.z = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Brightness"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.brightness.x = value;
                        options.color.brightness.y = value;
                        options.color.brightness.z = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Contrast"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.contrast.x = value;
                        options.color.contrast.y = value;
                        options.color.contrast.z = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Saturation"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.saturation.x = value;
                        options.color.saturation.y = value;
                        options.color.saturation.z = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Tint"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.tint = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.invertCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.color.enabled = true;
                        options.color.invert = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });
        }

        ColorWidget::ColorWidget() :
            _p(new Private)
        {}

        ColorWidget::~ColorWidget()
        {}

        std::shared_ptr<ColorWidget> ColorWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorWidget>(new ColorWidget);
            out->_init(context, app, parent);
            return out;
        }

        void ColorWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void ColorWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct LevelsWidget::Private
        {
            std::shared_ptr<dtk::Settings> settings;

            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::map<std::string, std::shared_ptr<dtk::FloatEditSlider> > sliders;
            std::map<std::string, std::shared_ptr<dtk::FloatEdit> > rangeEdits;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > optionsObservers;
        };

        void LevelsWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::LevelsWidget", parent);
            DTK_P();

            p.settings = app->getSettings();

            p.enabledCheckBox = dtk::CheckBox::create(context);

            dtk::RangeF range(0.F, 1.F);
            p.settings->getT("/ColorControls/Levels/InRange", range);
            p.sliders["InLow"] = dtk::FloatEditSlider::create(context);
            p.sliders["InLow"]->setRange(range);
            p.sliders["InLow"]->setDefaultValue(0.F);

            p.sliders["InHigh"] = dtk::FloatEditSlider::create(context);
            p.sliders["InHigh"]->setRange(range);
            p.sliders["InHigh"]->setDefaultValue(1.F);

            p.rangeEdits["InMin"] = dtk::FloatEdit::create(context);
            p.rangeEdits["InMin"]->setRange(dtk::RangeF(-1000000.F, 1000000.F));
            p.rangeEdits["InMin"]->setValue(range.min());

            p.rangeEdits["InMax"] = dtk::FloatEdit::create(context);
            p.rangeEdits["InMax"]->setRange(dtk::RangeF(-1000000.F, 1000000.F));
            p.rangeEdits["InMax"]->setValue(range.max());

            p.sliders["Gamma"] = dtk::FloatEditSlider::create(context);
            p.sliders["Gamma"]->setRange(dtk::RangeF(.1F, 4.F));
            p.sliders["Gamma"]->setDefaultValue(1.F);

            p.settings->getT("/ColorControls/Levels/OutRange", range);
            p.sliders["OutLow"] = dtk::FloatEditSlider::create(context);
            p.sliders["OutLow"]->setRange(range);
            p.sliders["OutLow"]->setDefaultValue(0.F);

            p.sliders["OutHigh"] = dtk::FloatEditSlider::create(context);
            p.sliders["OutHigh"]->setRange(range);
            p.sliders["OutHigh"]->setDefaultValue(1.F);

            p.rangeEdits["OutMin"] = dtk::FloatEdit::create(context);
            p.rangeEdits["OutMin"]->setRange(dtk::RangeF(-1000000.F, 1000000.F));
            p.rangeEdits["OutMin"]->setValue(range.min());

            p.rangeEdits["OutMax"] = dtk::FloatEdit::create(context);
            p.rangeEdits["OutMax"]->setRange(dtk::RangeF(-1000000.F, 1000000.F));
            p.rangeEdits["OutMax"]->setValue(range.max());

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("In low:", p.sliders["InLow"]);
            p.layout->addRow("In high:", p.sliders["InHigh"]);
            auto hLayout = dtk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.rangeEdits["InMin"]->setParent(hLayout);
            p.rangeEdits["InMax"]->setParent(hLayout);
            p.layout->addRow("In range:", hLayout);
            p.layout->addRow("Gamma:", p.sliders["Gamma"]);
            p.layout->addRow("Out low:", p.sliders["OutLow"]);
            p.layout->addRow("Out high:", p.sliders["OutHigh"]);
            hLayout = dtk::HorizontalLayout::create(context);
            hLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.rangeEdits["OutMin"]->setParent(hLayout);
            p.rangeEdits["OutMax"]->setParent(hLayout);
            p.layout->addRow("Out range:", hLayout);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.levels.enabled);
                    _p->sliders["InLow"]->setValue(value.levels.inLow);
                    _p->sliders["InHigh"]->setValue(value.levels.inHigh);
                    _p->sliders["Gamma"]->setValue(value.levels.gamma);
                    _p->sliders["OutLow"]->setValue(value.levels.outLow);
                    _p->sliders["OutHigh"]->setValue(value.levels.outHigh);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["InLow"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = true;
                        options.levels.inLow = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["InHigh"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = true;
                        options.levels.inHigh = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.rangeEdits["InMin"]->setCallback(
                [this](float value)
                {
                    DTK_P();
                    dtk::RangeF range = p.sliders["InLow"]->getRange();
                    range = dtk::RangeF(value, range.max());
                    p.sliders["InLow"]->setRange(range);
                    p.sliders["InHigh"]->setRange(range);
                });

            p.rangeEdits["InMax"]->setCallback(
                [this](float value)
                {
                    DTK_P();
                    dtk::RangeF range = p.sliders["InLow"]->getRange();
                    range = dtk::RangeF(range.min(), value);
                    p.sliders["InLow"]->setRange(range);
                    p.sliders["InHigh"]->setRange(range);
                });

            p.sliders["Gamma"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = true;
                        options.levels.gamma = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["OutLow"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = true;
                        options.levels.outLow = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["OutHigh"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.levels.enabled = true;
                        options.levels.outHigh = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.rangeEdits["OutMin"]->setCallback(
                [this](float value)
                {
                    DTK_P();
                    dtk::RangeF range = p.sliders["OutLow"]->getRange();
                    range = dtk::RangeF(value, range.max());
                    p.sliders["OutLow"]->setRange(range);
                    p.sliders["OutHigh"]->setRange(range);
                });

            p.rangeEdits["OutMax"]->setCallback(
                [this](float value)
                {
                    DTK_P();
                    dtk::RangeF range = p.sliders["OutLow"]->getRange();
                    range = dtk::RangeF(range.min(), value);
                    p.sliders["OutLow"]->setRange(range);
                    p.sliders["OutHigh"]->setRange(range);
                });
        }

        LevelsWidget::LevelsWidget() :
            _p(new Private)
        {}

        LevelsWidget::~LevelsWidget()
        {
            DTK_P();
            float min = p.rangeEdits["InMin"]->getValue();
            float max = p.rangeEdits["InMax"]->getValue();
            p.settings->setT("/ColorControls/Levels/InRange", dtk::RangeF(min, max));
            min = p.rangeEdits["OutMin"]->getValue();
            max = p.rangeEdits["OutMax"]->getValue();
            p.settings->setT("/ColorControls/Levels/OutRange", dtk::RangeF(min, max));
        }

        std::shared_ptr<LevelsWidget> LevelsWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<LevelsWidget>(new LevelsWidget);
            out->_init(context, app, parent);
            return out;
        }

        void LevelsWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void LevelsWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct EXRDisplayWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::map<std::string, std::shared_ptr<dtk::FloatEditSlider> > sliders;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > optionsObservers;
        };

        void EXRDisplayWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::EXRDisplayWidget", parent);
            DTK_P();

            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.sliders["Exposure"] = dtk::FloatEditSlider::create(context);
            p.sliders["Exposure"]->setRange(dtk::RangeF(-10.F, 10.F));
            p.sliders["Exposure"]->setDefaultValue(0.F);
            p.sliders["Defog"] = dtk::FloatEditSlider::create(context);
            p.sliders["Defog"]->setDefaultValue(0.F);
            p.sliders["KneeLow"] = dtk::FloatEditSlider::create(context);
            p.sliders["KneeLow"]->setRange(dtk::RangeF(-3.F, 3.F));
            p.sliders["KneeLow"]->setDefaultValue(0.F);
            p.sliders["KneeHigh"] = dtk::FloatEditSlider::create(context);
            p.sliders["KneeHigh"]->setRange(dtk::RangeF(3.5F, 7.5F));
            p.sliders["KneeHigh"]->setDefaultValue(5.F);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Exposure:", p.sliders["Exposure"]);
            p.layout->addRow("Defog:", p.sliders["Defog"]);
            p.layout->addRow("Knee low:", p.sliders["KneeLow"]);
            p.layout->addRow("Knee high:", p.sliders["KneeHigh"]);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.exrDisplay.enabled);
                    _p->sliders["Exposure"]->setValue(value.exrDisplay.exposure);
                    _p->sliders["Defog"]->setValue(value.exrDisplay.defog);
                    _p->sliders["KneeLow"]->setValue(value.exrDisplay.kneeLow);
                    _p->sliders["KneeHigh"]->setValue(value.exrDisplay.kneeHigh);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.exrDisplay.enabled = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Exposure"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.exrDisplay.enabled = true;
                        options.exrDisplay.exposure = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["Defog"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.exrDisplay.enabled = true;
                        options.exrDisplay.defog = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["KneeLow"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.exrDisplay.enabled = true;
                        options.exrDisplay.kneeLow = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["KneeHigh"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.exrDisplay.enabled = true;
                        options.exrDisplay.kneeHigh = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });
        }

        EXRDisplayWidget::EXRDisplayWidget() :
            _p(new Private)
        {}

        EXRDisplayWidget::~EXRDisplayWidget()
        {}

        std::shared_ptr<EXRDisplayWidget> EXRDisplayWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<EXRDisplayWidget>(new EXRDisplayWidget);
            out->_init(context, app, parent);
            return out;
        }

        void EXRDisplayWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void EXRDisplayWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct SoftClipWidget::Private
        {
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::map<std::string, std::shared_ptr<dtk::FloatEditSlider> > sliders;
            std::shared_ptr<dtk::FormLayout> layout;

            std::shared_ptr<dtk::ValueObserver<tl::timeline::DisplayOptions> > optionsObservers;
        };

        void SoftClipWidget::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<dtk::IWidget>& parent)
        {
            dtk::IWidget::_init(context, "djv::app::SoftClipWidget", parent);
            DTK_P();

            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.sliders["SoftClip"] = dtk::FloatEditSlider::create(context);
            p.sliders["SoftClip"]->setDefaultValue(0.F);

            p.layout = dtk::FormLayout::create(context, shared_from_this());
            p.layout->setMarginRole(dtk::SizeRole::MarginSmall);
            p.layout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            p.layout->addRow("Enabled:", p.enabledCheckBox);
            p.layout->addRow("Soft clip:", p.sliders["SoftClip"]);

            p.optionsObservers = dtk::ValueObserver<tl::timeline::DisplayOptions>::create(
                app->getViewportModel()->observeDisplayOptions(),
                [this](const tl::timeline::DisplayOptions& value)
                {
                    _p->enabledCheckBox->setChecked(value.softClip.enabled);
                    _p->sliders["SoftClip"]->setValue(value.softClip.value);
                });

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.softClip.enabled = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });

            p.sliders["SoftClip"]->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getViewportModel()->getDisplayOptions();
                        options.softClip.enabled = true;
                        options.softClip.value = value;
                        app->getViewportModel()->setDisplayOptions(options);
                    }
                });
        }

        SoftClipWidget::SoftClipWidget() :
            _p(new Private)
        {}

        SoftClipWidget::~SoftClipWidget()
        {}

        std::shared_ptr<SoftClipWidget> SoftClipWidget::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<SoftClipWidget>(new SoftClipWidget);
            out->_init(context, app, parent);
            return out;
        }

        void SoftClipWidget::setGeometry(const dtk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void SoftClipWidget::sizeHintEvent(const dtk::SizeHintEvent& value)
        {
            IWidget::sizeHintEvent(value);
            _setSizeHint(_p->layout->getSizeHint());
        }

        struct ColorControlsTool::Private
        {
            std::shared_ptr<OCIOWidget> ocioWidget;
            std::shared_ptr<LUTWidget> lutWidget;
            std::shared_ptr<ColorWidget> colorWidget;
            std::shared_ptr<LevelsWidget> levelsWidget;
            std::shared_ptr<EXRDisplayWidget> exrDisplayWidget;
            std::shared_ptr<SoftClipWidget> softClipWidget;
            std::map<std::string, std::shared_ptr<dtk::Bellows> > bellows;
        };

        void ColorControlsTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::ColorControls,
                "djv::app::ColorControlsTool",
                parent);
            DTK_P();

            p.ocioWidget = OCIOWidget::create(context, app);
            p.lutWidget = LUTWidget::create(context, app);
            p.colorWidget = ColorWidget::create(context, app);
            p.levelsWidget = LevelsWidget::create(context, app);
            p.exrDisplayWidget = EXRDisplayWidget::create(context, app);
            p.softClipWidget = SoftClipWidget::create(context, app);

            auto layout = dtk::VerticalLayout::create(context);
            layout->setSpacingRole(dtk::SizeRole::None);
            p.bellows["OCIO"] = dtk::Bellows::create(context, "OCIO", layout);
            p.bellows["OCIO"]->setWidget(p.ocioWidget);
            p.bellows["LUT"] = dtk::Bellows::create(context, "LUT", layout);
            p.bellows["LUT"]->setWidget(p.lutWidget);
            p.bellows["Color"] = dtk::Bellows::create(context, "Color", layout);
            p.bellows["Color"]->setWidget(p.colorWidget);
            p.bellows["Levels"] = dtk::Bellows::create(context, "Levels", layout);
            p.bellows["Levels"]->setWidget(p.levelsWidget);
            p.bellows["EXRDisplay"] = dtk::Bellows::create(context, "EXR Display", layout);
            p.bellows["EXRDisplay"]->setWidget(p.exrDisplayWidget);
            p.bellows["SoftClip"] = dtk::Bellows::create(context, "Soft Clip", layout);
            p.bellows["SoftClip"]->setWidget(p.softClipWidget);
            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            _loadSettings(p.bellows);
        }

        ColorControlsTool::ColorControlsTool() :
            _p(new Private)
        {}

        ColorControlsTool::~ColorControlsTool()
        {
            _saveSettings(_p->bellows);
        }

        std::shared_ptr<ColorControlsTool> ColorControlsTool::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<ColorControlsTool>(new ColorControlsTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}

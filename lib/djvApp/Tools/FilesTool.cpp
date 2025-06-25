// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/FilesToolPrivate.h>

#include <djvApp/App.h>

#include <feather-tk/ui/Bellows.h>
#include <feather-tk/ui/ButtonGroup.h>
#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/FloatEditSlider.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/GridLayout.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>
#include <feather-tk/ui/Settings.h>
#include <feather-tk/ui/ToolButton.h>

namespace djv
{
    namespace app
    {
        struct FilesTool::Private
        {
            std::shared_ptr<feather_tk::Settings> settings;

            std::shared_ptr<feather_tk::ButtonGroup> aButtonGroup;
            std::shared_ptr<feather_tk::ButtonGroup> bButtonGroup;
            std::map<std::shared_ptr<FilesModelItem>, std::shared_ptr<FileButton> > aButtons;
            std::map<std::shared_ptr<FilesModelItem>, std::shared_ptr<feather_tk::ToolButton> > bButtons;
            std::vector<std::shared_ptr<feather_tk::ComboBox> > layerComboBoxes;
            std::shared_ptr<feather_tk::ComboBox> compareComboBox;
            std::shared_ptr<feather_tk::ComboBox> compareTimeComboBox;
            std::shared_ptr<feather_tk::FloatEditSlider> wipeXSlider;
            std::shared_ptr<feather_tk::FloatEditSlider> wipeYSlider;
            std::shared_ptr<feather_tk::FloatEditSlider> wipeRotationSlider;
            std::shared_ptr<feather_tk::Label> wipeLabel;
            std::shared_ptr<feather_tk::FloatEditSlider> overlaySlider;
            std::shared_ptr<feather_tk::FormLayout> compareLayout;
            std::map<std::string, std::shared_ptr<feather_tk::Bellows> > bellows;
            std::shared_ptr<feather_tk::GridLayout> widgetLayout;

            std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<FilesModelItem> > > filesObserver;
            std::shared_ptr<feather_tk::ValueObserver<std::shared_ptr<FilesModelItem> > > aObserver;
            std::shared_ptr<feather_tk::ListObserver<std::shared_ptr<FilesModelItem> > > bObserver;
            std::shared_ptr<feather_tk::ListObserver<int> > layersObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareOptions> > compareObserver;
            std::shared_ptr<feather_tk::ValueObserver<tl::timeline::CompareTime> > compareTimeObserver;
        };

        void FilesTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Files,
                "djv::app::FilesTool",
                parent);
            FEATHER_TK_P();

            p.settings = app->getSettings();

            p.aButtonGroup = feather_tk::ButtonGroup::create(context, feather_tk::ButtonGroupType::Radio);
            p.bButtonGroup = feather_tk::ButtonGroup::create(context, feather_tk::ButtonGroupType::Check);

            p.compareComboBox = feather_tk::ComboBox::create(
                context,
                tl::timeline::getCompareLabels());
            p.compareComboBox->setHStretch(feather_tk::Stretch::Expanding);
            p.compareTimeComboBox = feather_tk::ComboBox::create(
                context,
                tl::timeline::getCompareTimeLabels());
            p.compareTimeComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p.wipeXSlider = feather_tk::FloatEditSlider::create(context);
            p.wipeXSlider->setDefaultValue(.5F);
            p.wipeYSlider = feather_tk::FloatEditSlider::create(context);
            p.wipeYSlider->setDefaultValue(.5F);
            p.wipeRotationSlider = feather_tk::FloatEditSlider::create(context);
            p.wipeRotationSlider->setRange(feather_tk::RangeF(0.F, 360.F));
            p.wipeRotationSlider->setStep(1.F);
            p.wipeRotationSlider->setLargeStep(10.F);
            p.wipeRotationSlider->setDefaultValue(0.F);

            p.overlaySlider = feather_tk::FloatEditSlider::create(context);
            p.overlaySlider->setDefaultValue(.5F);

            auto layout = feather_tk::VerticalLayout::create(context);
            layout->setSpacingRole(feather_tk::SizeRole::None);

            p.widgetLayout = feather_tk::GridLayout::create(context, layout);
            p.widgetLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            p.widgetLayout->setSpacingRole(feather_tk::SizeRole::SpacingTool);

            feather_tk::Divider::create(context, feather_tk::Orientation::Vertical, layout);

            auto vLayout = feather_tk::VerticalLayout::create(context);
            vLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            vLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.compareLayout = feather_tk::FormLayout::create(context, vLayout);
            p.compareLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            p.compareLayout->addRow("Mode:", p.compareComboBox);
            p.compareLayout->addRow("Time:", p.compareTimeComboBox);
            p.compareLayout->addRow("X:", p.wipeXSlider);
            p.compareLayout->addRow("Y:", p.wipeYSlider);
            p.compareLayout->addRow("Rotation:", p.wipeRotationSlider);
            p.compareLayout->addRow("Amount:", p.overlaySlider);
            p.wipeLabel = feather_tk::Label::create(context, "Alt+click to move the wipe position", vLayout);
            p.bellows["Compare"] = feather_tk::Bellows::create(context, "Compare", layout);
            p.bellows["Compare"]->setWidget(vLayout);

            auto scrollWidget = feather_tk::ScrollWidget::create(context, feather_tk::ScrollType::Both);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(layout);
            _setWidget(scrollWidget);

            _loadSettings(p.bellows);

            auto appWeak = std::weak_ptr<App>(app);
            p.aButtonGroup->setCheckedCallback(
                [appWeak](int index, bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setA(index);
                    }
                });

            p.bButtonGroup->setCheckedCallback(
                [appWeak](int index, bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setB(index, value);
                    }
                });

            p.compareComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getFilesModel()->getCompareOptions();
                        options.compare = static_cast<tl::timeline::Compare>(value);
                        app->getFilesModel()->setCompareOptions(options);
                    }
                });

            p.compareTimeComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getFilesModel()->setCompareTime(
                            static_cast<tl::timeline::CompareTime>(value));
                    }
                });

            p.wipeXSlider->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getFilesModel()->getCompareOptions();
                        options.wipeCenter.x = value;
                        app->getFilesModel()->setCompareOptions(options);
                    }
                });

            p.wipeYSlider->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getFilesModel()->getCompareOptions();
                        options.wipeCenter.y = value;
                        app->getFilesModel()->setCompareOptions(options);
                    }
                });

            p.wipeRotationSlider->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getFilesModel()->getCompareOptions();
                        options.wipeRotation = value;
                        app->getFilesModel()->setCompareOptions(options);
                    }
                });

            p.overlaySlider->setCallback(
                [appWeak](float value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getFilesModel()->getCompareOptions();
                        options.overlay = value;
                        app->getFilesModel()->setCompareOptions(options);
                    }
                });

            p.filesObserver = feather_tk::ListObserver<std::shared_ptr<FilesModelItem> >::create(
                app->getFilesModel()->observeFiles(),
                [this](const std::vector<std::shared_ptr<FilesModelItem> >& value)
                {
                    _filesUpdate(value);
                });

            p.aObserver = feather_tk::ValueObserver<std::shared_ptr<FilesModelItem> >::create(
                app->getFilesModel()->observeA(),
                [this](const std::shared_ptr<FilesModelItem>& value)
                {
                    _aUpdate(value);
                });

            p.bObserver = feather_tk::ListObserver<std::shared_ptr<FilesModelItem> >::create(
                app->getFilesModel()->observeB(),
                [this](const std::vector<std::shared_ptr<FilesModelItem> >& value)
                {
                    _bUpdate(value);
                });

            p.layersObserver = feather_tk::ListObserver<int>::create(
                app->getFilesModel()->observeLayers(),
                [this](const std::vector<int>& value)
                {
                    _layersUpdate(value);
                });

            p.compareObserver = feather_tk::ValueObserver<tl::timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const tl::timeline::CompareOptions& value)
                {
                    _compareUpdate(value);
                });

            p.compareTimeObserver = feather_tk::ValueObserver<tl::timeline::CompareTime>::create(
                app->getFilesModel()->observeCompareTime(),
                [this](const tl::timeline::CompareTime& value)
                {
                    _p->compareTimeComboBox->setCurrentIndex(static_cast<int>(value));
                });
        }

        FilesTool::FilesTool() :
            _p(new Private)
        {}

        FilesTool::~FilesTool()
        {
            _saveSettings(_p->bellows);
        }

        std::shared_ptr<FilesTool> FilesTool::create(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FilesTool>(new FilesTool);
            out->_init(context, app, parent);
            return out;
        }

        void FilesTool::_filesUpdate(const std::vector<std::shared_ptr<FilesModelItem> >& value)
        {
            FEATHER_TK_P();
            p.aButtonGroup->clearButtons();
            p.bButtonGroup->clearButtons();
            p.layerComboBoxes.clear();
            auto children = p.widgetLayout->getChildren();
            for (const auto& widget : children)
            {
                widget->setParent(nullptr);
            }
            children.clear();
            auto appWeak = _app;
            if (auto app = appWeak.lock())
            {
                const auto& a = app->getFilesModel()->getA();
                const auto& b = app->getFilesModel()->getB();
                if (auto context = getContext())
                {
                    size_t row = 0;
                    for (const auto& item : value)
                    {
                        auto aButton = FileButton::create(context, item);
                        aButton->setChecked(item == a);
                        aButton->setTooltip(item->path.get());
                        p.aButtons[item] = aButton;
                        p.aButtonGroup->addButton(aButton);
                        aButton->setParent(p.widgetLayout);
                        p.widgetLayout->setGridPos(aButton, row, 0);

                        auto bButton = feather_tk::ToolButton::create(context);
                        bButton->setText("B");
                        const auto i = std::find(b.begin(), b.end(), item);
                        bButton->setChecked(i != b.end());
                        bButton->setVAlign(feather_tk::VAlign::Center);
                        bButton->setTooltip("Set the B file(s)");
                        p.bButtons[item] = bButton;
                        p.bButtonGroup->addButton(bButton);
                        bButton->setParent(p.widgetLayout);
                        p.widgetLayout->setGridPos(bButton, row, 1);

                        auto layerComboBox = feather_tk::ComboBox::create(context);
                        layerComboBox->setItems(item->videoLayers);
                        layerComboBox->setCurrentIndex(item->videoLayer);
                        layerComboBox->setVAlign(feather_tk::VAlign::Center);
                        layerComboBox->setTooltip("Set the current layer");
                        p.layerComboBoxes.push_back(layerComboBox);
                        layerComboBox->setParent(p.widgetLayout);
                        p.widgetLayout->setGridPos(layerComboBox, row, 2);

                        layerComboBox->setIndexCallback(
                            [appWeak, item](int value)
                            {
                                if (auto app = appWeak.lock())
                                {
                                    app->getFilesModel()->setLayer(item, value);
                                }
                            });

                        ++row;
                    }
                    if (value.empty())
                    {
                        auto label = feather_tk::Label::create(context, "No files open", p.widgetLayout);
                        p.widgetLayout->setGridPos(label, 0, 0);
                    }
                }
            }
        }

        void FilesTool::_aUpdate(const std::shared_ptr<FilesModelItem>& value)
        {
            FEATHER_TK_P();
            for (const auto& button : p.aButtons)
            {
                button.second->setChecked(button.first == value);
            }
        }

        void FilesTool::_bUpdate(const std::vector<std::shared_ptr<FilesModelItem> >& value)
        {
            FEATHER_TK_P();
            for (const auto& button : p.bButtons)
            {
                const auto i = std::find(value.begin(), value.end(), button.first);
                button.second->setChecked(i != value.end());
            }
        }

        void FilesTool::_layersUpdate(const std::vector<int>& value)
        {
            FEATHER_TK_P();
            for (size_t i = 0; i < value.size() && i < p.layerComboBoxes.size(); ++i)
            {
                p.layerComboBoxes[i]->setCurrentIndex(value[i]);
            }
        }

        void FilesTool::_compareUpdate(const tl::timeline::CompareOptions& value)
        {
            FEATHER_TK_P();
            p.compareComboBox->setCurrentIndex(static_cast<int>(value.compare));
            p.wipeXSlider->setValue(value.wipeCenter.x);
            p.wipeYSlider->setValue(value.wipeCenter.y);
            p.wipeRotationSlider->setValue(value.wipeRotation);
            p.overlaySlider->setValue(value.overlay);

            p.compareLayout->setRowVisible(p.wipeXSlider, value.compare == tl::timeline::Compare::Wipe);
            p.compareLayout->setRowVisible(p.wipeYSlider, value.compare == tl::timeline::Compare::Wipe);
            p.compareLayout->setRowVisible(p.wipeRotationSlider, value.compare == tl::timeline::Compare::Wipe);
            p.wipeLabel->setVisible(value.compare == tl::timeline::Compare::Wipe);
            p.compareLayout->setRowVisible(p.overlaySlider, value.compare == tl::timeline::Compare::Overlay);
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2025 Darby Johnston
// All rights reserved.

#include <djvApp/Tools/DevicesTool.h>

#if defined(TLRENDER_BMD)
#include <djvApp/Models/BMDDevicesModel.h>
#endif // TLRENDER_BMD
#include <djvApp/App.h>

#if defined(TLRENDER_BMD)
#include <tlDevice/BMDOutputDevice.h>
#endif // TLRENDER_BMD

#include <dtk/ui/Bellows.h>
#include <dtk/ui/CheckBox.h>
#include <dtk/ui/ComboBox.h>
#include <dtk/ui/Divider.h>
#include <dtk/ui/FloatEdit.h>
#include <dtk/ui/FloatEditSlider.h>
#include <dtk/ui/FormLayout.h>
#include <dtk/ui/Label.h>
#include <dtk/ui/RowLayout.h>
#include <dtk/ui/ScrollWidget.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        struct DevicesTool::Private
        {
#if defined(TLRENDER_BMD)
            std::shared_ptr<dtk::CheckBox> enabledCheckBox;
            std::shared_ptr<dtk::ComboBox> deviceComboBox;
            std::shared_ptr<dtk::ComboBox> displayModeComboBox;
            std::shared_ptr<dtk::ComboBox> pixelTypeComboBox;
            std::shared_ptr<dtk::CheckBox> _444SDIVideoOutputCheckBox;
            std::shared_ptr<dtk::ComboBox> videoLevelsComboBox;

            std::shared_ptr<dtk::ValueObserver<bmd::DevicesModelData> > dataObserver;
#endif // TLRENDER_BMD
        };

        void DevicesTool::_init(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Devices,
                "tl::play_app::DevicesTool",
                parent);
            DTK_P();

#if defined(TLRENDER_BMD)
            p.enabledCheckBox = dtk::CheckBox::create(context);

            p.deviceComboBox = dtk::ComboBox::create(context);
            p.deviceComboBox->setHStretch(dtk::Stretch::Expanding);
            p.displayModeComboBox = dtk::ComboBox::create(context);
            p.displayModeComboBox->setHStretch(dtk::Stretch::Expanding);
            p.pixelTypeComboBox = dtk::ComboBox::create(context);
            p.pixelTypeComboBox->setHStretch(dtk::Stretch::Expanding);

            p._444SDIVideoOutputCheckBox = dtk::CheckBox::create(context);

            p.videoLevelsComboBox = dtk::ComboBox::create(context, dtk::getVideoLevelsLabels());
            p.videoLevelsComboBox->setHStretch(dtk::Stretch::Expanding);

            auto formLayout = dtk::FormLayout::create(context);
            formLayout->setMarginRole(dtk::SizeRole::MarginSmall);
            formLayout->setSpacingRole(dtk::SizeRole::SpacingSmall);
            formLayout->addRow("Enabled:", p.enabledCheckBox);
            formLayout->addRow("Device:", p.deviceComboBox);
            formLayout->addRow("Display mode:", p.displayModeComboBox);
            formLayout->addRow("Pixel type:", p.pixelTypeComboBox);
            formLayout->addRow("444 SDI:", p._444SDIVideoOutputCheckBox);
            formLayout->addRow("Video levels:", p.videoLevelsComboBox);

            auto scrollWidget = dtk::ScrollWidget::create(context);
            scrollWidget->setBorder(false);
            scrollWidget->setWidget(formLayout);
            _setWidget(scrollWidget);

            auto appWeak = std::weak_ptr<App>(app);
            p.enabledCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setDeviceEnabled(value);
                    }
                });

            p.deviceComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setDeviceIndex(value);
                    }
                });
            p.displayModeComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setDisplayModeIndex(value);
                    }
                });
            p.pixelTypeComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setPixelTypeIndex(value);
                    }
                });

            p._444SDIVideoOutputCheckBox->setCheckedCallback(
                [appWeak](bool value)
                {
                    if (auto app = appWeak.lock())
                    {
                        auto options = app->getBMDDevicesModel()->observeData()->get().boolOptions;
                        options[bmd::Option::_444SDIVideoOutput] = value;
                        app->getBMDDevicesModel()->setBoolOptions(options);
                    }
                });

            p.videoLevelsComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setVideoLevels(static_cast<dtk::VideoLevels>(value));
                    }
                });

            p.dataObserver = dtk::ValueObserver<bmd::DevicesModelData>::create(
                app->getBMDDevicesModel()->observeData(),
                [this](const bmd::DevicesModelData& value)
                {
                    DTK_P();
                    p.enabledCheckBox->setChecked(value.deviceEnabled);

                    p.deviceComboBox->setItems(value.devices);
                    p.deviceComboBox->setCurrentIndex(value.deviceIndex);
                    p.displayModeComboBox->setItems(value.displayModes);
                    p.displayModeComboBox->setCurrentIndex(value.displayModeIndex);
                    std::vector<std::string> pixelTypes;
                    for (const auto& pixelType : value.pixelTypes)
                    {
                        std::stringstream ss;
                        ss << pixelType;
                        pixelTypes.push_back(ss.str());
                    }
                    p.pixelTypeComboBox->setItems(pixelTypes);
                    p.pixelTypeComboBox->setCurrentIndex(value.pixelTypeIndex);

                    const auto i = value.boolOptions.find(bmd::Option::_444SDIVideoOutput);
                    p._444SDIVideoOutputCheckBox->setChecked(i != value.boolOptions.end() ? i->second : false);

                    p.videoLevelsComboBox->setCurrentIndex(static_cast<int>(value.videoLevels));
                });
#endif // TLRENDER_BMD
        }

        DevicesTool::DevicesTool() :
            _p(new Private)
        {}

        DevicesTool::~DevicesTool()
        {}

        std::shared_ptr<DevicesTool> DevicesTool::create(
            const std::shared_ptr<dtk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<DevicesTool>(new DevicesTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}

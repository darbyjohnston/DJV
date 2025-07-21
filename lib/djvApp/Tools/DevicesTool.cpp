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

#include <feather-tk/ui/Bellows.h>
#include <feather-tk/ui/CheckBox.h>
#include <feather-tk/ui/ComboBox.h>
#include <feather-tk/ui/Divider.h>
#include <feather-tk/ui/FloatEdit.h>
#include <feather-tk/ui/FloatEditSlider.h>
#include <feather-tk/ui/FormLayout.h>
#include <feather-tk/ui/Label.h>
#include <feather-tk/ui/RowLayout.h>
#include <feather-tk/ui/ScrollWidget.h>

#include <sstream>

namespace djv
{
    namespace app
    {
        struct DevicesTool::Private
        {
#if defined(TLRENDER_BMD)
            std::shared_ptr<feather_tk::CheckBox> enabledCheckBox;
            std::shared_ptr<feather_tk::ComboBox> deviceComboBox;
            std::shared_ptr<feather_tk::ComboBox> displayModeComboBox;
            std::shared_ptr<feather_tk::ComboBox> pixelTypeComboBox;
            std::shared_ptr<feather_tk::CheckBox> _444SDIVideoOutputCheckBox;
            std::shared_ptr<feather_tk::ComboBox> videoLevelsComboBox;

            std::shared_ptr<feather_tk::ValueObserver<tl::bmd::DevicesModelData> > dataObserver;
#endif // TLRENDER_BMD
        };

        void DevicesTool::_init(
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            IToolWidget::_init(
                context,
                app,
                Tool::Devices,
                "djv::app::DevicesTool",
                parent);
            FEATHER_TK_P();

#if defined(TLRENDER_BMD)
            p.enabledCheckBox = feather_tk::CheckBox::create(context);

            p.deviceComboBox = feather_tk::ComboBox::create(context);
            p.deviceComboBox->setHStretch(feather_tk::Stretch::Expanding);
            p.displayModeComboBox = feather_tk::ComboBox::create(context);
            p.displayModeComboBox->setHStretch(feather_tk::Stretch::Expanding);
            p.pixelTypeComboBox = feather_tk::ComboBox::create(context);
            p.pixelTypeComboBox->setHStretch(feather_tk::Stretch::Expanding);

            p._444SDIVideoOutputCheckBox = feather_tk::CheckBox::create(context);

            p.videoLevelsComboBox = feather_tk::ComboBox::create(context, feather_tk::getVideoLevelsLabels());
            p.videoLevelsComboBox->setHStretch(feather_tk::Stretch::Expanding);

            auto formLayout = feather_tk::FormLayout::create(context);
            formLayout->setMarginRole(feather_tk::SizeRole::MarginSmall);
            formLayout->setSpacingRole(feather_tk::SizeRole::SpacingSmall);
            formLayout->addRow("Enabled:", p.enabledCheckBox);
            formLayout->addRow("Device:", p.deviceComboBox);
            formLayout->addRow("Display mode:", p.displayModeComboBox);
            formLayout->addRow("Pixel type:", p.pixelTypeComboBox);
            formLayout->addRow("444 SDI:", p._444SDIVideoOutputCheckBox);
            formLayout->addRow("Video levels:", p.videoLevelsComboBox);

            auto scrollWidget = feather_tk::ScrollWidget::create(context);
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
                        options[tl::bmd::Option::_444SDIVideoOutput] = value;
                        app->getBMDDevicesModel()->setBoolOptions(options);
                    }
                });

            p.videoLevelsComboBox->setIndexCallback(
                [appWeak](int value)
                {
                    if (auto app = appWeak.lock())
                    {
                        app->getBMDDevicesModel()->setVideoLevels(static_cast<feather_tk::VideoLevels>(value));
                    }
                });

            p.dataObserver = feather_tk::ValueObserver<tl::bmd::DevicesModelData>::create(
                app->getBMDDevicesModel()->observeData(),
                [this](const tl::bmd::DevicesModelData& value)
                {
                    FEATHER_TK_P();
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

                    const auto i = value.boolOptions.find(tl::bmd::Option::_444SDIVideoOutput);
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
            const std::shared_ptr<feather_tk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<DevicesTool>(new DevicesTool);
            out->_init(context, app, parent);
            return out;
        }
    }
}

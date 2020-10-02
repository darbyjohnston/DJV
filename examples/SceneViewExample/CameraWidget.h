// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include "ISettingsWidget.h"

#include <djvUIComponents/SceneWidget.h>

#include <djvUI/FloatEdit.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>

class CameraWidget : public ISettingsWidget
{
    DJV_NON_COPYABLE(CameraWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    CameraWidget();

public:
    ~CameraWidget() override;

    static std::shared_ptr<CameraWidget> create(const std::shared_ptr<djv::System::Context>&);

    void setCameraData(const djv::Scene3D::PolarCameraData&);
    void setCameraDataCallback(const std::function<void(const djv::Scene3D::PolarCameraData&)>&);

    void setLabelSizeGroup(const std::weak_ptr<djv::UI::Text::LabelSizeGroup>&) override;

protected:
    void _initEvent(djv::System::Event::Init&) override;

private:
    void _widgetUpdate();

    djv::Scene3D::PolarCameraData _cameraData;
    std::map<std::string, std::shared_ptr<djv::UI::Numeric::FloatEdit> > _floatEdits;
    std::map<std::string, std::shared_ptr<djv::UI::FormLayout> > _layouts;
    std::map<std::string, std::shared_ptr<djv::UI::GroupBox> > _groupBoxes;
    std::function<void(const djv::Scene3D::PolarCameraData&)> _cameraDataCallback;
};

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include "ISettingsWidget.h"

#include <djvUIComponents/SceneWidget.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>

class RenderWidget : public ISettingsWidget
{
    DJV_NON_COPYABLE(RenderWidget);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    RenderWidget();

public:
    ~RenderWidget() override;

    static std::shared_ptr<RenderWidget> create(const std::shared_ptr<djv::Core::Context>&);

    void setRenderOptions(const djv::UI::SceneRenderOptions&);
    void setRenderOptionsCallback(const std::function<void(const djv::UI::SceneRenderOptions&)>&);

    void setLabelSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>&) override;

protected:
    void _initEvent(djv::Core::Event::Init&) override;

private:
    void _widgetUpdate();

    djv::UI::SceneRenderOptions _renderOptions;
    std::shared_ptr<djv::UI::ComboBox> _shaderModeComboBox;
    std::shared_ptr<djv::UI::ComboBox> _depthBufferModeComboBox;
    std::shared_ptr<djv::UI::ComboBox> _depthBufferTypeComboBox;
    std::shared_ptr<djv::UI::ComboBox> _multiSamplingComboBox;
    std::map<std::string, std::shared_ptr<djv::UI::FormLayout> > _layouts;
    std::map<std::string, std::shared_ptr<djv::UI::GroupBox> > _groupBoxes;
    std::function<void(const djv::UI::SceneRenderOptions&)> _renderOptionsCallback;
};

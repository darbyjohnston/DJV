// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "RenderWidget.h"

#include <djvUIComponents/SceneWidget.h>

#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvScene3D/Render.h>

#include <djvGL/OffscreenBufferFunc.h>

using namespace djv;

void RenderWidget::_init(const std::shared_ptr<System::Context>& context)
{
    ISettingsWidget::_init(context);

    _shaderModeComboBox = UI::ComboBox::create(context);
    _layouts["Shader"] = UI::FormLayout::create(context);
    _layouts["Shader"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["Shader"]->addChild(_shaderModeComboBox);
    _groupBoxes["Shader"] = UI::GroupBox::create(context);
    _groupBoxes["Shader"]->addChild(_layouts["Shader"]);

    _depthBufferModeComboBox = UI::ComboBox::create(context);
    _depthBufferTypeComboBox = UI::ComboBox::create(context);
    _layouts["DepthBuffer"] = UI::FormLayout::create(context);
    _layouts["DepthBuffer"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["DepthBuffer"]->addChild(_depthBufferModeComboBox);
    _layouts["DepthBuffer"]->addChild(_depthBufferTypeComboBox);
    _groupBoxes["DepthBuffer"] = UI::GroupBox::create(context);
    _groupBoxes["DepthBuffer"]->addChild(_layouts["DepthBuffer"]);

    _multiSamplingComboBox = UI::ComboBox::create(context);
    _layouts["Multisampling"] = UI::FormLayout::create(context);
    _layouts["Multisampling"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["Multisampling"]->addChild(_multiSamplingComboBox);
    _groupBoxes["Multisampling"] = UI::GroupBox::create(context);
    _groupBoxes["Multisampling"]->addChild(_layouts["Multisampling"]);

    auto layout = UI::VerticalLayout::create(context);
    layout->setMargin(UI::MetricsRole::Margin);
    layout->addChild(_groupBoxes["Shader"]);
    layout->addChild(_groupBoxes["DepthBuffer"]);
    layout->addChild(_groupBoxes["Multisampling"]);
    addChild(layout);

    _widgetUpdate();

    auto weak = std::weak_ptr<RenderWidget>(std::dynamic_pointer_cast<RenderWidget>(shared_from_this()));
    _shaderModeComboBox->setCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderOptions.shaderMode = static_cast<Render3D::DefaultMaterialMode>(value);
                if (widget->_renderOptionsCallback)
                {
                    widget->_renderOptionsCallback(widget->_renderOptions);
                }
            }
        });

    _depthBufferModeComboBox->setCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderOptions.depthBufferMode = static_cast<Render3D::DepthBufferMode>(value);
                if (widget->_renderOptionsCallback)
                {
                    widget->_renderOptionsCallback(widget->_renderOptions);
                }
            }
        });

    _depthBufferTypeComboBox->setCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderOptions.depthBufferType = static_cast<GL::OffscreenDepthType>(value);
                if (widget->_renderOptionsCallback)
                {
                    widget->_renderOptionsCallback(widget->_renderOptions);
                }
            }
        });

    _multiSamplingComboBox->setCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderOptions.multiSampling = static_cast<GL::OffscreenSampling>(value);
                if (widget->_renderOptionsCallback)
                {
                    widget->_renderOptionsCallback(widget->_renderOptions);
                }
            }
        });
}

RenderWidget::RenderWidget()
{}

RenderWidget::~RenderWidget()
{}

std::shared_ptr<RenderWidget> RenderWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<RenderWidget>(new RenderWidget);
    out->_init(context);
    return out;
}

void RenderWidget::setRenderOptions(const UI::SceneRenderOptions& value)
{
    _renderOptions = value;
    _widgetUpdate();
}

void RenderWidget::setRenderOptionsCallback(const std::function<void(const UI::SceneRenderOptions&)>& value)
{
    _renderOptionsCallback = value;
}

void RenderWidget::setLabelSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>& value)
{
    for (auto i : _layouts)
    {
        i.second->setLabelSizeGroup(value);
    }
}

void RenderWidget::_initEvent(System::Event::Init&)
{
    setTitle(_getText(DJV_TEXT("widget_render")));
    _layouts["Shader"]->setText(_shaderModeComboBox, _getText(DJV_TEXT("widget_render_mode")) + ":");
    _layouts["DepthBuffer"]->setText(_depthBufferModeComboBox, _getText(DJV_TEXT("widget_render_mode")) + ":");
    _layouts["DepthBuffer"]->setText(_depthBufferTypeComboBox, _getText(DJV_TEXT("widget_render_type")) + ":");
    _layouts["Multisampling"]->setText(_multiSamplingComboBox, _getText(DJV_TEXT("widget_render_samples")) + ":");
    _groupBoxes["Shader"]->setText(_getText(DJV_TEXT("widget_render_shader")));
    _groupBoxes["DepthBuffer"]->setText(_getText(DJV_TEXT("widget_render_depth_buffer")));
    _groupBoxes["Multisampling"]->setText(_getText(DJV_TEXT("widget_render_multisampling")));
    _widgetUpdate();
}

void RenderWidget::_widgetUpdate()
{
    std::vector<std::string> items;
    for (const auto& i : Render3D::getDefaultMaterialModeEnums())
    {
        std::stringstream ss;
        ss << i;
        items.push_back(_getText(ss.str()));
    }
    _shaderModeComboBox->setItems(items);
    _shaderModeComboBox->setCurrentItem(static_cast<int>(_renderOptions.shaderMode));

    items.clear();
    for (const auto& i : Render3D::getDepthBufferModeEnums())
    {
        std::stringstream ss;
        ss << i;
        items.push_back(_getText(ss.str()));
    }
    _depthBufferModeComboBox->setItems(items);
    _depthBufferModeComboBox->setCurrentItem(static_cast<int>(_renderOptions.depthBufferMode));

    items.clear();
    for (const auto& i : GL::getOffscreenDepthTypeEnums())
    {
        std::stringstream ss;
        ss << i;
        items.push_back(_getText(ss.str()));
    }
    _depthBufferTypeComboBox->setItems(items);
    _depthBufferTypeComboBox->setCurrentItem(static_cast<int>(_renderOptions.depthBufferType));

    items.clear();
    for (const auto& i : GL::getOffscreenSamplingEnums())
    {
        std::stringstream ss;
        ss << i;
        items.push_back(_getText(ss.str()));
    }
    _multiSamplingComboBox->setItems(items);
    _multiSamplingComboBox->setCurrentItem(static_cast<int>(_renderOptions.multiSampling));
}

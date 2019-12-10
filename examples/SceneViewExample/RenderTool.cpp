//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include "RenderTool.h"

#include <djvUIComponents/SceneWidget.h>

#include <djvUI/ScrollWidget.h>

#include <djvScene/Render.h>

using namespace djv;

void RenderTool::_init(const std::shared_ptr<Core::Context>& context)
{
    ITool::_init(context);

    _shaderModeComboBox = UI::ComboBox::create(context);

    _depthBufferModeComboBox = UI::ComboBox::create(context);

    _depthBufferTypeComboBox = UI::ComboBox::create(context);

    _multiSamplingComboBox = UI::ComboBox::create(context);

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _formLayout->addChild(_shaderModeComboBox);
    _formLayout->addChild(_depthBufferModeComboBox);
    _formLayout->addChild(_depthBufferTypeComboBox);
    _formLayout->addChild(_multiSamplingComboBox);
    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->setBackgroundRole(UI::ColorRole::Background);
    scrollWidget->addChild(_formLayout);
    addChild(scrollWidget);

    _widgetUpdate();

    auto weak = std::weak_ptr<RenderTool>(std::dynamic_pointer_cast<RenderTool>(shared_from_this()));
    _shaderModeComboBox->setCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                widget->_renderOptions.shaderMode = static_cast<AV::Render3D::DefaultMaterialMode>(value);
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
                widget->_renderOptions.depthBufferMode = static_cast<AV::Render3D::DepthBufferMode>(value);
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
                widget->_renderOptions.depthBufferType = static_cast<AV::OpenGL::OffscreenDepthType>(value);
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
                widget->_renderOptions.multiSampling = static_cast<AV::OpenGL::OffscreenSampling>(value);
                if (widget->_renderOptionsCallback)
                {
                    widget->_renderOptionsCallback(widget->_renderOptions);
                }
            }
        });
}

RenderTool::RenderTool()
{}

RenderTool::~RenderTool()
{}

std::shared_ptr<RenderTool> RenderTool::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<RenderTool>(new RenderTool);
    out->_init(context);
    return out;
}

void RenderTool::setRenderOptions(const UI::SceneRenderOptions& value)
{
    _renderOptions = value;
    _widgetUpdate();
}

void RenderTool::setRenderOptionsCallback(const std::function<void(const UI::SceneRenderOptions&)>& value)
{
    _renderOptionsCallback = value;
}

void RenderTool::_initEvent(Core::Event::Init&)
{
    setTitle(_getText(DJV_TEXT("Render")));
    _formLayout->setText(_shaderModeComboBox, _getText(DJV_TEXT("Shader mode")) + ":");
    _formLayout->setText(_depthBufferModeComboBox, _getText(DJV_TEXT("Depth buffer mode")) + ":");
    _formLayout->setText(_depthBufferTypeComboBox, _getText(DJV_TEXT("Depth buffer type")) + ":");
    _formLayout->setText(_multiSamplingComboBox, _getText(DJV_TEXT("Multisampling")) + ":");
    _widgetUpdate();
}

void RenderTool::_widgetUpdate()
{
    _shaderModeComboBox->clearItems();
    for (const auto& i : AV::Render3D::getDefaultMaterialModeEnums())
    {
        std::stringstream ss;
        ss << i;
        _shaderModeComboBox->addItem(ss.str());
    }
    _shaderModeComboBox->setCurrentItem(static_cast<int>(_renderOptions.shaderMode));

    _depthBufferModeComboBox->clearItems();
    for (const auto& i : AV::Render3D::getDepthBufferModeEnums())
    {
        std::stringstream ss;
        ss << i;
        _depthBufferModeComboBox->addItem(ss.str());
    }
    _depthBufferModeComboBox->setCurrentItem(static_cast<int>(_renderOptions.depthBufferMode));

    _depthBufferTypeComboBox->clearItems();
    for (const auto& i : AV::OpenGL::getOffscreenDepthTypeEnums())
    {
        std::stringstream ss;
        ss << i;
        _depthBufferTypeComboBox->addItem(ss.str());
    }
    _depthBufferTypeComboBox->setCurrentItem(static_cast<int>(_renderOptions.depthBufferType));

    _multiSamplingComboBox->clearItems();
    for (const auto& i : AV::OpenGL::getOffscreenSamplingEnums())
    {
        std::stringstream ss;
        ss << i;
        _multiSamplingComboBox->addItem(ss.str());
    }
    _multiSamplingComboBox->setCurrentItem(static_cast<int>(_renderOptions.multiSampling));
}

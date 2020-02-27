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
    virtual ~RenderWidget();

    static std::shared_ptr<RenderWidget> create(const std::shared_ptr<djv::Core::Context>&);

    void setRenderOptions(const djv::UI::SceneRenderOptions&);
    void setRenderOptionsCallback(const std::function<void(const djv::UI::SceneRenderOptions&)>&);

    void setSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>&) override;

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

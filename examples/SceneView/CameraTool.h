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

#include "ITool.h"

#include <djvUIComponents/SceneWidget.h>

#include <djvUI/FloatEdit.h>
#include <djvUI/FloatSlider.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>

class CameraTool : public ITool
{
    DJV_NON_COPYABLE(CameraTool);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    CameraTool();

public:
    virtual ~CameraTool();

    static std::shared_ptr<CameraTool> create(const std::shared_ptr<djv::Core::Context>&);

    void setCameraInfo(const djv::UI::CameraInfo&);
    void setCameraInfoCallback(const std::function<void(const djv::UI::CameraInfo&)>&);

protected:
    void _initEvent(djv::Core::Event::Init&) override;

private:
    void _widgetUpdate();

    djv::UI::CameraInfo _cameraInfo;
    std::shared_ptr<djv::UI::FloatSlider> _fovSlider;
    std::shared_ptr<djv::UI::FloatEdit> _clipEdit[2];
    std::shared_ptr<djv::UI::HorizontalLayout> _clipLayout;
    std::shared_ptr<djv::UI::FloatEdit> _distanceEdit;
    std::shared_ptr<djv::UI::FloatSlider> _latitudeSlider;
    std::shared_ptr<djv::UI::FloatSlider> _longitudeSlider;
    std::shared_ptr<djv::UI::FormLayout> _formLayout;
    std::function<void(const djv::UI::CameraInfo&)> _cameraInfoCallback;
};

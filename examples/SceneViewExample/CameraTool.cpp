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

#include "CameraTool.h"

using namespace djv;

void CameraTool::_init(const std::shared_ptr<Core::Context>& context)
{
    ITool::_init(context);

    _fovSlider = UI::FloatSlider::create(context);
    _fovSlider->setRange(Core::FloatRange(10.F, 170.F));
    auto model = _fovSlider->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _clipEdit[0] = UI::FloatEdit::create(context);
    _clipEdit[0]->setRange(Core::FloatRange(.001F, 10000000.F));
    _clipEdit[0]->setPrecision(3);
    model = _clipEdit[0]->getModel();
    model->setSmallIncrement(.1F);
    model->setLargeIncrement(10.F);
    _clipEdit[1] = UI::FloatEdit::create(context);
    _clipEdit[1]->setRange(Core::FloatRange(.001F, 10000000.F));
    _clipEdit[1]->setPrecision(3);
    model = _clipEdit[1]->getModel();
    model->setSmallIncrement(.1F);
    model->setLargeIncrement(10.F);

    _distanceEdit = UI::FloatEdit::create(context);
    _distanceEdit->setRange(Core::FloatRange(1.F, 10000000.F));
    model = _clipEdit[0]->getModel();
    model->setSmallIncrement(.1F);
    model->setLargeIncrement(10.F);

    _latitudeSlider = UI::FloatSlider::create(context);
    _latitudeSlider->setRange(Core::FloatRange(-89.F, 89.F));
    model = _latitudeSlider->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _longitudeSlider = UI::FloatSlider::create(context);
    _longitudeSlider->setRange(Core::FloatRange(0.F, 360.F));
    model = _longitudeSlider->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setBackgroundRole(UI::ColorRole::Background);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->addChild(_fovSlider);
    _clipLayout = UI::HorizontalLayout::create(context);
    _clipLayout->addChild(_clipEdit[0]);
    _clipLayout->addChild(_clipEdit[1]);
    _formLayout->addChild(_clipLayout);
    _formLayout->addChild(_distanceEdit);
    _formLayout->addChild(_latitudeSlider);
    _formLayout->addChild(_longitudeSlider);
    addChild(_formLayout);

    _widgetUpdate();

    auto weak = std::weak_ptr<CameraTool>(std::dynamic_pointer_cast<CameraTool>(shared_from_this()));
    _fovSlider->setValueCallback(
        [weak](float value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.fov = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });

    _clipEdit[0]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.clip.min = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });
    _clipEdit[1]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.clip.max = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });

    _distanceEdit->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.distance = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });

    _latitudeSlider->setValueCallback(
        [weak](float value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.latitude = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });

    _longitudeSlider->setValueCallback(
        [weak](float value)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraInfo.longitude = value;
                if (widget->_cameraInfoCallback)
                {
                    widget->_cameraInfoCallback(widget->_cameraInfo);
                }
            }
        });
}

CameraTool::CameraTool()
{}

CameraTool::~CameraTool()
{}

std::shared_ptr<CameraTool> CameraTool::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<CameraTool>(new CameraTool);
    out->_init(context);
    return out;
}

void CameraTool::setCameraInfo(const UI::CameraInfo& value)
{
    _cameraInfo = value;
    _widgetUpdate();
}

void CameraTool::setCameraInfoCallback(const std::function<void(const UI::CameraInfo&)>& value)
{
    _cameraInfoCallback = value;
}

void CameraTool::_initEvent(Core::Event::Init&)
{
    setTitle(_getText(DJV_TEXT("Camera")));

    _formLayout->setText(_fovSlider, _getText(DJV_TEXT("FOV")) + ":");
    _formLayout->setText(_clipLayout, _getText(DJV_TEXT("Clip")) + ":");
    _formLayout->setText(_distanceEdit, _getText(DJV_TEXT("Distance")) + ":");
    _formLayout->setText(_latitudeSlider, _getText(DJV_TEXT("Latitude")) + ":");
    _formLayout->setText(_longitudeSlider, _getText(DJV_TEXT("Longitude")) + ":");
}

void CameraTool::_widgetUpdate()
{
    _fovSlider->setValue(_cameraInfo.fov);
    _clipEdit[0]->setValue(_cameraInfo.clip.min);
    _clipEdit[1]->setValue(_cameraInfo.clip.max);
    _distanceEdit->setValue(_cameraInfo.distance);
    _latitudeSlider->setValue(_cameraInfo.latitude);
    _longitudeSlider->setValue(_cameraInfo.longitude);
}
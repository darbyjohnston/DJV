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

#include "CameraWidget.h"

using namespace djv;

namespace
{
    const float posMax = 10000000.F;
}

void CameraWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    ISettingsWidget::_init(context);

    _floatEdits["FOV"] = UI::FloatEdit::create(context);
    _floatEdits["FOV"]->setRange(Core::FloatRange(10.F, 170.F));
    auto model = _floatEdits["FOV"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["NearClip"] = UI::FloatEdit::create(context);
    _floatEdits["NearClip"]->setRange(Core::FloatRange(.001F, posMax));
    _floatEdits["NearClip"]->setPrecision(3);
    model = _floatEdits["NearClip"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _floatEdits["FarClip"] = UI::FloatEdit::create(context);
    _floatEdits["FarClip"]->setRange(Core::FloatRange(.001F, posMax));
    _floatEdits["FarClip"]->setPrecision(3);
    model = _floatEdits["FarClip"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["TargetX"] = UI::FloatEdit::create(context);
    _floatEdits["TargetX"]->setRange(Core::FloatRange(-posMax, posMax));
    model = _floatEdits["TargetX"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["TargetY"] = UI::FloatEdit::create(context);
    _floatEdits["TargetY"]->setRange(Core::FloatRange(-posMax, posMax));
    model = _floatEdits["TargetY"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["TargetZ"] = UI::FloatEdit::create(context);
    _floatEdits["TargetZ"]->setRange(Core::FloatRange(-posMax, posMax));
    model = _floatEdits["TargetZ"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["Distance"] = UI::FloatEdit::create(context);
    _floatEdits["Distance"]->setRange(Core::FloatRange(1.F, posMax));
    model = _floatEdits["Distance"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["Latitude"] = UI::FloatEdit::create(context);
    _floatEdits["Latitude"]->setRange(Core::FloatRange(-89.F, 89.F));
    model = _floatEdits["Latitude"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _floatEdits["Longitutde"] = UI::FloatEdit::create(context);
    _floatEdits["Longitutde"]->setRange(Core::FloatRange(0.F, 360.F));
    model = _floatEdits["Longitutde"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _formLayout->addChild(_floatEdits["FOV"]);
    _formLayout->addChild(_floatEdits["NearClip"]);
    _formLayout->addChild(_floatEdits["FarClip"]);
    _formLayout->addChild(_floatEdits["TargetX"]);
    _formLayout->addChild(_floatEdits["TargetY"]);
    _formLayout->addChild(_floatEdits["TargetZ"]);
    _formLayout->addChild(_floatEdits["Distance"]);
    _formLayout->addChild(_floatEdits["Latitude"]);
    _formLayout->addChild(_floatEdits["Longitutde"]);
    addChild(_formLayout);

    _widgetUpdate();

    auto weak = std::weak_ptr<CameraWidget>(std::dynamic_pointer_cast<CameraWidget>(shared_from_this()));
    _floatEdits["FOV"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.fov = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["NearClip"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.clip.min = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });
    _floatEdits["FarClip"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.clip.max = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["TargetX"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.target.x = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["TargetY"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.target.y = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["TargetZ"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.target.z = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["Distance"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.distance = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["Latitude"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.latitude = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });

    _floatEdits["Longitutde"]->setValueCallback(
        [weak](float value, UI::TextEditReason)
        {
            if (auto widget = weak.lock())
            {
                widget->_cameraData.longitude = value;
                if (widget->_cameraDataCallback)
                {
                    widget->_cameraDataCallback(widget->_cameraData);
                }
            }
        });
}

CameraWidget::CameraWidget()
{}

CameraWidget::~CameraWidget()
{}

std::shared_ptr<CameraWidget> CameraWidget::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<CameraWidget>(new CameraWidget);
    out->_init(context);
    return out;
}

void CameraWidget::setCameraData(const Scene::PolarCameraData& value)
{
    _cameraData = value;
    _widgetUpdate();
}

void CameraWidget::setCameraDataCallback(const std::function<void(const Scene::PolarCameraData&)>& value)
{
    _cameraDataCallback = value;
}

void CameraWidget::_initEvent(Core::Event::Init&)
{
    setTitle(_getText(DJV_TEXT("Camera")));
    _formLayout->setText(_floatEdits["FOV"], _getText(DJV_TEXT("FOV")) + ":");
    _formLayout->setText(_floatEdits["NearClip"], _getText(DJV_TEXT("Near clip")) + ":");
    _formLayout->setText(_floatEdits["FarClip"], _getText(DJV_TEXT("Far clip")) + ":");
    _formLayout->setText(_floatEdits["TargetX"], _getText(DJV_TEXT("Target X")) + ":");
    _formLayout->setText(_floatEdits["TargetY"], _getText(DJV_TEXT("Target Y")) + ":");
    _formLayout->setText(_floatEdits["TargetZ"], _getText(DJV_TEXT("Target Z")) + ":");
    _formLayout->setText(_floatEdits["Distance"], _getText(DJV_TEXT("Distance")) + ":");
    _formLayout->setText(_floatEdits["Latitude"], _getText(DJV_TEXT("Latitude")) + ":");
    _formLayout->setText(_floatEdits["Longitutde"], _getText(DJV_TEXT("Longitude")) + ":");
}

void CameraWidget::_widgetUpdate()
{
    _floatEdits["FOV"]->setValue(_cameraData.fov);
    _floatEdits["NearClip"]->setValue(_cameraData.clip.min);
    _floatEdits["FarClip"]->setValue(_cameraData.clip.max);
    _floatEdits["TargetX"]->setValue(_cameraData.target.x);
    _floatEdits["TargetY"]->setValue(_cameraData.target.y);
    _floatEdits["TargetZ"]->setValue(_cameraData.target.z);
    _floatEdits["Distance"]->setValue(_cameraData.distance);
    _floatEdits["Latitude"]->setValue(_cameraData.latitude);
    _floatEdits["Longitutde"]->setValue(_cameraData.longitude);
}

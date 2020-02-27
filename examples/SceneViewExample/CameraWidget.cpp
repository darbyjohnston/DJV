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

#include <djvUI/RowLayout.h>

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
    _layouts["Lens"] = UI::FormLayout::create(context);
    _layouts["Lens"]->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _layouts["Lens"]->addChild(_floatEdits["FOV"]);
    _groupBoxes["Lens"] = UI::GroupBox::create(context);
    _groupBoxes["Lens"]->addChild(_layouts["Lens"]);

    _floatEdits["ClippingNear"] = UI::FloatEdit::create(context);
    _floatEdits["ClippingNear"]->setRange(Core::FloatRange(.001F, posMax));
    _floatEdits["ClippingNear"]->setPrecision(3);
    model = _floatEdits["ClippingNear"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _floatEdits["ClippingFar"] = UI::FloatEdit::create(context);
    _floatEdits["ClippingFar"]->setRange(Core::FloatRange(.001F, posMax));
    _floatEdits["ClippingFar"]->setPrecision(3);
    model = _floatEdits["ClippingFar"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _layouts["Clipping"] = UI::FormLayout::create(context);
    _layouts["Clipping"]->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _layouts["Clipping"]->addChild(_floatEdits["ClippingNear"]);
    _layouts["Clipping"]->addChild(_floatEdits["ClippingFar"]);
    _groupBoxes["Clipping"] = UI::GroupBox::create(context);
    _groupBoxes["Clipping"]->addChild(_layouts["Clipping"]);

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
    _layouts["Target"] = UI::FormLayout::create(context);
    _layouts["Target"]->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _layouts["Target"]->addChild(_floatEdits["TargetX"]);
    _layouts["Target"]->addChild(_floatEdits["TargetY"]);
    _layouts["Target"]->addChild(_floatEdits["TargetZ"]);
    _groupBoxes["Target"] = UI::GroupBox::create(context);
    _groupBoxes["Target"]->addChild(_layouts["Target"]);

    _floatEdits["PositionDistance"] = UI::FloatEdit::create(context);
    _floatEdits["PositionDistance"]->setRange(Core::FloatRange(1.F, posMax));
    model = _floatEdits["PositionDistance"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _floatEdits["PositionLatitude"] = UI::FloatEdit::create(context);
    _floatEdits["PositionLatitude"]->setRange(Core::FloatRange(-89.F, 89.F));
    model = _floatEdits["PositionLatitude"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _floatEdits["PositionLongitude"] = UI::FloatEdit::create(context);
    _floatEdits["PositionLongitude"]->setRange(Core::FloatRange(0.F, 360.F));
    model = _floatEdits["PositionLongitude"]->getModel();
    model->setSmallIncrement(1.F);
    model->setLargeIncrement(10.F);
    _layouts["Position"] = UI::FormLayout::create(context);
    _layouts["Position"]->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _layouts["Position"]->addChild(_floatEdits["PositionDistance"]);
    _layouts["Position"]->addChild(_floatEdits["PositionLatitude"]);
    _layouts["Position"]->addChild(_floatEdits["PositionLongitude"]);
    _groupBoxes["Position"] = UI::GroupBox::create(context);
    _groupBoxes["Position"]->addChild(_layouts["Position"]);

    auto layout = UI::VerticalLayout::create(context);
    layout->addChild(_groupBoxes["Lens"]);
    layout->addChild(_groupBoxes["Clipping"]);
    layout->addChild(_groupBoxes["Target"]);
    layout->addChild(_groupBoxes["Position"]);
    layout->addSpacer();
    addChild(layout);

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

    _floatEdits["ClippingNear"]->setValueCallback(
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
    _floatEdits["ClippingFar"]->setValueCallback(
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

    _floatEdits["PositionDistance"]->setValueCallback(
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

    _floatEdits["PositionLatitude"]->setValueCallback(
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

    _floatEdits["PositionLongitude"]->setValueCallback(
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

void CameraWidget::setSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>& value)
{
    for (auto i : _layouts)
    {
        i.second->setSizeGroup(value);
    }
}

void CameraWidget::_initEvent(Core::Event::Init&)
{
    setTitle(_getText(DJV_TEXT("widget_camera")));
    _layouts["Lens"]->setText(_floatEdits["FOV"], _getText(DJV_TEXT("widget_camera_fov")) + ":");
    _layouts["Clipping"]->setText(_floatEdits["ClippingNear"], _getText(DJV_TEXT("widget_camera_near")) + ":");
    _layouts["Clipping"]->setText(_floatEdits["ClippingFar"], _getText(DJV_TEXT("widget_camera_far")) + ":");
    _layouts["Target"]->setText(_floatEdits["TargetX"], _getText(DJV_TEXT("widget_camera_x")) + ":");
    _layouts["Target"]->setText(_floatEdits["TargetY"], _getText(DJV_TEXT("widget_camera_y")) + ":");
    _layouts["Target"]->setText(_floatEdits["TargetZ"], _getText(DJV_TEXT("widget_camera_z")) + ":");
    _layouts["Position"]->setText(_floatEdits["PositionDistance"], _getText(DJV_TEXT("widget_camera_distance")) + ":");
    _layouts["Position"]->setText(_floatEdits["PositionLatitude"], _getText(DJV_TEXT("widget_camera_latitude")) + ":");
    _layouts["Position"]->setText(_floatEdits["PositionLongitude"], _getText(DJV_TEXT("widget_camera_longitude")) + ":");
    _groupBoxes["Lens"]->setText(_getText(DJV_TEXT("widget_camera_lens")));
    _groupBoxes["Clipping"]->setText(_getText(DJV_TEXT("widget_camera_clipping")));
    _groupBoxes["Target"]->setText(_getText(DJV_TEXT("widget_camera_target")));
    _groupBoxes["Position"]->setText(_getText(DJV_TEXT("widget_camera_position")));
}

void CameraWidget::_widgetUpdate()
{
    _floatEdits["FOV"]->setValue(_cameraData.fov);
    _floatEdits["ClippingNear"]->setValue(_cameraData.clip.min);
    _floatEdits["ClippingFar"]->setValue(_cameraData.clip.max);
    _floatEdits["TargetX"]->setValue(_cameraData.target.x);
    _floatEdits["TargetY"]->setValue(_cameraData.target.y);
    _floatEdits["TargetZ"]->setValue(_cameraData.target.z);
    _floatEdits["PositionDistance"]->setValue(_cameraData.distance);
    _floatEdits["PositionLatitude"]->setValue(_cameraData.latitude);
    _floatEdits["PositionLongitude"]->setValue(_cameraData.longitude);
}

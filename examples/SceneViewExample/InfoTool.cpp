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

#include "InfoTool.h"

#include <djvUI/ScrollWidget.h>

using namespace djv;

void InfoTool::_init(const std::shared_ptr<Core::Context>& context)
{
    ITool::_init(context);

    _sceneSizeLabel = UI::Label::create(context);
    _sceneSizeLabel->setFont(AV::Font::familyMono);
    _sceneSizeLabel->setTextHAlign(UI::TextHAlign::Left);

    _sceneXRangeLabel = UI::Label::create(context);
    _sceneXRangeLabel->setFont(AV::Font::familyMono);
    _sceneXRangeLabel->setTextHAlign(UI::TextHAlign::Left);

    _sceneYRangeLabel = UI::Label::create(context);
    _sceneYRangeLabel->setFont(AV::Font::familyMono);
    _sceneYRangeLabel->setTextHAlign(UI::TextHAlign::Left);

    _sceneZRangeLabel = UI::Label::create(context);
    _sceneZRangeLabel->setFont(AV::Font::familyMono);
    _sceneZRangeLabel->setTextHAlign(UI::TextHAlign::Left);

    _primitivesCountLabel = UI::Label::create(context);
    _primitivesCountLabel->setFont(AV::Font::familyMono);
    _primitivesCountLabel->setTextHAlign(UI::TextHAlign::Left);

    _pointCountLabel = UI::Label::create(context);
    _pointCountLabel->setFont(AV::Font::familyMono);
    _pointCountLabel->setTextHAlign(UI::TextHAlign::Left);

    _fpsLabel = UI::Label::create(context);
    _fpsLabel->setFont(AV::Font::familyMono);
    _fpsLabel->setTextHAlign(UI::TextHAlign::Left);

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setBackgroundRole(UI::ColorRole::Background);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::SpacingSmall));
    _formLayout->addChild(_sceneSizeLabel);
    _formLayout->addChild(_sceneXRangeLabel);
    _formLayout->addChild(_sceneYRangeLabel);
    _formLayout->addChild(_sceneZRangeLabel);
    _formLayout->addChild(_primitivesCountLabel);
    _formLayout->addChild(_pointCountLabel);
    _formLayout->addChild(_fpsLabel);
    auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
    scrollWidget->setBackgroundRole(UI::ColorRole::Background);
    scrollWidget->addChild(_formLayout);
    addChild(scrollWidget);

    auto weak = std::weak_ptr<InfoTool>(std::dynamic_pointer_cast<InfoTool>(shared_from_this()));
}

InfoTool::InfoTool()
{}

InfoTool::~InfoTool()
{}

std::shared_ptr<InfoTool> InfoTool::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<InfoTool>(new InfoTool);
    out->_init(context);
    return out;
}

void InfoTool::setBBox(const Core::BBox3f& value)
{
    _bbox = value;
    _textUpdate();
}

void InfoTool::setPrimitivesCount(size_t value)
{
    _primitivesCount = value;
    _textUpdate();
}

void InfoTool::setPointCount(size_t value)
{
    _pointCount = value;
    _textUpdate();
}

void InfoTool::setFPS(float value)
{
    _fps = value;
    _textUpdate();
}

void InfoTool::_initEvent(Core::Event::Init&)
{
    _textUpdate();
}

void InfoTool::_textUpdate()
{
    setTitle(_getText(DJV_TEXT("Information")));
    _formLayout->setText(_sceneSizeLabel, _getText(DJV_TEXT("Scene size")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.w() << " x " << _bbox.h() << " x " << _bbox.d();
        _sceneSizeLabel->setText(ss.str());
    }
    _formLayout->setText(_sceneXRangeLabel, _getText(DJV_TEXT("Scene X range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.x << " - " << _bbox.max.x;
        _sceneXRangeLabel->setText(ss.str());
    }
    _formLayout->setText(_sceneYRangeLabel, _getText(DJV_TEXT("Scene Y range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.y << " - " << _bbox.max.y;
        _sceneYRangeLabel->setText(ss.str());
    }
    _formLayout->setText(_sceneZRangeLabel, _getText(DJV_TEXT("Scene Z range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.z << " - " << _bbox.max.z;
        _sceneZRangeLabel->setText(ss.str());
    }
    _formLayout->setText(_primitivesCountLabel, _getText(DJV_TEXT("Primitives")) + ":");
    {
        std::stringstream ss;
        ss << _primitivesCount;
        _primitivesCountLabel->setText(ss.str());
    }
    _formLayout->setText(_pointCountLabel, _getText(DJV_TEXT("Points")) + ":");
    {
        std::stringstream ss;
        ss << _pointCount;
        _pointCountLabel->setText(ss.str());
    }
    _formLayout->setText(_fpsLabel, _getText(DJV_TEXT("FPS")) + ":");
    {
        std::stringstream ss;
        ss.precision(2);
        ss << std::fixed << _fps;
        _fpsLabel->setText(ss.str());
    }
}

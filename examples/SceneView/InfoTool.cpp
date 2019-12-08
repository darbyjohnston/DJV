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

#include <djvUI/RowLayout.h>

using namespace djv;

void InfoTool::_init(const std::shared_ptr<Core::Context>& context)
{
    ITool::_init(context);

    _bboxLabel = UI::Label::create(context);
    _bboxLabel->setFont(AV::Font::familyMono);
    _bboxLabel->setTextHAlign(UI::TextHAlign::Left);

    _primitivesCountLabel = UI::Label::create(context);
    _primitivesCountLabel->setFont(AV::Font::familyMono);
    _primitivesCountLabel->setTextHAlign(UI::TextHAlign::Left);

    _triangleCountLabel = UI::Label::create(context);
    _triangleCountLabel->setFont(AV::Font::familyMono);
    _triangleCountLabel->setTextHAlign(UI::TextHAlign::Left);

    _fpsLabel = UI::Label::create(context);
    _fpsLabel->setFont(AV::Font::familyMono);
    _fpsLabel->setTextHAlign(UI::TextHAlign::Left);

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setBackgroundRole(UI::ColorRole::Background);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->addChild(_bboxLabel);
    _formLayout->addChild(_primitivesCountLabel);
    _formLayout->addChild(_triangleCountLabel);
    _formLayout->addChild(_fpsLabel);
    addChild(_formLayout);

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

void InfoTool::setTriangleCount(size_t value)
{
    _triangleCount = value;
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
    _formLayout->setText(_bboxLabel, _getText(DJV_TEXT("Bounding box")) + ":");
    {
        std::stringstream ss;
        ss << _bbox;
        _bboxLabel->setText(ss.str());
    }
    _formLayout->setText(_primitivesCountLabel, _getText(DJV_TEXT("Primitives")) + ":");
    {
        std::stringstream ss;
        ss << _primitivesCount;
        _primitivesCountLabel->setText(ss.str());
    }
    _formLayout->setText(_triangleCountLabel, _getText(DJV_TEXT("Triangles")) + ":");
    {
        std::stringstream ss;
        ss << _triangleCount;
        _triangleCountLabel->setText(ss.str());
    }
    _formLayout->setText(_fpsLabel, _getText(DJV_TEXT("FPS")) + ":");
    {
        std::stringstream ss;
        ss.precision(2);
        ss << std::fixed << _fps;
        _fpsLabel->setText(ss.str());
    }
}

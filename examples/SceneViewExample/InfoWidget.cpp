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

#include "InfoWidget.h"

using namespace djv;

void InfoWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    ISettingsWidget::_init(context);

    _sceneSizeTextBlock = UI::TextBlock::create(context);
    _sceneSizeTextBlock->setFontFamily(AV::Font::familyMono);
    _sceneSizeTextBlock->setTextSizeRole(UI::MetricsRole::Slider);
    _sceneSizeTextBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _sceneXRangeTextBlock = UI::TextBlock::create(context);
    _sceneXRangeTextBlock->setFontFamily(AV::Font::familyMono);
    _sceneXRangeTextBlock->setTextSizeRole(UI::MetricsRole::Slider);
    _sceneXRangeTextBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _sceneYRangeTextBlock = UI::TextBlock::create(context);
    _sceneYRangeTextBlock->setFontFamily(AV::Font::familyMono);
    _sceneYRangeTextBlock->setTextSizeRole(UI::MetricsRole::Slider);
    _sceneYRangeTextBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _sceneZRangeTextBlock = UI::TextBlock::create(context);
    _sceneZRangeTextBlock->setFontFamily(AV::Font::familyMono);
    _sceneZRangeTextBlock->setTextSizeRole(UI::MetricsRole::Slider);
    _sceneZRangeTextBlock->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _primitivesCountLabel = UI::Label::create(context);
    _primitivesCountLabel->setFont(AV::Font::familyMono);
    _primitivesCountLabel->setTextHAlign(UI::TextHAlign::Left);
    _primitivesCountLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _pointCountLabel = UI::Label::create(context);
    _pointCountLabel->setFont(AV::Font::familyMono);
    _pointCountLabel->setTextHAlign(UI::TextHAlign::Left);
    _pointCountLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _fpsLabel = UI::Label::create(context);
    _fpsLabel->setFont(AV::Font::familyMono);
    _fpsLabel->setTextHAlign(UI::TextHAlign::Left);
    _fpsLabel->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));

    _formLayout = UI::FormLayout::create(context);
    _formLayout->setMargin(UI::Layout::Margin(UI::MetricsRole::MarginSmall));
    _formLayout->setSpacing(UI::Layout::Spacing(UI::MetricsRole::None));
    _formLayout->addChild(_sceneSizeTextBlock);
    _formLayout->addChild(_sceneXRangeTextBlock);
    _formLayout->addChild(_sceneYRangeTextBlock);
    _formLayout->addChild(_sceneZRangeTextBlock);
    _formLayout->addChild(_primitivesCountLabel);
    _formLayout->addChild(_pointCountLabel);
    _formLayout->addChild(_fpsLabel);
    addChild(_formLayout);

    auto weak = std::weak_ptr<InfoWidget>(std::dynamic_pointer_cast<InfoWidget>(shared_from_this()));
}

InfoWidget::InfoWidget()
{}

InfoWidget::~InfoWidget()
{}

std::shared_ptr<InfoWidget> InfoWidget::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<InfoWidget>(new InfoWidget);
    out->_init(context);
    return out;
}

void InfoWidget::setBBox(const Core::BBox3f& value)
{
    _bbox = value;
    _textUpdate();
}

void InfoWidget::setPrimitivesCount(size_t value)
{
    _primitivesCount = value;
    _textUpdate();
}

void InfoWidget::setPointCount(size_t value)
{
    _pointCount = value;
    _textUpdate();
}

void InfoWidget::setFPS(float value)
{
    _fps = value;
    _textUpdate();
}

void InfoWidget::_initEvent(Core::Event::Init&)
{
    _textUpdate();
}

void InfoWidget::_textUpdate()
{
    setTitle(_getText(DJV_TEXT("Information")));
    _formLayout->setText(_sceneSizeTextBlock, _getText(DJV_TEXT("Scene size")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.w() << " x " << _bbox.h() << " x " << _bbox.d();
        _sceneSizeTextBlock->setText(ss.str());
    }
    _formLayout->setText(_sceneXRangeTextBlock, _getText(DJV_TEXT("Scene X range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.x << " - " << _bbox.max.x;
        _sceneXRangeTextBlock->setText(ss.str());
    }
    _formLayout->setText(_sceneYRangeTextBlock, _getText(DJV_TEXT("Scene Y range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.y << " - " << _bbox.max.y;
        _sceneYRangeTextBlock->setText(ss.str());
    }
    _formLayout->setText(_sceneZRangeTextBlock, _getText(DJV_TEXT("Scene Z range")) + ":");
    {
        std::stringstream ss;
        ss << _bbox.min.z << " - " << _bbox.max.z;
        _sceneZRangeTextBlock->setText(ss.str());
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

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include "InfoWidget.h"

using namespace djv;

void InfoWidget::_init(const std::shared_ptr<System::Context>& context)
{
    ISettingsWidget::_init(context);

    _labels["SceneSizeW"] = UI::Text::Label::create(context);
    _labels["SceneSizeW"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneSizeW"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneSizeW"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneSizeH"] = UI::Text::Label::create(context);
    _labels["SceneSizeH"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneSizeH"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneSizeH"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneSizeD"] = UI::Text::Label::create(context);
    _labels["SceneSizeD"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneSizeD"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneSizeD"]->setMargin(UI::MetricsRole::MarginSmall);
    _layouts["SceneSize"] = UI::FormLayout::create(context);
    _layouts["SceneSize"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["SceneSize"]->addChild(_labels["SceneSizeW"]);
    _layouts["SceneSize"]->addChild(_labels["SceneSizeH"]);
    _layouts["SceneSize"]->addChild(_labels["SceneSizeD"]);
    _groupBoxes["SceneSize"] = UI::GroupBox::create(context);
    _groupBoxes["SceneSize"]->addChild(_layouts["SceneSize"]);

    _labels["SceneRangeMinX"] = UI::Text::Label::create(context);
    _labels["SceneRangeMinX"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMinX"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMinX"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneRangeMaxX"] = UI::Text::Label::create(context);
    _labels["SceneRangeMaxX"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMaxX"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMaxX"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneRangeMinY"] = UI::Text::Label::create(context);
    _labels["SceneRangeMinY"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMinY"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMinY"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneRangeMaxY"] = UI::Text::Label::create(context);
    _labels["SceneRangeMaxY"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMaxY"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMaxY"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneRangeMinZ"] = UI::Text::Label::create(context);
    _labels["SceneRangeMinZ"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMinZ"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMinZ"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["SceneRangeMaxZ"] = UI::Text::Label::create(context);
    _labels["SceneRangeMaxZ"]->setFontFamily(Render2D::Font::familyMono);
    _labels["SceneRangeMaxZ"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["SceneRangeMaxZ"]->setMargin(UI::MetricsRole::MarginSmall);
    _layouts["SceneRange"] = UI::FormLayout::create(context);
    _layouts["SceneRange"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMinX"]);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMaxX"]);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMinY"]);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMaxY"]);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMinZ"]);
    _layouts["SceneRange"]->addChild(_labels["SceneRangeMaxZ"]);
    _groupBoxes["SceneRange"] = UI::GroupBox::create(context);
    _groupBoxes["SceneRange"]->addChild(_layouts["SceneRange"]);

    _labels["Primitives"] = UI::Text::Label::create(context);
    _labels["Primitives"]->setFontFamily(Render2D::Font::familyMono);
    _labels["Primitives"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["Primitives"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["PointCount"] = UI::Text::Label::create(context);
    _labels["PointCount"]->setFontFamily(Render2D::Font::familyMono);
    _labels["PointCount"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["PointCount"]->setMargin(UI::MetricsRole::MarginSmall);
    _labels["FPS"] = UI::Text::Label::create(context);
    _labels["FPS"]->setFontFamily(Render2D::Font::familyMono);
    _labels["FPS"]->setTextHAlign(UI::TextHAlign::Left);
    _labels["FPS"]->setMargin(UI::MetricsRole::MarginSmall);
    _layouts["Stats"] = UI::FormLayout::create(context);
    _layouts["Stats"]->setSpacing(UI::MetricsRole::SpacingSmall);
    _layouts["Stats"]->addChild(_labels["Primitives"]);
    _layouts["Stats"]->addChild(_labels["PointCount"]);
    _layouts["Stats"]->addChild(_labels["FPS"]);
    _groupBoxes["Stats"] = UI::GroupBox::create(context);
    _groupBoxes["Stats"]->addChild(_layouts["Stats"]);

    auto layout = UI::VerticalLayout::create(context);
    layout->setMargin(UI::MetricsRole::Margin);
    layout->addChild(_groupBoxes["SceneSize"]);
    layout->addChild(_groupBoxes["SceneRange"]);
    layout->addChild(_groupBoxes["Stats"]);
    addChild(layout);

    auto weak = std::weak_ptr<InfoWidget>(std::dynamic_pointer_cast<InfoWidget>(shared_from_this()));
}

InfoWidget::InfoWidget()
{}

InfoWidget::~InfoWidget()
{}

std::shared_ptr<InfoWidget> InfoWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<InfoWidget>(new InfoWidget);
    out->_init(context);
    return out;
}

void InfoWidget::setBBox(const Math::BBox3f& value)
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

void InfoWidget::setLabelSizeGroup(const std::weak_ptr<djv::UI::Text::LabelSizeGroup>& value)
{
    for (auto i : _layouts)
    {
        i.second->setLabelSizeGroup(value);
    }
}

void InfoWidget::_initEvent(System::Event::Init&)
{
    _textUpdate();
}

void InfoWidget::_textUpdate()
{
    setTitle(_getText(DJV_TEXT("widget_info")));
    {
        std::stringstream ss;
        ss << _bbox.w();
        _labels["SceneSizeW"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.h();
        _labels["SceneSizeH"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.d();
        _labels["SceneSizeD"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.min.x;
        _labels["SceneRangeMinX"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.max.x;
        _labels["SceneRangeMaxX"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.min.y;
        _labels["SceneRangeMinY"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.max.y;
        _labels["SceneRangeMaxY"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.min.z;
        _labels["SceneRangeMinZ"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _bbox.max.z;
        _labels["SceneRangeMaxZ"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _primitivesCount;
        _labels["Primitives"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _pointCount;
        _labels["PointCount"]->setText(ss.str());
    }
    {
        std::stringstream ss;
        ss << _fps;
        _labels["FPS"]->setText(ss.str());
    }

    _layouts["SceneSize"]->setText(_labels["SceneSizeW"], _getText(DJV_TEXT("widget_info_width")) + ":");
    _layouts["SceneSize"]->setText(_labels["SceneSizeH"], _getText(DJV_TEXT("widget_info_height")) + ":");
    _layouts["SceneSize"]->setText(_labels["SceneSizeD"], _getText(DJV_TEXT("widget_info_depth")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMinX"], _getText(DJV_TEXT("widget_info_min_x")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMaxX"], _getText(DJV_TEXT("widget_info_max_x")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMinY"], _getText(DJV_TEXT("widget_info_min_y")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMaxY"], _getText(DJV_TEXT("widget_info_max_y")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMinZ"], _getText(DJV_TEXT("widget_info_min_z")) + ":");
    _layouts["SceneRange"]->setText(_labels["SceneRangeMaxZ"], _getText(DJV_TEXT("widget_info_max_z")) + ":");
    _layouts["Stats"]->setText(_labels["Primitives"], _getText(DJV_TEXT("widget_info_primitives")) + ":");
    _layouts["Stats"]->setText(_labels["PointCount"], _getText(DJV_TEXT("widget_info_point_count")) + ":");
    _layouts["Stats"]->setText(_labels["FPS"], _getText(DJV_TEXT("widget_info_fps")) + ":");

    _groupBoxes["SceneSize"]->setText(_getText(DJV_TEXT("widget_info_scene_size")));
    _groupBoxes["SceneRange"]->setText(_getText(DJV_TEXT("widget_info_scene_range")));
    _groupBoxes["Stats"]->setText(_getText(DJV_TEXT("widget_info_stats")));
}

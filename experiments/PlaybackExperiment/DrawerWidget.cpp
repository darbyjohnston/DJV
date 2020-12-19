// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "DrawerWidget.h"

#include "Media.h"

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>

using namespace djv;

struct DrawerWidget::Private
{
    std::shared_ptr<UIComponents::LineGraphWidget> videoQueueGraphWidget;
    std::shared_ptr<UIComponents::LineGraphWidget> audioQueueGraphWidget;
    std::shared_ptr<UI::VerticalLayout> layout;

    std::shared_ptr<Core::Observer::Value<size_t> > videoQueueSizeObserver;
    std::shared_ptr<Core::Observer::Value<size_t> > audioQueueSizeObserver;
};

void DrawerWidget::_init(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);
    DJV_PRIVATE_PTR();

    p.videoQueueGraphWidget = UIComponents::LineGraphWidget::create(context);
    p.audioQueueGraphWidget = UIComponents::LineGraphWidget::create(context);

    p.layout = UI::VerticalLayout::create(context);
    p.layout->setMargin(UI::MetricsRole::MarginSmall);
    auto label = UI::Text::Label::create(context);
    label->setText("Video Queue");
    label->setTextHAlign(UI::TextHAlign::Left);
    p.layout->addChild(label);
    p.layout->addChild(p.videoQueueGraphWidget);
    label = UI::Text::Label::create(context);
    label->setText("Audio Queue");
    label->setTextHAlign(UI::TextHAlign::Left);
    p.layout->addChild(label);
    p.layout->addChild(p.audioQueueGraphWidget);
    addChild(p.layout);

    auto weak = std::weak_ptr<DrawerWidget>(std::dynamic_pointer_cast<DrawerWidget>(shared_from_this()));
    p.videoQueueSizeObserver = Core::Observer::Value<size_t>::create(
        media->observeVideoQueueSize(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                widget->_p->videoQueueGraphWidget->addSample(value);
            }
        });

    p.audioQueueSizeObserver = Core::Observer::Value<size_t>::create(
        media->observeAudioQueueSize(),
        [weak](size_t value)
        {
            if (auto widget = weak.lock())
            {
                std::stringstream ss;
                ss << value;
                widget->_p->audioQueueGraphWidget->addSample(value);
            }
        });
}

DrawerWidget::DrawerWidget() :
    _p(new Private)
{}

DrawerWidget::~DrawerWidget()
{}

std::shared_ptr<DrawerWidget> DrawerWidget::create(
    const std::shared_ptr<Media>& media,
    const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<DrawerWidget>(new DrawerWidget);
    out->_init(media, context);
    return out;
}

void DrawerWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    _setMinimumSize(_p->layout->getMinimumSize());
}

void DrawerWidget::_layoutEvent(System::Event::Layout&)
{
    _p->layout->setGeometry(getGeometry());
}


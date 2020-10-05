// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "PlaybackWidget.h"

#include <djvUI/Style.h>
#include <djvUI/ToolButton.h>

#include <djvRender2D/Render.h>

using namespace djv;

void PlaybackWidget::_init(const std::shared_ptr<System::Context>& context)
{
    Widget::_init(context);

    auto reverseButton = UI::ToolButton::create(context);
    reverseButton->setIcon("djvIconPlaybackReverse");
    reverseButton->setCheckedIcon("djvIconPlaybackStop");
    auto forwardButton = UI::ToolButton::create(context);
    forwardButton->setIcon("djvIconPlaybackForward");
    forwardButton->setCheckedIcon("djvIconPlaybackStop");

    _buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
    _buttonGroup->setButtons({ reverseButton, forwardButton });

    _layout = UI::HorizontalLayout::create(context);
    _layout->setSpacing(UI::MetricsRole::None);
    _layout->addChild(reverseButton);
    _layout->addChild(forwardButton);
    addChild(_layout);

    auto weak = std::weak_ptr<PlaybackWidget>(std::dynamic_pointer_cast<PlaybackWidget>(shared_from_this()));
    _buttonGroup->setExclusiveCallback(
        [weak](int value)
        {
            if (auto widget = weak.lock())
            {
                Playback playback = Playback::Stop;
                switch (value)
                {
                case 0: playback = Playback::Forward; break;
                case 1: playback = Playback::Reverse; break;
                }
                if (widget->_playbackCallback)
                {
                    widget->_playbackCallback(playback);
                }
            }
        });
}

PlaybackWidget::PlaybackWidget()
{}

PlaybackWidget::~PlaybackWidget()
{}

std::shared_ptr<PlaybackWidget> PlaybackWidget::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<PlaybackWidget>(new PlaybackWidget);
    out->_init(context);
    return out;
}

void PlaybackWidget::setPlayback(Playback value)
{
    int index = -1;
    switch (value)
    {
    case Playback::Forward: index = 0; break;
    case Playback::Reverse: index = 1; break;
    default: break;
    }
    _buttonGroup->setChecked(index);
}

void PlaybackWidget::setPlaybackCallback(const std::function<void(Playback)>& value)
{
    _playbackCallback = value;
}

void PlaybackWidget::_preLayoutEvent(System::Event::PreLayout&)
{
    _setMinimumSize(_layout->getMinimumSize());
}

void PlaybackWidget::_layoutEvent(System::Event::Layout&)
{
    _layout->setGeometry(getGeometry());
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"

#include <djvUI/ButtonGroup.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Widget.h>

class PlaybackWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(PlaybackWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    PlaybackWidget();

public:
    ~PlaybackWidget() override;

    static std::shared_ptr<PlaybackWidget> create(const std::shared_ptr<djv::System::Context>&);

    void setPlayback(Playback);

    void setPlaybackCallback(const std::function<void(Playback)>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _layoutEvent(djv::System::Event::Layout&) override;

private:
    std::shared_ptr<djv::Core::Observer::ValueSubject<Playback> > _playback;
    std::shared_ptr<djv::UI::ButtonGroup> _buttonGroup;
    std::shared_ptr<djv::UI::HorizontalLayout> _layout;
    std::function<void(Playback)> _playbackCallback;
};
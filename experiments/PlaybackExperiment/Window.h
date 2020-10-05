// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include "Enum.h"
#include "IO.h"

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/Label.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvImage/Image.h>

class ImageWidget;
class Media;
class PlaybackWidget;
class TimelineWidget;

class Window : public djv::UI::Window
{
    DJV_NON_COPYABLE(Window);

protected:
    void _init(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);
    Window();

public:
    ~Window() override;

    static std::shared_ptr<Window> create(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);

private:
    void _widgetUpdate();

    std::shared_ptr<Media> _media;
    IOInfo _info;
    double _currentTime = 0.0;

    std::shared_ptr<ImageWidget> _imageWidget;
    std::shared_ptr<PlaybackWidget> _playbackWidget;
    std::shared_ptr<djv::UI::Text::Label> _currentTimeLabel;
    std::shared_ptr<TimelineWidget> _timelineWidget;
    std::shared_ptr<djv::UIComponents::LineGraphWidget> _videoQueueGraphWidget;
    std::shared_ptr<djv::UIComponents::LineGraphWidget> _audioQueueGraphWidget;
    std::shared_ptr<djv::UI::VerticalLayout> _layout;

    std::shared_ptr<djv::Core::Observer::Value<IOInfo> > _infoObserver;
    std::shared_ptr<djv::Core::Observer::Value<std::shared_ptr<djv::Image::Image> > > _imageObserver;
    std::shared_ptr<djv::Core::Observer::Value<size_t> > _videoQueueSizeObserver;
    std::shared_ptr<djv::Core::Observer::Value<size_t> > _audioQueueSizeObserver;
    std::shared_ptr<djv::Core::Observer::Value<Playback> > _playbackObserver;
    std::shared_ptr<djv::Core::Observer::Value<double> > _currentTimeObserver;
};

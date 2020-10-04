// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

#include <djvUI/RowLayout.h>

#include <djvImage/Image.h>

class ImageWidget;
class Media;
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
    std::shared_ptr<Media> _media;
    std::shared_ptr<ImageWidget> _imageWidget;
    std::shared_ptr<TimelineWidget> _timelineWidget;
    std::shared_ptr<djv::UI::VerticalLayout> _layout;
    std::shared_ptr<djv::Core::Observer::Value<std::shared_ptr<djv::Image::Image> > > _imageObserver;
};

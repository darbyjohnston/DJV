// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Widget.h>

#include <djvImage/Image.h>

class ImageWidget : public djv::UI::Widget
{
    DJV_NON_COPYABLE(ImageWidget);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    ImageWidget();

public:
    ~ImageWidget() override;

    static std::shared_ptr<ImageWidget> create(const std::shared_ptr<djv::System::Context>&);

    void setImage(const std::shared_ptr<djv::Image::Image>&);

protected:
    void _preLayoutEvent(djv::System::Event::PreLayout&) override;
    void _paintEvent(djv::System::Event::Paint&) override;

private:
    DJV_PRIVATE();
};
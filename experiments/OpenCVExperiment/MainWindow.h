// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

#include <djvImage/Image.h>

class MainWindow : public djv::UI::Window
{
    DJV_NON_COPYABLE(MainWindow);

protected:
    void _init(const std::shared_ptr<djv::System::Context>&);
    MainWindow();

public:
    ~MainWindow() override;

    static std::shared_ptr<MainWindow> create(const std::shared_ptr<djv::System::Context>&);

    void setImage(const std::shared_ptr<djv::Image::Image>&);

protected:
    void _initEvent(djv::System::Event::Init&);

private:
    void _widgetUpdate();

    DJV_PRIVATE();
};

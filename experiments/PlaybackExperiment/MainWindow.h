// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvUI/Window.h>

class Media;

class MainWindow : public djv::UI::Window
{
    DJV_NON_COPYABLE(MainWindow);

protected:
    void _init(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);
    MainWindow();

public:
    ~MainWindow() override;

    static std::shared_ptr<MainWindow> create(
        const std::shared_ptr<Media>&,
        const std::shared_ptr<djv::System::Context>&);

protected:
    void _initEvent(djv::System::Event::Init&);

private:
    void _widgetUpdate();

    DJV_PRIVATE();
};

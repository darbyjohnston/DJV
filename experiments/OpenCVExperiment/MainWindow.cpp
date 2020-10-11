// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "MainWindow.h"

#include "ImageWidget.h"

#include <djvImage/Image.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

struct MainWindow::Private
{
    std::shared_ptr<ImageWidget> imageWidget;
};

void MainWindow::_init(const std::shared_ptr<System::Context>& context)
{
    UI::Window::_init(context);
    DJV_PRIVATE_PTR();

    p.imageWidget = ImageWidget::create(context);

    addChild(p.imageWidget);

    _widgetUpdate();
}

MainWindow::MainWindow() :
    _p(new Private)
{}

MainWindow::~MainWindow()
{}

std::shared_ptr<MainWindow> MainWindow::create(const std::shared_ptr<System::Context>& context)
{
    auto out = std::shared_ptr<MainWindow>(new MainWindow);
    out->_init(context);
    return out;
}

void MainWindow::setImage(const std::shared_ptr<djv::Image::Image>& value)
{
    _p->imageWidget->setImage(value);
}

void MainWindow::_initEvent(System::Event::Init& event)
{
    DJV_PRIVATE_PTR();
    if (event.getData().text)
    {
    }
}

void MainWindow::_widgetUpdate()
{
    DJV_PRIVATE_PTR();
}

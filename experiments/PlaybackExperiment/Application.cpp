// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Application.h"

#include "IO.h"
#include "Media.h"
#include "Window.h"

using namespace djv;

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);

    IOSystem::create(shared_from_this());

    if (args.size() > 0)
    {
        _media = Media::create(args.front(), shared_from_this());
    }
}

Application::Application()
{}

Application::~Application()
{}

std::shared_ptr<Application> Application::create(std::list<std::string>& args)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(args);
    return out;
}

void Application::run()
{
    _window = Window::create(_media, shared_from_this());
    _window->show();

    Desktop::Application::run();
}
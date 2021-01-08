// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Application.h"

#include "IO.h"
#include "MainWindow.h"
#include "Media.h"

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvAV/TimeFunc.h>

using namespace djv;

struct Application::Private
{
    std::shared_ptr<MainWindow> mainWindow;
    std::shared_ptr<Media> media;
};

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);
    DJV_PRIVATE_PTR();

    UIComponents::UIComponentsSystem::create(shared_from_this());
    IO::IOSystem::create(shared_from_this());

    //args.push_back("/Users/darby/Desktop/good_over_evil-ras_natural.ogg");
    //args.push_back("/Users/darby/Desktop/Albany_2012-10-10.mov");
    //args.push_back("/Users/darby/Desktop/MOE DOC V8 FINAL .mp4");
    args.push_back("/Users/darby/Desktop/MVI_2791.m4v");
    //args.push_back("/Users/darby/Desktop/Until the End of the World Disc 1.m4v");
    //args.push_back("/Users/darby/Desktop/Chimera_DCI4k2398p_HDR_P3PQ.mp4");
    //args.push_back("/Users/darby/Desktop/Black Marble - It's Immaterial - 10 Portland U.mp3");
    if (args.size() > 0)
    {
        p.media = Media::create(args.front(), shared_from_this());
    }
}

Application::Application() :
    _p(new Private)
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
    DJV_PRIVATE_PTR();

    p.mainWindow = MainWindow::create(p.media, shared_from_this());
    p.mainWindow->show();

    Desktop::Application::run();
}

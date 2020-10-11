// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include "Application.h"

#include "MainWindow.h"

#include <djvUIComponents/UIComponentsSystem.h>

#include <djvAV/IOSystem.h>

#include <djvSystem/LogSystem.h>
#include <djvSystem/TimerFunc.h>

#include <opencv2/opencv.hpp>

using namespace djv;

struct Application::Private
{
    std::shared_ptr<MainWindow> mainWindow;
    std::shared_ptr<AV::IO::IRead> read;
    std::shared_ptr<System::Timer> timer;
};

namespace
{
    int toCV(Image::Type value)
    {
        const std::array<int, static_cast<size_t>(Image::Type::Count)> data =
        {
            -1,
            CV_8UC1, CV_16UC1, -1, CV_16FC1, CV_32FC1,
            CV_8UC2, CV_16UC2, -1, CV_16FC2, CV_32FC2,
            CV_8UC3, -1, CV_16UC3, -1, CV_16FC3, CV_32FC3,
            CV_8UC4, -CV_16UC4, -1, CV_16FC4, CV_32FC4
        };
        return data[static_cast<size_t>(value)];
    }

} // namespace

void Application::_init(std::list<std::string>& args)
{
    Desktop::Application::_init(args);
    DJV_PRIVATE_PTR();

    UIComponents::UIComponentsSystem::create(shared_from_this());

    if (args.size() > 0)
    {
        try
        {
            auto io = getSystemT<AV::IO::IOSystem>();
            p.read = io->read(System::File::Info(args.front()));
        }
        catch (const std::exception& e)
        {
            auto logSystem = getSystemT<System::LogSystem>();
            logSystem->log("Application", e.what(), System::LogLevel::Error);
            exit(1);
        }
        if (p.read)
        {
            p.timer = System::Timer::create(shared_from_this());
            p.timer->setRepeating(true);
            auto weak = std::weak_ptr<Application>(std::dynamic_pointer_cast<Application>(shared_from_this()));
            p.timer->start(
                System::getTimerDuration(System::TimerValue::Fast),
                [weak](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
                {
                    if (auto app = weak.lock())
                    {
                        std::shared_ptr<Image::Image> image;
                        {
                            std::unique_lock<std::mutex> lock(app->_p->read->getMutex());
                            auto& queue = app->_p->read->getVideoQueue();
                            if (!queue.isEmpty())
                            {
                                image = queue.popFrame().image;
                            }
                        }
                        if (image)
                        {
                            const auto& info = image->getInfo();
                            const int cvType = toCV(info.type);
                            if (cvType != -1)
                            {
                                auto image2 = Image::Image::create(Image::Info(info.size.w, info.size.h, info.type));

                                cv::Mat cvImage(info.size.w, info.size.h, cvType, image->getData());
                                cv::Mat cvImage2(info.size.w, info.size.h, cvType, image2->getData());
                                //cvImage.convertTo(cvImage2, -1, 1, 90);
                                cv::threshold(cvImage, cvImage2, 128, 255, cv::THRESH_BINARY);

                                app->_p->mainWindow->setImage(image2);
                                app->_p->read.reset();
                                app->_p->timer->stop();
                            }
                        }
                    }
                });
        }
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

    p.mainWindow = MainWindow::create(shared_from_this());
    p.mainWindow->show();

    Desktop::Application::run();
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvCore/Animation.h>

#include <djvCore/Error.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        // Create an application.
        auto args = CmdLine::Application::args(argc, argv);
        auto app = CmdLine::Application::create(args);

        // Create an animation.
        auto animation = Core::Animation::Animation::create(app);
        animation->setType(Core::Animation::Type::Sine);
        animation->setRepeating(true);
        animation->start(
            0.f,
            1.f,
            std::chrono::milliseconds(1 * 1000),
            [](float value)
            {
                std::string buf;
                for (size_t i = 0; i < static_cast<size_t>(value * 40.f); ++i)
                {
                    buf += "#";
                }
                std::cout << buf << std::endl;
            },
            [app](float value)
            {
                app->exit(0);
            });

        // Run the application.
        app->run();
        r = app->getExitCode();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}

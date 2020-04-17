// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/Application.h>

#include <djvCore/Error.h>

#if defined(DJV_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif // DJV_WINDOWS

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = ViewApp::Application::args(argc, argv);
        auto app = ViewApp::Application::create(args);
        if (0 == app->getExitCode())
        {
            app->run();
        }
        r = app->getExitCode();
    }
    catch (const std::exception & error)
    {
        std::cout << Core::Error::format(error) << std::endl;
    }
    return r;
}

#if defined(DJV_PLATFORM_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif // DJV_WINDOWS

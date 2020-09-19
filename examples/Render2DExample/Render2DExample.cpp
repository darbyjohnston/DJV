// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvRender2D/Render.h>

#include <djvGL/GLFWSystem.h>
#include <djvGL/GL.h>

#include <djvImage/Color.h>

#include <djvSystem/Timer.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/RandomFunc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

const size_t circleCount = 10000;

struct Circle
{
    Circle(const glm::vec2 & area)
    {
        pos.x = Core::Random::getRandom(0.f, area.x);
        pos.y = Core::Random::getRandom(0.f, area.x);
        color = Image::Color(
            Core::Random::getRandom(0.f, 1.f),
            Core::Random::getRandom(0.f, 1.f),
            Core::Random::getRandom(0.f, 1.f),
            1.f);
        radiusRate = Core::Random::getRandom(.1f, 10.f);
        alphaRate = Core::Random::getRandom(.001f, .01f);
    }

    glm::vec2           pos;
    float               radius      = 0.f;
    float               radiusRate;
    Image::Color    color;
    float               alphaRate;
};

class Application : public CmdLine::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(std::list<std::string>&);

    Application();

public:
    static std::shared_ptr<Application> create(std::list<std::string>&);

    void run() override;

private:
    void _render();

    Image::Size _windowSize = Image::Size(1280, 720);
    std::vector<Circle> _circles;
    std::shared_ptr<System::Timer> _timer;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    _timer = System::Timer::create(shared_from_this());
    _timer->setRepeating(true);
    _timer->start(
        std::chrono::milliseconds(10),
        [this](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
    {
        while (_circles.size() < circleCount)
        {
            _circles.push_back(Circle(glm::vec2(_windowSize.w, _windowSize.h)));
        }
        auto i = _circles.begin();
        while (i != _circles.end())
        {
            i->radius += i->radiusRate;
            const float a = i->color.getF32(3) - i->alphaRate;
            if (a <= 0.f)
            {
                i = _circles.erase(i);
            }
            else
            {
                i->color.setF32(a, 3);
                ++i;
            }
        }
    });

    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getGLFWWindow();
    glfwSetWindowSize(glfwWindow, _windowSize.w, _windowSize.h);
    glfwShowWindow(glfwWindow);
}

Application::Application()
{}

std::shared_ptr<Application> Application::create(std::list<std::string>& args)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(args);
    return out;
}

void Application::run()
{
    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getGLFWWindow();
    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
        tick();
        _render();
        glfwSwapBuffers(glfwWindow);
    }
}

void Application::_render()
{
    if (auto render = getSystemT<Render2D::Render>())
    {
        auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getGLFWWindow();
        glm::ivec2 windowSize = glm::ivec2(0, 0);
        glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
        _windowSize.w = windowSize.x;
        _windowSize.h = windowSize.y;
        render->beginFrame(_windowSize);
        for (const auto & i : _circles)
        {
            render->setFillColor(i.color);
            //render->drawCircle(i.pos, i.radius);
            render->drawRect(Math::BBox2f(i.pos.x, i.pos.y, i.radius, i.radius));
        }
        render->endFrame();
    }
}

int main(int argc, char ** argv)
{
    int r = 1;
    try
    {
        auto args = Application::args(argc, argv);
        auto app = Application::create(args);
        app->run();
        r = app->getExitCode();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}

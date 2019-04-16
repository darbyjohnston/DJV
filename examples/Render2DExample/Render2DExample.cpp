//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvCmdLineApp/Application.h>
#include <djvCmdLineApp/GLFWSystem.h>

#include <djvAV/Color.h>
#include <djvAV/Render2D.h>

#include <djvCore/Error.h>
#include <djvCore/Timer.h>

#include <GLFW/glfw3.h>

using namespace djv;

const size_t circleCount = 10000;

struct Circle
{
    Circle(const glm::vec2 & area)
    {
        pos.x = Core::Math::getRandom(0.f, area.x);
        pos.y = Core::Math::getRandom(0.f, area.x);
        color = AV::Image::Color(
            Core::Math::getRandom(0.f, 1.f),
            Core::Math::getRandom(0.f, 1.f),
            Core::Math::getRandom(0.f, 1.f),
            1.f);
        radiusRate = Core::Math::getRandom(.1f, 1.f);
        alphaRate = Core::Math::getRandom(.001f, .01f);
    }
    glm::vec2 pos;
    float radius = 0.f;
    float radiusRate;
    AV::Image::Color color;
    float alphaRate;
};

class Application : public CmdLine::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(int argc, char ** argv);

    Application();

public:
    static std::unique_ptr<Application> create(int argc, char ** argv);

    int run();

private:
    void _render();

    glm::ivec2 _windowSize = glm::ivec2(1280, 720);
    std::vector<Circle> _circles;
    std::shared_ptr<Core::Time::Timer> _timer;
};

void Application::_init(int argc, char ** argv)
{
    CmdLine::Application::_init(argc, argv);

    _timer = Core::Time::Timer::create(this);
    _timer->setRepeating(true);
    _timer->start(
        std::chrono::milliseconds(10),
        [this](float)
    {
        while (_circles.size() < circleCount)
        {
            _circles.push_back(Circle(_windowSize));
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

    auto glfwWindow = getSystemT<CmdLine::GLFWSystem>()->getGLFWWindow();
    glfwSetWindowSize(glfwWindow, _windowSize.x, _windowSize.y);
    glfwShowWindow(glfwWindow);
}

Application::Application()
{}

std::unique_ptr<Application> Application::create(int argc, char ** argv)
{
    auto out = std::unique_ptr<Application>(new Application);
    out->_init(argc, argv);
    return out;
}

int Application::run()
{
    auto time = std::chrono::system_clock::now();
    auto glfwWindow = getSystemT<CmdLine::GLFWSystem>()->getGLFWWindow();
    while (!glfwWindowShouldClose(glfwWindow))
    {
        auto now = std::chrono::system_clock::now();
        const std::chrono::duration<float> delta = now - time;
        time = now;
        const float dt = delta.count();
        //std::cout << "FPS: " << (dt > 0.f ? 1.f / dt : 0.f) << std::endl;

        glfwPollEvents();
        tick(dt);
        _render();
        //glfwSwapBuffers(glfwWindow);
        gl::glFlush();
    }
    return 0;
}

void Application::_render()
{
    if (auto render = getSystemT<AV::Render::Render2D>())
    {
        auto glfwWindow = getSystemT<CmdLine::GLFWSystem>()->getGLFWWindow();
        glfwGetWindowSize(glfwWindow, &_windowSize.x, &_windowSize.y);
        render->beginFrame(_windowSize);
        for (const auto & i : _circles)
        {
            render->setFillColor(i.color);
            //render->drawCircle(i.pos, i.radius);
            render->drawRect(Core::BBox2f(i.pos.x, i.pos.y, i.radius, i.radius));
        }
        render->endFrame();
    }
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        return Application::create(argc, argv)->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvRender2D/Render.h>
#include <djvRender2D/RenderSystem.h>

#include <djvGL/GLFWSystem.h>
#include <djvGL/GL.h>

#include <djvImage/Data.h>

#include <djvCore/Error.h>
#include <djvCore/Random.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

using namespace djv;

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
    int _wrapX(int) const;
    int _wrapY(int) const;
    uint8_t _getNeighborCount(const uint8_t*, int, int) const;
    void _calc();
    void _render();

    Image::Size _windowSize = Image::Size(1280, 720);
    Image::Size _bufferSize = Image::Size(128, 72);
    std::vector<std::shared_ptr<Image::Data> > _buffers;
    size_t _bufferA = 0;
    size_t _bufferB = 1;
    bool _randomize = true;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    Render2D::RenderSystem::create(shared_from_this());

    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
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
    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
        tick();
        _calc();
        _render();
        glfwSwapBuffers(glfwWindow);

        size_t tmp = _bufferA;
        _bufferA = _bufferB;
        _bufferB = tmp;
    }
}

int Application::_wrapX(int value) const
{
    while (value >= _bufferSize.w)
    {
        value -= _bufferSize.w;
    }
    while (value < 0)
    {
        value += _bufferSize.w;
    }
    return value;
}

int Application::_wrapY(int value) const
{
    while (value >= _bufferSize.h)
    {
        value -= _bufferSize.h;
    }
    while (value < 0)
    {
        value += _bufferSize.h;
    }
    return value;
}

uint8_t Application::_getNeighborCount(const uint8_t* p, int x, int y) const
{
    uint8_t out = 0;

    const size_t xOffset0 = _wrapX(x - 1);
    const size_t xOffset1 = _wrapX(x + 1);
    size_t yOffset = _wrapY(y - 1) * static_cast<size_t>(_bufferSize.w);
    if (p[yOffset + xOffset0]) ++out;
    if (p[yOffset + x]) ++out;
    if (p[yOffset + xOffset1]) ++out;

    yOffset = y * static_cast<size_t>(_bufferSize.w);
    if (p[yOffset + xOffset0]) ++out;
    if (out < 4 && p[yOffset + xOffset1]) ++out;

    yOffset = _wrapY(y + 1) * static_cast<size_t>(_bufferSize.w);
    if (out < 4 && p[yOffset + xOffset0]) ++out;
    if (out < 4 && p[yOffset + x]) ++out;
    if (out < 4 && p[yOffset + xOffset1]) ++out;

    return out;
}

void Application::_calc()
{
    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
    glm::ivec2 windowSize = glm::ivec2(0, 0);
    glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
    if (windowSize.x != _windowSize.w || windowSize.y != _windowSize.h)
    {
        _windowSize.w = windowSize.x;
        _windowSize.h = windowSize.y;
        _buffers.clear();
    }

    if (_buffers.empty())
    {
        _bufferSize.w = _windowSize.w / 3;
        _bufferSize.h = _windowSize.h / 3;
        _buffers.push_back(Image::Data::create(Image::Info(_bufferSize, Image::Type::L_U8)));
        _buffers.push_back(Image::Data::create(Image::Info(_bufferSize, Image::Type::L_U8)));
        _buffers[0]->zero();
        _buffers[1]->zero();
    }

    if (_randomize)
    {
        _randomize = false;
        uint8_t* b = _buffers[_bufferB]->getData();
        for (uint16_t y = 0; y < _bufferSize.h; ++y)
        {
            for (uint16_t x = 0; x < _bufferSize.w; ++x, ++b)
            {
                if (Core::Random::getRandom() > .5F)
                {
                    *b = 255;
                }
            }
        }
    }

    uint8_t* a = _buffers[_bufferA]->getData();
    const uint8_t* bData = _buffers[_bufferB]->getData();
    const uint8_t* b = bData;
    size_t changes = 0;
    const int neighborX[] =
    {
        -1, 0, 1,
        -1,    1,
        -1, 0, 1
    };
    const int neighborY[] =
    {
        -1, -1, -1,
         0,      0,
         1,  1,  1
    };
    for (int y = 0; y < _bufferSize.h; ++y)
    {
        for (int x = 0; x < _bufferSize.w; ++x, ++a, ++b)
        {
            uint8_t neighbors = 0;
            for (size_t i = 0; i < 8 && neighbors < 4; ++i)
            {
                int x2 = x + neighborX[i];
                if (x2 < 0) x2 = _bufferSize.w - 1;
                else if (x2 >= _bufferSize.w) x2 = 0;
                int y2 = y + neighborY[i];
                if (y2 < 0) y2 = _bufferSize.h - 1;
                else if (y2 >= _bufferSize.h) y2 = 0;
                if (*(bData + y2 * static_cast<size_t>(_bufferSize.w) + x2))
                {
                    ++neighbors;
                }
            }
            if (255 == *b && (2 == neighbors || 3 == neighbors))
            {
                *a = 255;
            }
            else if (*b < 255 && 3 == neighbors)
            {
                *a = 255;
                ++changes;
            }
            else if (*b > 0)
            {
                *a = 0;
                ++changes;
            }
            else
            {
                *a = 0;
            }
        }
    }
    _buffers[_bufferA]->newUID();

    // Randomize if the number of changes is less than 2%.
    if (changes < static_cast<int>(_bufferSize.w) * static_cast<int>(_bufferSize.h) * .02F)
    {
        _randomize = true;
    }
}

void Application::_render()
{
    if (auto render = getSystemT<Render2D::Render>())
    {
        render->beginFrame(_windowSize);
        render->pushTransform(glm::scale(glm::mat3x3(1.F), glm::vec2(
            _windowSize.w / static_cast<float>(_bufferSize.w),
            _windowSize.h / static_cast<float>(_bufferSize.h))));
        Render2D::ImageOptions options;
        options.cache = Render2D::ImageCache::Dynamic;
        render->drawImage(_buffers[_bufferA], glm::vec2(0.F, 0.F), options);
        render->popTransform();
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

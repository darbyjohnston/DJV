//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvAV/AVSystem.h>
#include <djvAV/Application.h>
#include <djvAV/Color.h>
#include <djvAV/Render2D.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>

#include <GLFW/glfw3.h>

using namespace djv;

const size_t drawCount = 10000;
const size_t randomCount = 10000;
glm::ivec2 windowSize = glm::ivec2(0, 0);

struct RandomColor
{
    RandomColor() : c(
        Core::Math::getRandom(0.f, 1.f),
        Core::Math::getRandom(0.f, 1.f),
        Core::Math::getRandom(0.f, 1.f),
        Core::Math::getRandom(0.f, 1.f))
    {}
    AV::Image::Color c;
    RandomColor * next = nullptr;
};

struct RandomPos
{
    RandomPos() : v(
        floorf(Core::Math::getRandom(windowSize.x / -2.f, windowSize.x * 1.5f)),
        floorf(Core::Math::getRandom(windowSize.y / -2.f, windowSize.y * 1.5f)))
    {}
    glm::vec2 v;
    RandomPos * next = nullptr;
};

struct RandomSize
{
    RandomSize() : v(
        ceilf(Core::Math::getRandom(10.f, 500.f)),
        ceilf(Core::Math::getRandom(10.f, 500.f)))
    {}
    glm::vec2 v;
    RandomSize * next = nullptr;
};

struct RandomText
{
    RandomText() :
        s(Core::String::getRandomName()),
        size(sizes[Core::Math::getRandom(static_cast<int>(sizes.size()) - 1)])
    {}
    std::string s;
    static const std::vector<float> sizes;
    float size = 0.f;
    RandomText * next = nullptr;
};

const std::vector<float> RandomText::sizes = { 12.f, 24.f, 48.f, 96.f, 1000.f };

class Application : public AV::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(int argc, char ** argv);
    
    Application();

public:
    static std::unique_ptr<Application> create(int argc, char ** argv);

    int run();

private:
    void _generateRandomNumbers();
    void _initRandomNumbers();
    void _drawRandomRectangle();
    void _drawRandomRoundedRectangle();
    void _drawRandomCircle();
    void _drawRandomText();
    void _render();

    RandomColor * _randomColors = nullptr;
    RandomColor * _currentColor = nullptr;
    RandomPos *   _randomPos    = nullptr;
    RandomPos *   _currentPos   = nullptr;
    RandomSize *  _randomSizes  = nullptr;
    RandomSize *  _currentSize  = nullptr;
    RandomText *  _randomText   = nullptr;
    RandomText *  _currentText  = nullptr;
};

void Application::_init(int argc, char ** argv)
{
    AV::Application::_init(argc, argv);
    auto glfwWindow = getGLFWWindow();
    glfwSetWindowSize(glfwWindow, 1280, 720);
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
    auto glfwWindow = getGLFWWindow();
    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();
        _render();
        glfwSwapBuffers(glfwWindow);
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> delta = now - time;
        time = now;
        const float dt = delta.count();
        std::cout << "FPS: " << (dt > 0.f ? 1.f / dt : 0.f) << std::endl;
    }
    return 0;
}

void Application::_generateRandomNumbers()
{
    _randomColors = new RandomColor;
    _randomPos    = new RandomPos;
    _randomSizes  = new RandomSize;
    _randomText   = new RandomText;
    auto newColor = _randomColors;
    auto newPos   = _randomPos;
    auto newSize  = _randomSizes;
    auto newText  = _randomText;
    for (size_t i = 0; i < randomCount; ++i)
    {
        newColor->next = new RandomColor;
        newColor       = newColor->next;
        newPos->next   = new RandomPos;
        newPos         = newPos->next;
        newSize->next  = new RandomSize;
        newSize        = newSize->next;
        newText->next  = new RandomText;
        newText        = newText->next;
    }
    newColor->next = _randomColors;
    newPos->next   = _randomPos;
    newSize->next  = _randomSizes;
    newText->next  = _randomText;
}

void Application::_initRandomNumbers()
{
    _currentColor = _randomColors;
    _currentPos   = _randomPos;
    _currentSize  = _randomSizes;
    _currentText  = _randomText;
    int random = Core::Math::getRandom(static_cast<int>(randomCount));
    for (int i = 0; i < random; ++i)
    {
        _currentColor = _currentColor->next;
    }
    random = Core::Math::getRandom(static_cast<int>(randomCount));
    for (int i = 0; i < random; ++i)
    {
        _currentPos = _currentPos->next;
    }
    random = Core::Math::getRandom(static_cast<int>(randomCount));
    for (int i = 0; i < random; ++i)
    {
        _currentSize = _currentSize->next;
    }
    random = Core::Math::getRandom(static_cast<int>(randomCount));
    for (int i = 0; i < random; ++i)
    {
        _currentText = _currentText->next;
    }
}

void Application::_drawRandomRectangle()
{
    if (auto render = getSystemT<AV::Render::Render2D>().lock())
    {
        render->setFillColor(_currentColor->c);
        render->drawRect(Core::BBox2f(_currentPos->v.x, _currentPos->v.y, _currentSize->v.x, _currentSize->v.y));
        _currentColor = _currentColor->next;
        _currentPos = _currentPos->next;
        _currentSize = _currentSize->next;
    }
}

void Application::_drawRandomRoundedRectangle()
{
    if (auto render = getSystemT<AV::Render::Render2D>().lock())
    {
        render->setFillColor(_currentColor->c);
        render->drawRoundedRect(
            Core::BBox2f(_currentPos->v.x, _currentPos->v.y, _currentSize->v.x, _currentSize->v.y),
            std::min(_currentSize->v.x, _currentSize->v.y) / 4.f);
        _currentColor = _currentColor->next;
        _currentPos = _currentPos->next;
        _currentSize = _currentSize->next;
    }
}

void Application::_drawRandomCircle()
{
    if (auto render = getSystemT<AV::Render::Render2D>().lock())
    {
        render->setFillColor(_currentColor->c);
        render->drawCircle(_currentPos->v, _currentSize->v.x);
        _currentColor = _currentColor->next;
        _currentPos = _currentPos->next;
        _currentSize = _currentSize->next;
    }
}

void Application::_drawRandomText()
{
    if (auto render = getSystemT<AV::Render::Render2D>().lock())
    {
        render->setFillColor(_currentColor->c);
        AV::Font::Info fontInfo;
        fontInfo.size = _currentText->size;
        render->setCurrentFont(fontInfo);
        render->drawText(_currentText->s, _currentPos->v);
        _currentColor = _currentColor->next;
        _currentPos = _currentPos->next;
        _currentSize = _currentSize->next;
        _currentText = _currentText->next;
    }
}

void Application::_render()
{
    if (auto render = getSystemT<AV::Render::Render2D>().lock())
    {
        glm::ivec2 size;
        auto glfwWindow = getGLFWWindow();
        glfwGetWindowSize(glfwWindow, &size.x, &size.y);
        if (size != windowSize)
        {
            windowSize = size;
            _generateRandomNumbers();
        }
        _initRandomNumbers();
        render->beginFrame(windowSize);
        for (size_t i = 0; i < drawCount / 4; ++i)
        {
            _drawRandomRectangle();
            _drawRandomRoundedRectangle();
            _drawRandomCircle();
            _drawRandomText();
        }
        render->endFrame();
    }
}

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        r = Application::create(argc, argv)->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}

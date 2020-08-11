// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/AVSystem.h>
#include <djvAV/Color.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IOSystem.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render2D.h>

#include <djvCore/Error.h>
#include <djvCore/String.h>
#include <djvCore/ResourceSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv;

const size_t drawCount = 10000;
const size_t randomCount = 1000;
AV::Image::Size windowSize;

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
        floorf(Core::Math::getRandom(windowSize.w / -2.f, windowSize.w * 1.5f)),
        floorf(Core::Math::getRandom(windowSize.h / -2.f, windowSize.h * 1.5f)))
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

struct RandomIcon
{
    RandomIcon(const std::vector<std::shared_ptr<AV::Image::Image> >& images)
    {
        image = images[Core::Math::getRandom(static_cast<int>(images.size()) - 1)];
    }
    std::shared_ptr<AV::Image::Image> image;
    RandomIcon * next = nullptr;
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
    void _generateRandomNumbers();
    void _initRandomNumbers();
    void _drawRandomRectangle();
    void _drawRandomPill();
    void _drawRandomCircle();
    void _drawRandomText();
    void _drawRandomIcon();
    void _render();

    GLFWwindow*  _glfwWindow   = nullptr;
    std::shared_ptr<AV::Render2D::Render> _render2D;
    RandomColor* _randomColors = nullptr;
    RandomColor* _currentColor = nullptr;
    RandomPos*   _randomPos    = nullptr;
    RandomPos*   _currentPos   = nullptr;
    RandomSize*  _randomSizes  = nullptr;
    RandomSize*  _currentSize  = nullptr;
    RandomText*  _randomText   = nullptr;
    RandomText*  _currentText  = nullptr;
    RandomIcon*  _randomIcon   = nullptr;
    RandomIcon*  _currentIcon  = nullptr;
    std::vector<std::shared_ptr<AV::Image::Image> > _images;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    _glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    //glfwSetWindowSize(_glfwWindow, 1280, 720);
    glfwShowWindow(_glfwWindow);
    _render2D = getSystemT<AV::Render2D::Render>();

    static const std::vector<std::string> names =
    {
        "96DPI/djvIconAdd.png"
    };
    auto io = getSystemT<AV::IO::System>();
    auto resourceSystem = getSystemT<Core::ResourceSystem>();
    for (const auto& i : names)
    {
        try
        {
            auto read = io->read(Core::FileSystem::Path(
                resourceSystem->getPath(Core::FileSystem::ResourcePath::Icons),
                i));
            while (1)
            {
                {
                    std::lock_guard<std::mutex> lock(read->getMutex());
                    auto& queue = read->getVideoQueue();
                    if (!queue.isEmpty())
                    {
                        _images.push_back(queue.getFrame().image);
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Cannot read " << names[0] << ": " << e.what() << std::endl;
        }
    }
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
    auto time = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        glfwPollEvents();
        _render();
        glfwSwapBuffers(_glfwWindow);
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = now - time;
        time = now;
        const float dt = delta.count();
        std::cout << "FPS: " << (dt > 0.f ? 1.f / dt : 0.f) << std::endl;
    }
}

void Application::_generateRandomNumbers()
{
    _randomColors = new RandomColor;
    _randomPos    = new RandomPos;
    _randomSizes  = new RandomSize;
    _randomText   = new RandomText;
    _randomIcon   = new RandomIcon(_images);
    auto newColor = _randomColors;
    auto newPos   = _randomPos;
    auto newSize  = _randomSizes;
    auto newText  = _randomText;
    auto newIcon  = _randomIcon;
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
        newIcon->next  = new RandomIcon(_images);
        newIcon        = newIcon->next;
    }
    newColor->next = _randomColors;
    newPos->next   = _randomPos;
    newSize->next  = _randomSizes;
    newText->next  = _randomText;
    newIcon->next  = _randomIcon;
}

void Application::_initRandomNumbers()
{
    _currentColor = _randomColors;
    _currentPos   = _randomPos;
    _currentSize  = _randomSizes;
    _currentText  = _randomText;
    _currentIcon  = _randomIcon;
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
    random = Core::Math::getRandom(static_cast<int>(randomCount));
    for (int i = 0; i < random; ++i)
    {
        _currentIcon = _currentIcon->next;
    }
}

void Application::_drawRandomRectangle()
{
    _render2D->setFillColor(_currentColor->c);
    _render2D->drawRect(Core::BBox2f(_currentPos->v.x, _currentPos->v.y, _currentSize->v.x, _currentSize->v.y));
    _currentColor = _currentColor->next;
    _currentPos = _currentPos->next;
    _currentSize = _currentSize->next;
}

void Application::_drawRandomPill()
{
    _render2D->setFillColor(_currentColor->c);
    _render2D->drawPill(
        Core::BBox2f(_currentPos->v.x, _currentPos->v.y, _currentSize->v.x, _currentSize->v.y));
    _currentColor = _currentColor->next;
    _currentPos = _currentPos->next;
    _currentSize = _currentSize->next;
}

void Application::_drawRandomCircle()
{
    _render2D->setFillColor(_currentColor->c);
    _render2D->drawCircle(_currentPos->v, _currentSize->v.x);
    _currentColor = _currentColor->next;
    _currentPos = _currentPos->next;
    _currentSize = _currentSize->next;
}

void Application::_drawRandomText()
{
    _render2D->setFillColor(_currentColor->c);
    auto fontSystem = getSystemT<AV::Font::System>();
    const AV::Font::FontInfo fontInfo(1, 1, _currentText->size, AV::dpiDefault);
    _render2D->drawText(fontSystem->getGlyphs(_currentText->s, fontInfo).get(), _currentPos->v);
    _currentColor = _currentColor->next;
    _currentPos = _currentPos->next;
    _currentSize = _currentSize->next;
    _currentText = _currentText->next;
}

void Application::_drawRandomIcon()
{
    _render2D->setFillColor(_currentColor->c);
    _render2D->drawFilledImage(_currentIcon->image, _currentPos->v);
    _currentColor = _currentColor->next;
    _currentPos = _currentPos->next;
    _currentSize = _currentSize->next;
    _currentIcon = _currentIcon->next;
}

void Application::_render()
{
    glm::ivec2 size = glm::ivec2(0, 0);
    glfwGetWindowSize(_glfwWindow, &size.x, &size.y);
    if (size.x != windowSize.w || size.y != windowSize.h)
    {
        windowSize.w = size.x;
        windowSize.h = size.y;
        _generateRandomNumbers();
    }
    _initRandomNumbers();
    _render2D->beginFrame(windowSize);
    for (size_t i = 0; i < drawCount / 5; ++i)
    {
        _drawRandomRectangle();
        _drawRandomPill();
        _drawRandomCircle();
        _drawRandomText();
        _drawRandomIcon();
    }
    _render2D->endFrame();
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

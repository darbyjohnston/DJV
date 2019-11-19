//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvAV/GLFWSystem.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render3D.h>
#include <djvAV/Shape.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/Error.h>
#include <djvCore/Math.h>
#include <djvCore/Timer.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv;

class Application : public CmdLine::Application
{
    DJV_NON_COPYABLE(Application);

protected:
    void _init(int argc, char ** argv);

    Application();

public:
    static std::shared_ptr<Application> create(int argc, char ** argv);

    int run();

private:
    void _render();

    std::vector<AV::Geom::TriangleMesh> _meshes;
    std::shared_ptr<AV::Render3D::IMaterial> _material;
    glm::vec3 _camera = glm::vec3(15.F, 15.F, 15.F);
    AV::Render3D::RenderOptions _options;
    std::shared_ptr<Core::Time::Timer> _timer;
};

void Application::_init(int argc, char ** argv)
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    CmdLine::Application::_init(args);

    for (size_t i = 0; i < 1000; ++i)
    {
        AV::Geom::TriangleMesh mesh;
        switch (Core::Math::getRandom(0, 2))
        {
        case 0:
        {
            AV::Geom::Cube shape(Core::Math::getRandom(.1F, 1.F));
            shape.triangulate(mesh);
            break;
        }
        case 1:
        {
            AV::Geom::Sphere shape(Core::Math::getRandom(.1F, 1.F), AV::Geom::Sphere::Resolution(20, 20));
            shape.triangulate(mesh);
            break;
        }
        case 2:
        {
            AV::Geom::Cylinder shape(Core::Math::getRandom(.1F, 1.F), Core::Math::getRandom(.1F, 1.F), 20);
            shape.setCapped(true);
            shape.triangulate(mesh);
            break;
        }
        }
        const glm::vec3 pos(
            Core::Math::getRandom(-10.F, 10.F),
            Core::Math::getRandom(-10.F, 10.F),
            Core::Math::getRandom(-10.F, 10.F));
        for (size_t i = 0; i < mesh.v.size(); ++i)
        {
            auto& v = mesh.v[i];
            v.x += pos.x;
            v.y += pos.y;
            v.z += pos.z;
        }
        _meshes.push_back(mesh);
    }

    _material = AV::Render3D::DefaultMaterial::create(shared_from_this());

    _options.size = AV::Image::Size(1280, 720);

    _timer = Core::Time::Timer::create(shared_from_this());
    _timer->setRepeating(true);
    _timer->start(
        std::chrono::milliseconds(10),
        [this](float)
    {
            _camera.y += .1F;
    });

    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    glfwSetWindowSize(glfwWindow, _options.size.w, _options.size.h);
    glfwShowWindow(glfwWindow);
}

Application::Application()
{}

std::shared_ptr<Application> Application::create(int argc, char ** argv)
{
    auto out = std::shared_ptr<Application>(new Application);
    out->_init(argc, argv);
    return out;
}

int Application::run()
{
    auto time = std::chrono::system_clock::now();
    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
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
        glfwSwapBuffers(glfwWindow);
        //glFlush();
    }
    return 0;
}

void Application::_render()
{
    auto render = getSystemT<AV::Render3D::Render>();
    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    glm::ivec2 windowSize = glm::ivec2(0, 0);
    glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
    //_options.camera.v = glm::lookAt(glm::vec3(0.F, 0.F, -5.F), glm::vec3(0.F, 0.F, 0.F), glm::vec3(0.F, 1.F, 0.F));
    _options.camera.v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_camera.z));
    _options.camera.v = glm::rotate(_options.camera.v, Core::Math::deg2rad(_camera.x), glm::vec3(1.F, 0.F, 0.F));
    _options.camera.v = glm::rotate(_options.camera.v, Core::Math::deg2rad(_camera.y), glm::vec3(0.F, 1.F, 0.F));
    _options.camera.p = glm::perspective(45.F, windowSize.x / static_cast<float>(windowSize.y), .01F, 1000.F);
    _options.size.w = windowSize.x;
    _options.size.h = windowSize.y;
    render->beginFrame(_options);
    render->setMaterial(_material);
    for (const auto& i : _meshes)
    {
        render->drawTriangleMesh(i);
    }
    render->endFrame();
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

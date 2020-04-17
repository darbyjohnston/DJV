// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvAV/GLFWSystem.h>
#include <djvAV/OpenGL.h>
#include <djvAV/Render3D.h>
#include <djvAV/Render3DCamera.h>
#include <djvAV/Render3DMaterial.h>
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
    void _init(std::list<std::string>&);

    Application();

public:
    static std::shared_ptr<Application> create(std::list<std::string>&);

    void run() override;

private:
    void _render();

    std::vector<AV::Geom::TriangleMesh> _meshes;
    std::shared_ptr<AV::Render3D::IMaterial> _material;
    glm::vec3 _camera = glm::vec3(15.F, 15.F, 15.F);
    AV::Render3D::RenderOptions _options;
    std::shared_ptr<Core::Time::Timer> _timer;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    size_t meshTriangles = 0;
    size_t meshVertices = 0;
    size_t meshNormals = 0;
    for (size_t i = 0; i < 10000; ++i)
    {
        AV::Geom::TriangleMesh mesh;
        switch (Core::Math::getRandom(0, 2))
        {
        case 0:
        {
            AV::Geom::Cube shape(Core::Math::getRandom(.1F, .2F));
            shape.triangulate(mesh);
            break;
        }
        case 1:
        {
            AV::Geom::Sphere shape(Core::Math::getRandom(.1F, .2F), AV::Geom::Sphere::Resolution(20, 20));
            shape.triangulate(mesh);
            break;
        }
        case 2:
        {
            AV::Geom::Cylinder shape(Core::Math::getRandom(.1F, .2F), Core::Math::getRandom(.1F, 1.F), 20);
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
        meshTriangles += mesh.triangles.size();
        meshVertices += mesh.v.size();
        meshNormals += mesh.n.size();
    }
    //std::cout << "triangles = " << meshTriangles << std::endl;
    //std::cout << "vertices = " << meshVertices << std::endl;
    //std::cout << "normals = " << meshNormals << std::endl;

    _material = AV::Render3D::DefaultMaterial::create(shared_from_this());

    _options.size = AV::Image::Size(1280, 720);

    _timer = Core::Time::Timer::create(shared_from_this());
    _timer->setRepeating(true);
    _timer->start(
        std::chrono::milliseconds(10),
        [this](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
    {
            _camera.y += .1F;
    });

    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    glfwSetWindowSize(glfwWindow, _options.size.w, _options.size.h);
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
    auto time = std::chrono::steady_clock::now();
    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    while (!glfwWindowShouldClose(glfwWindow))
    {
        auto now = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - time);
        time = now;

        glfwPollEvents();
        tick(time, delta);
        _render();
        glfwSwapBuffers(glfwWindow);
    }
}

void Application::_render()
{
    auto render = getSystemT<AV::Render3D::Render>();
    auto glfwWindow = getSystemT<AV::GLFW::System>()->getGLFWWindow();
    glm::ivec2 windowSize = glm::ivec2(0, 0);
    glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
    glm::mat4x4 v(1.F);
    glm::mat4x4 p(1.F);
    v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_camera.z));
    v = glm::rotate(v, Core::Math::deg2rad(_camera.x), glm::vec3(1.F, 0.F, 0.F));
    v = glm::rotate(v, Core::Math::deg2rad(_camera.y), glm::vec3(0.F, 1.F, 0.F));
    p = glm::perspective(45.F, windowSize.x / static_cast<float>(windowSize.y > 0 ? windowSize.y : 1.F), .01F, 1000.F);
    auto renderCamera = AV::Render3D::DefaultCamera::create();
    renderCamera->setV(v);
    renderCamera->setP(p);
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

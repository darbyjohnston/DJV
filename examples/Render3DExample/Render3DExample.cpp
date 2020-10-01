// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvCmdLineApp/Application.h>

#include <djvRender3D/Camera.h>
#include <djvRender3D/Light.h>
#include <djvRender3D/Material.h>
#include <djvRender3D/Render.h>
#include <djvRender3D/RenderSystem.h>

#include <djvGL/GLFWSystem.h>
#include <djvGL/GL.h>

#include <djvSystem/Timer.h>

#include <djvGeom/Shape.h>
#include <djvGeom/TriangleMesh.h>

#include <djvCore/ErrorFunc.h>
#include <djvCore/RandomFunc.h>

#include <djvMath/Math.h>

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

    std::vector<Geom::TriangleMesh> _meshes;
    std::shared_ptr<Render3D::IMaterial> _material;
    glm::vec3 _cameraPos = glm::vec3(15.F, 15.F, 15.F);
    std::shared_ptr<Render3D::DirectionalLight> _light;
    std::shared_ptr<Render3D::DefaultCamera> _camera;
    Render3D::RenderOptions _options;
    std::shared_ptr<System::Timer> _timer;
};

void Application::_init(std::list<std::string>& args)
{
    CmdLine::Application::_init(args);

    Render3D::RenderSystem::create(shared_from_this());

    size_t meshTriangles = 0;
    size_t meshVertices = 0;
    size_t meshNormals = 0;
    for (size_t i = 0; i < 10000; ++i)
    {
        Geom::TriangleMesh mesh;
        switch (Core::Random::getRandom(0, 2))
        {
        case 0:
        {
            Geom::Cube shape(Core::Random::getRandom(.1F, .2F));
            shape.triangulate(mesh);
            break;
        }
        case 1:
        {
            Geom::Sphere shape(Core::Random::getRandom(.1F, .2F), Geom::Sphere::Resolution(20, 20));
            shape.triangulate(mesh);
            break;
        }
        case 2:
        {
            Geom::Cylinder shape(Core::Random::getRandom(.1F, .2F), Core::Random::getRandom(.1F, 1.F), 20);
            shape.setCapped(true);
            shape.triangulate(mesh);
            break;
        }
        }
        const glm::vec3 pos(
            Core::Random::getRandom(-10.F, 10.F),
            Core::Random::getRandom(-10.F, 10.F),
            Core::Random::getRandom(-10.F, 10.F));
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

    _material = Render3D::DefaultMaterial::create(shared_from_this());

    _camera = Render3D::DefaultCamera::create();
    
    _light = Render3D::DirectionalLight::create();
    
    _options.camera = _camera;
    _options.size = Image::Size(1280, 720);

    _timer = System::Timer::create(shared_from_this());
    _timer->setRepeating(true);
    _timer->start(
        std::chrono::milliseconds(10),
        [this](const std::chrono::steady_clock::time_point&, const Core::Time::Duration&)
    {
            _cameraPos.y += .1F;
    });

    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
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
    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
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
    auto render = getSystemT<Render3D::Render>();
    auto glfwWindow = getSystemT<GL::GLFW::GLFWSystem>()->getWindow();
    glm::ivec2 windowSize = glm::ivec2(0, 0);
    glfwGetWindowSize(glfwWindow, &windowSize.x, &windowSize.y);
    glm::mat4x4 v(1.F);
    glm::mat4x4 p(1.F);
    v = glm::translate(glm::mat4x4(1.F), glm::vec3(0.F, 0.F, -_cameraPos.z));
    v = glm::rotate(v, Math::deg2rad(_cameraPos.x), glm::vec3(1.F, 0.F, 0.F));
    v = glm::rotate(v, Math::deg2rad(_cameraPos.y), glm::vec3(0.F, 1.F, 0.F));
    p = glm::perspective(45.F, windowSize.x / static_cast<float>(windowSize.y > 0 ? windowSize.y : 1.F), .01F, 1000.F);
    _camera->setV(v);
    _camera->setP(p);
    _options.size.w = windowSize.x;
    _options.size.h = windowSize.y;
    render->beginFrame(_options);
    render->addLight(_light);
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

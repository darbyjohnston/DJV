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

#include <djvDesktopApp/Application.h>

#include <djvUI/Label.h>
#include <djvUI/Window.h>

#include <djvScene/Camera.h>
#include <djvScene/Material.h>
#include <djvScene/Primitive.h>
#include <djvScene/Scene.h>

#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/Render2D.h>
#include <djvAV/Render3D.h>
#include <djvAV/Shape.h>

#include <djvCore/Error.h>

using namespace djv;

class SceneWidget : public UI::Widget
{
    DJV_NON_COPYABLE(SceneWidget);

protected:
    void _init(const std::shared_ptr<Core::Context>&);
    SceneWidget();

public:
    virtual ~SceneWidget();

    static std::shared_ptr<SceneWidget> create(const std::shared_ptr<Core::Context>&);

    void setScene(const std::shared_ptr<Scene::Scene>&);

protected:
    void _layoutEvent(Core::Event::Layout&) override;
    void _paintEvent(Core::Event::Paint&) override;
    void _pointerMoveEvent(Core::Event::PointerMove&) override;
    void _buttonPressEvent(Core::Event::ButtonPress&) override;
    void _buttonReleaseEvent(Core::Event::ButtonRelease&) override;

    void _updateEvent(Core::Event::Update&) override;

private:
    std::shared_ptr<AV::Render3D::Render> _render3D;
    AV::Image::Size _size;
    std::shared_ptr<Scene::Scene> _scene;
    std::shared_ptr<Scene::PolarCamera> _camera;
    std::shared_ptr<AV::OpenGL::OffscreenBuffer> _offscreenBuffer;
    Core::Event::PointerID _pressedID = Core::Event::InvalidID;
    glm::vec2 _pointerPos = glm::vec2(0.F, 0.F);
};

void SceneWidget::_init(const std::shared_ptr<Core::Context>& context)
{
    Widget::_init(context);
    setPointerEnabled(true);
    _render3D = context->getSystemT<AV::Render3D::Render>();
    _camera = Scene::PolarCamera::create();
}

SceneWidget::SceneWidget()
{}

SceneWidget::~SceneWidget()
{}

std::shared_ptr<SceneWidget> SceneWidget::create(const std::shared_ptr<Core::Context>& context)
{
    auto out = std::shared_ptr<SceneWidget>(new SceneWidget);
    out->_init(context);
    return out;
}

void SceneWidget::setScene(const std::shared_ptr<Scene::Scene>& value)
{
    _scene = value;
    _redraw();
}

void SceneWidget::_layoutEvent(Core::Event::Layout&)
{
    const Core::BBox2f& g = getGeometry();
    _size.w = g.w();
    _size.h = g.h();
    _camera->setAspect(_size.w / static_cast<float>(_size.h > 0 ? _size.h : 0));
}

void SceneWidget::_paintEvent(Core::Event::Paint&)
{
    if (_offscreenBuffer)
    {
        const Core::BBox2f& g = getGeometry();
        auto& render = _getRender();
        render->drawTexture(g, _offscreenBuffer->getTextureID());
    }
}

void SceneWidget::_pointerMoveEvent(Core::Event::PointerMove& event)
{
    Widget::_pointerMoveEvent(event);
    if (_pressedID)
    {
        event.accept();
        const auto& pointerInfo = event.getPointerInfo();
        _camera->setLon(_camera->getLon() + (pointerInfo.projectedPos.x - _pointerPos.x) * .5F);
        _camera->setLat(_camera->getLat() + (pointerInfo.projectedPos.y - _pointerPos.y) * .5F);
        _pointerPos = pointerInfo.projectedPos;
        _redraw();
    }
}

void SceneWidget::_buttonPressEvent(Core::Event::ButtonPress& event)
{
    if (_pressedID)
        return;
    event.accept();
    const auto& pointerInfo = event.getPointerInfo();
    _pressedID = pointerInfo.id;
    _pointerPos = pointerInfo.projectedPos;
}

void SceneWidget::_buttonReleaseEvent(Core::Event::ButtonRelease& event)
{
    const auto& pointerInfo = event.getPointerInfo();
    if (pointerInfo.id == _pressedID)
    {
        event.accept();
        _pressedID = Core::Event::InvalidID;
    }
}

void SceneWidget::_updateEvent(Core::Event::Update&)
{
    if (_size.isValid())
    {
        AV::Image::Info info(_size, AV::Image::Type::RGBA_U8);
        if (!_offscreenBuffer || (_offscreenBuffer && info != _offscreenBuffer->getInfo()))
        {
            _offscreenBuffer = AV::OpenGL::OffscreenBuffer::create(info);
        }
    }
    else
    {
        _offscreenBuffer.reset();
    }
    if (_offscreenBuffer)
    {
        const AV::OpenGL::OffscreenBufferBinding binding(_offscreenBuffer);
        if (_scene)
        {
            Scene::RenderOptions options;
            options.size = _size;
            options.camera = _camera;
            _scene->render(_render3D, options);
        }
    }
}
    
int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        // Create an application.
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
        auto app = Desktop::Application::create(args);

        // Create a scene.
        auto scene = Scene::Scene::create(app);
        AV::Geom::Cube cube;
        AV::Geom::TriangleMesh mesh;
        cube.triangulate(mesh);
        auto material = Scene::BasicMaterial::create(app);
        auto meshPrimitive = Scene::MeshPrimitive::create();
        meshPrimitive->setMesh(mesh);
        meshPrimitive->setMaterial(material);
        scene->addPrimitive(meshPrimitive);

        // Create the widgets.
        auto widget = SceneWidget::create(app);
        widget->setScene(scene);
        auto label = UI::Label::create(app);
        label->setText("Hello world!");
        label->setFontSizeRole(UI::MetricsRole::FontTitle);

        // Create a window and show it.
        auto window = UI::Window::create(app);
        window->addChild(widget);
        //window->addChild(label);
        window->show();

        // Run the application.
        r = app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}

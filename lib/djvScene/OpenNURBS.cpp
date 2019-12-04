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

#include <djvScene/OpenNURBS.h>

#include <djvScene/Light.h>
#include <djvScene/Layer.h>
#include <djvScene/Material.h>
#include <djvScene/Primitive.h>
#include <djvScene/Scene.h>

#include <djvAV/Color.h>
#include <djvAV/TriangleMesh.h>

#include <opennurbs.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace Scene
    {
        namespace IO
        {
            namespace OpenNURBS
            {
                namespace
                {
                    AV::Image::Color fromONColor3(const ON_Color& value)
                    {
                        return AV::Image::Color(
                            value.Red() / 255.f,
                            value.Green() / 255.f,
                            value.Blue() / 255.f);
                    }

                    AV::Image::Color fromONColor4(const ON_Color& value)
                    {
                        return AV::Image::Color(
                            value.Red() / 255.f,
                            value.Green() / 255.f,
                            value.Blue() / 255.f,
                            (255 - value.Alpha()) / 255.f);
                    }

                    glm::vec2 fromON(const ON_2dPoint& value)
                    {
                        return glm::vec2(value.x, value.y);
                    }

                    glm::vec3 fromON(const ON_3dPoint& value)
                    {
                        return glm::vec3(value.x, value.y, value.z);
                    }

                    glm::vec3 fromON(const ON_3fVector& value)
                    {
                        return glm::vec3(value.x, value.y, value.z);
                    }

                    std::shared_ptr<AV::Geom::TriangleMesh> readMesh(const ON_Mesh* onMesh)
                    {
                        auto out = std::make_shared<AV::Geom::TriangleMesh>();

                        const int faceCount = onMesh->FaceCount();
                        const bool hasTexCoord = onMesh->HasTextureCoordinates();
                        const bool hasNormals = onMesh->HasVertexNormals();

                        for (int i = 0; i < faceCount; ++i)
                        {
                            const ON_MeshFace& f = onMesh->m_F[i];
                            const size_t v = 1 + out->v.size();
                            const size_t n = 1 + out->n.size();
                            const size_t t = 1 + out->t.size();
                            for (int j = 0; j < 3; ++j)
                            {
                                out->v.push_back(fromON(onMesh->m_V[f.vi[j]]));
                                if (hasTexCoord)
                                {
                                    out->t.push_back(fromON(onMesh->m_T[f.vi[j]]));
                                }
                                if (hasNormals)
                                {
                                    out->n.push_back(fromON(onMesh->m_N[f.vi[j]]));
                                }
                            }
                            if (f.IsQuad())
                            {
                                out->v.push_back(fromON(onMesh->m_V[f.vi[3]]));
                                if (hasTexCoord)
                                {
                                    out->t.push_back(fromON(onMesh->m_T[f.vi[3]]));
                                }
                                if (hasNormals)
                                {
                                    out->n.push_back(fromON(onMesh->m_N[f.vi[3]]));
                                }
                                if (onMesh->m_V[f.vi[0]].DistanceTo(onMesh->m_V[f.vi[2]]) <=
                                    onMesh->m_V[f.vi[1]].DistanceTo(onMesh->m_V[f.vi[3]]))
                                {
                                    AV::Geom::TriangleMesh::Triangle a;
                                    a.v0 = AV::Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                    a.v1 = AV::Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                    a.v2 = AV::Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                    out->triangles.push_back(a);
                                    AV::Geom::TriangleMesh::Triangle b;
                                    a.v0 = AV::Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                    a.v1 = AV::Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                    a.v2 = AV::Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                    out->triangles.push_back(a);
                                }
                                else
                                {
                                    AV::Geom::TriangleMesh::Triangle a;
                                    a.v0 = AV::Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                    a.v1 = AV::Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                    a.v2 = AV::Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                    out->triangles.push_back(a);
                                    AV::Geom::TriangleMesh::Triangle b;
                                    a.v0 = AV::Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                    a.v1 = AV::Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                    a.v2 = AV::Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                    out->triangles.push_back(a);
                                }
                            }
                            else
                            {
                                AV::Geom::TriangleMesh::Triangle a;
                                a.v0 = AV::Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                a.v1 = AV::Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                a.v2 = AV::Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                out->triangles.push_back(a);
                            }
                        }
                        return out;
                    }

                    glm::mat4x4 toGlm(const ON_Xform& value)
                    {
                        auto out = glm::mat4x4(
                            value.m_xform[0][0], value.m_xform[1][0], value.m_xform[2][0], value.m_xform[3][0],
                            value.m_xform[0][1], value.m_xform[1][1], value.m_xform[2][1], value.m_xform[3][1],
                            value.m_xform[0][2], value.m_xform[1][2], value.m_xform[2][2], value.m_xform[3][2],
                            value.m_xform[0][3], value.m_xform[1][3], value.m_xform[2][3], value.m_xform[3][3]);
                        return out;
                    }

                    /*void createInstances(
                        const ONX_Model& onModel,
                        const ON_ModelGeometryComponent* onInstanceComponent,
                        std::map<const ON_Mesh*, std::shared_ptr<AV::Geom::TriangleMesh> >& onMeshToMesh,
                        std::map<const ON_Layer*, std::shared_ptr<Layer> >& onLayerToLayer,
                        std::map<const ON_Material*, std::shared_ptr<Material> >& onMaterialToMaterial,
                        const std::shared_ptr<AV::OpenGL::Shader>& shader,
                        const std::shared_ptr<Entity>& parent,
                        const std::shared_ptr<Context>& context)
                    {
                        if (auto onInstanceRef = ON_InstanceRef::Cast(onInstanceComponent->Geometry(nullptr)))
                        {
                            auto onModelComponentRef = onModel.ComponentFromId(ON_ModelComponent::Type::InstanceDefinition, onInstanceRef->m_instance_definition_uuid);
                            if (auto onInstanceDef = ON_InstanceDefinition::Cast(onModelComponentRef.ModelComponent()))
                            {
                                auto entity = Entity::create(context);
                                ON_wString name;
                                entity->setName(std::string(ON_String(onInstanceDef->GetName(name))));
                                const Pose& pose = Pose::fromGlm(toGlm(onInstanceRef->m_xform));
                                entity->setPose(pose);
                                entity->setParent(parent);

                                if (auto attr = onInstanceComponent->Attributes(nullptr))
                                {
                                    std::shared_ptr<Layer> layer;
                                    auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                                    if (!onModelComponentRef.IsEmpty())
                                    {
                                        if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                        {
                                            entity->setLayer(onLayerToLayer[onLayer]);
                                        }
                                    }
                                }

                                const auto& onGeometryIdList = onInstanceDef->InstanceGeometryIdList();
                                for (int i = 0; i < onGeometryIdList.Count(); ++i)
                                {
                                    auto onGeometryRef = onModel.ComponentFromId(ON_ModelComponent::Type::ModelGeometry, onGeometryIdList[i]);
                                    if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onGeometryRef.ModelComponent()))
                                    {
                                        if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                                        {
                                            std::shared_ptr<Layer> layer;
                                            auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                                            if (!onModelComponentRef.IsEmpty())
                                            {
                                                if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                                {
                                                    layer = onLayerToLayer[onLayer];
                                                }
                                            }

                                            std::shared_ptr<Material> material;
                                            {
                                                auto onModelComponentRef = onModel.RenderMaterialFromIndex(attr->m_material_index);
                                                if (auto onMaterial = ON_Material::Cast(onModelComponentRef.ModelComponent()))
                                                {
                                                    const auto i = onMaterialToMaterial.find(onMaterial);
                                                    if (i != onMaterialToMaterial.end())
                                                    {
                                                        material = i->second;
                                                    }
                                                    else
                                                    {
                                                        material = Material::create(context);
                                                        material->setShader(shader);
                                                        material->setColor(fromONColor3(onMaterial->Diffuse()));
                                                    }
                                                }
                                            }

                                            if (auto onSubInstance = ON_InstanceRef::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                            {
                                                createInstances(onModel, onModelGeometryComponent, onMeshToMesh, onLayerToLayer, onMaterialToMaterial, shader, entity, context);
                                            }
                                            else if (auto onMesh = ON_Mesh::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                            {
                                                const auto j = onMeshToMesh.find(onMesh);
                                                if (j != onMeshToMesh.end())
                                                {
                                                    auto meshInstanceComponent = MeshComponent::create(context);
                                                    meshInstanceComponent->setMesh(j->second);
                                                    meshInstanceComponent->setMaterial(material);
                                                    meshInstanceComponent->setLayer(layer);
                                                    meshInstanceComponent->setParent(entity);
                                                }
                                            }
                                            else if (auto onBrep = ON_Brep::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                            {
                                                ON_SimpleArray<const ON_Mesh*> onMeshes(onBrep->m_F.Count());
                                                const int onMeshCount = onBrep->GetMesh(ON::render_mesh, onMeshes);
                                                for (int j = 0; j < onMeshCount; ++j)
                                                {
                                                    const auto k = onMeshToMesh.find(onMeshes[j]);
                                                    if (k != onMeshToMesh.end())
                                                    {
                                                        auto meshInstanceComponent = MeshComponent::create(context);
                                                        meshInstanceComponent->setMesh(k->second);
                                                        meshInstanceComponent->setMaterial(material);
                                                        meshInstanceComponent->setLayer(layer);
                                                        meshInstanceComponent->setParent(entity);
                                                    }
                                                }
                                            }
                                            else if (auto onExtrusion = ON_Extrusion::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                            {
                                                if (auto onMesh = onExtrusion->m_mesh_cache.Mesh(ON::render_mesh))
                                                {
                                                    const auto j = onMeshToMesh.find(onMesh);
                                                    if (j != onMeshToMesh.end())
                                                    {
                                                        auto meshInstanceComponent = MeshComponent::create(context);
                                                        meshInstanceComponent->setMesh(j->second);
                                                        meshInstanceComponent->setMaterial(material);
                                                        meshInstanceComponent->setLayer(layer);
                                                        meshInstanceComponent->setParent(entity);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }*/

                    void read(const std::string& fileName, const std::shared_ptr<Scene>& scene)
                    {
                        ONX_Model onModel;
                        if (FILE * f = ON::OpenFile(String::toWide(fileName).c_str(), L"rb"))
                        {
                            ON_BinaryFile onFile(ON::archive_mode::read3dm, f);
                            ON_wString onErrorLogString;
                            ON_TextLog onErrorLog(onErrorLogString);
                            if (!onModel.Read(onFile, &onErrorLog))
                            {
                                std::stringstream ss;
                                ss << DJV_TEXT("The file") << " '" << fileName << "' " << DJV_TEXT("cannot be opened") << ". ";
                                throw std::runtime_error(ss.str());
                            }
                            ON::CloseFile(f);
                        }

                        std::map<const ON_Layer*, std::shared_ptr<Layer> > onLayerToLayer;
                        ONX_ModelComponentIterator layerIt(onModel, ON_ModelComponent::Type::Layer);
                        for (auto onModelComponent = layerIt.FirstComponent(); onModelComponent; onModelComponent = layerIt.NextComponent())
                        {
                            if (auto onLayer = ON_Layer::Cast(onModelComponent))
                            {
                                auto layer = Layer::create();
                                ON_wString onName;
                                layer->setName(std::string(ON_String(onLayer->GetName(onName))));
                                layer->setVisible(onLayer->IsVisible());
                                layer->setColor(fromONColor4(onLayer->m_color));
                                onLayerToLayer[onLayer] = layer;
                            }
                        }
                        for (const auto i : onLayerToLayer)
                        {
                            if (i.first->ParentIdIsNotNil())
                            {
                                auto onModelComponentRef = onModel.ComponentFromId(ON_ModelComponent::Type::Layer, i.first->ParentId());
                                if (!onModelComponentRef.IsEmpty())
                                {
                                    if (auto onParentLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                    {
                                        onLayerToLayer[onParentLayer]->addItem(onLayerToLayer[i.first]);
                                    }
                                }
                            }
                            else
                            {
                                scene->addLayer(onLayerToLayer[i.first]);
                            }
                        }

                        auto defaultMaterial = DefaultMaterial::create();

                        std::map<const ON_Mesh*, std::shared_ptr<AV::Geom::TriangleMesh> > onMeshToMesh;
                        std::map<const ON_Material*, std::shared_ptr<IMaterial> > onMaterialToMaterial;
                        ONX_ModelComponentIterator geometryIt(onModel, ON_ModelComponent::Type::ModelGeometry);
                        for (auto onModelComponent = geometryIt.FirstComponent(); onModelComponent; onModelComponent = geometryIt.NextComponent())
                        {
                            if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onModelComponent))
                            {
                                if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                                {
                                    std::shared_ptr<Layer> layer;
                                    auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                                    if (!onModelComponentRef.IsEmpty())
                                    {
                                        if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                        {
                                            layer = onLayerToLayer[onLayer];
                                        }
                                    }

                                    std::shared_ptr<IMaterial> material;
                                    {
                                        auto onModelComponentRef = onModel.RenderMaterialFromIndex(attr->m_material_index);
                                        if (auto onMaterial = ON_Material::Cast(onModelComponentRef.ModelComponent()))
                                        {
                                            const auto i = onMaterialToMaterial.find(onMaterial);
                                            if (i != onMaterialToMaterial.end())
                                            {
                                                material = i->second;
                                            }
                                            else
                                            {
                                                auto newMaterial = DefaultMaterial::create();
                                                newMaterial->setAmbient(fromONColor3(onMaterial->Ambient()));
                                                newMaterial->setDiffuse(fromONColor3(onMaterial->Diffuse()));
                                                newMaterial->setSpecular(fromONColor3(onMaterial->Specular()));
                                                newMaterial->setShine(onMaterial->Shine() / 255.F);
                                                newMaterial->setTransparency(onMaterial->Transparency());
                                                newMaterial->setReflectivity(onMaterial->Reflectivity());
                                                newMaterial->setDisableLighting(onMaterial->DisableLighting());
                                                material = newMaterial;
                                            }
                                        }
                                    }

                                    if (auto onCurve = ON_Curve::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                    {
                                    }
                                    else if (auto onMesh = ON_Mesh::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                    {
                                        auto mesh = readMesh(onMesh);
                                        onMeshToMesh[onMesh] = mesh;
                                        if (attr->Mode() != ON::idef_object)
                                        {
                                            auto primitive = MeshPrimitive::create();
                                            primitive->setMesh(mesh);
                                            primitive->setMaterial(material ? material : defaultMaterial);
                                            scene->addPrimitive(primitive);
                                            if (layer)
                                            {
                                                layer->addItem(primitive);
                                            }
                                        }
                                    }
                                    else if (auto onBrep = ON_Brep::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                    {
                                        ON_SimpleArray<const ON_Mesh*> onMeshes(onBrep->m_F.Count());
                                        const int onMeshCount = onBrep->GetMesh(ON::render_mesh, onMeshes);
                                        for (int i = 0; i < onMeshCount; ++i)
                                        {
                                            auto mesh = readMesh(onMeshes[i]);
                                            onMeshToMesh[onMeshes[i]] = mesh;
                                            if (attr->Mode() != ON::idef_object)
                                            {
                                                auto primitive = MeshPrimitive::create();
                                                primitive->setMesh(mesh);
                                                primitive->setMaterial(material ? material : defaultMaterial);
                                                scene->addPrimitive(primitive);
                                                if (layer)
                                                {
                                                    layer->addItem(primitive);
                                                }
                                            }
                                        }
                                    }
                                    else if (auto onExtrusion = ON_Extrusion::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                    {
                                        if (auto onMesh = onExtrusion->m_mesh_cache.Mesh(ON::render_mesh))
                                        {
                                            auto mesh = readMesh(onMesh);
                                            onMeshToMesh[onMesh] = mesh;
                                            if (attr->Mode() != ON::idef_object)
                                            {
                                                auto primitive = MeshPrimitive::create();
                                                primitive->setMesh(mesh);
                                                primitive->setMaterial(material ? material : defaultMaterial);
                                                scene->addPrimitive(primitive);
                                                if (layer)
                                                {
                                                    layer->addItem(primitive);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        geometryIt = ONX_ModelComponentIterator(onModel, ON_ModelComponent::Type::RenderLight);
                        for (auto onModelComponent = geometryIt.FirstComponent(); onModelComponent; onModelComponent = geometryIt.NextComponent())
                        {
                            if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onModelComponent))
                            {
                                if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                                {
                                    std::shared_ptr<Layer> layer;
                                    auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                                    if (!onModelComponentRef.IsEmpty())
                                    {
                                        if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                        {
                                            layer = onLayerToLayer[onLayer];
                                        }
                                    }

                                    if (auto onLight = ON_Light::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                    {
                                        if (onLight->IsPointLight())
                                        {
                                            auto light = PointLight::create();
                                            light->setXForm(glm::translate(glm::mat4x4(1.F), fromON(onLight->Location())));
                                            light->setIntensity(onLight->Intensity());
                                            scene->addPrimitive(light);
                                            if (layer)
                                            {
                                                layer->addItem(light);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                } // namespace

                struct Read::Private
                {

                };

                Read::Read() :
                    _p(new Private)
                {}

                Read::~Read()
                {}

                std::shared_ptr<Read> Read::create(
                    const Core::FileSystem::FileInfo& fileInfo,
                    const std::shared_ptr<Core::ResourceSystem>& resourceSystem,
                    const std::shared_ptr<Core::LogSystem>& logSystem)
                {
                    auto out = std::shared_ptr<Read>(new Read);
                    out->_init(fileInfo, resourceSystem, logSystem);
                    return out;
                }

                std::future<Info> Read::getInfo()
                {
                    return std::async(
                        std::launch::async,
                        [this]
                        {
                            return Info();
                        });
                }

                std::future<std::shared_ptr<Scene> > Read::getScene()
                {
                    return std::async(
                        std::launch::async,
                        [this]
                        {
                            auto scene = Scene::create();
                            scene->setSceneOrient(SceneOrient::ZUp);
                            read(_fileInfo.getFileName(), scene);
                            return scene;
                        });
                }

                struct Plugin::Private
                {
                    Options options;
                };

                Plugin::Plugin() :
                    _p(new Private)
                {}

                Plugin::~Plugin()
                {}

                std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<Context>& context)
                {
                    auto out = std::shared_ptr<Plugin>(new Plugin);
                    out->_init(
                        pluginName,
                        DJV_TEXT("This plugin provides OpenNURBS file I/O."),
                        fileExtensions,
                        context);
                    return out;
                }

                picojson::value Plugin::getOptions() const
                {
                    return toJSON(_p->options);
                }

                void Plugin::setOptions(const picojson::value& value)
                {
                    fromJSON(value, _p->options);
                }

                std::shared_ptr<IRead> Plugin::read(const FileSystem::FileInfo& fileInfo) const
                {
                    return Read::create(fileInfo, _resourceSystem, _logSystem);
                }

            } // namespace OpenNURBS
        } // namespace IO
    } // namespace Scene
    
    picojson::value toJSON(const Scene::IO::OpenNURBS::Options& value)
    {
        picojson::value out(picojson::object_type, true);
        {
        }
        return out;
    }

    void fromJSON(const picojson::value& value, Scene::IO::OpenNURBS::Options& out)
    {
        if (value.is<picojson::object>())
        {
            for (const auto& i : value.get<picojson::object>())
            {
            }
        }
        else
        {
            throw std::invalid_argument(DJV_TEXT("Cannot parse the value."));
        }
    }

} // namespace djv


// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/OpenNURBS.h>

#include <djvScene3D/InstancePrimitive.h>
#include <djvScene3D/Light.h>
#include <djvScene3D/Layer.h>
#include <djvScene3D/Material.h>
#include <djvScene3D/MeshPrimitive.h>
#include <djvScene3D/NullPrimitive.h>
#include <djvScene3D/PointListPrimitive.h>
#include <djvScene3D/PolyLinePrimitive.h>
#include <djvScene3D/Scene.h>

#include <djvImage/Color.h>

#include <djvSystem/TextSystem.h>

#include <djvGeom/TriangleMesh.h>

#include <djvCore/StringFormat.h>
#include <djvCore/String.h>

#include <opennurbs/opennurbs.h>

#include <glm/gtc/matrix_transform.hpp>

#include <map>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        namespace OpenNURBS
        {
            namespace
            {
                Image::Color fromONColor3(const ON_Color& value)
                {
                    return Image::Color::RGB_U8(
                        value.Red  (),
                        value.Green(),
                        value.Blue ());
                }

                Image::Color fromONColor4(const ON_Color& value)
                {
                    return Image::Color(
                        value.Red  (),
                        value.Green(),
                        value.Blue (),
                        (255 - value.Alpha()));
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

                glm::mat4x4 fromON(const ON_Xform& value)
                {
                    return glm::mat4x4(
                        value.m_xform[0][0], value.m_xform[1][0], value.m_xform[2][0], value.m_xform[3][0],
                        value.m_xform[0][1], value.m_xform[1][1], value.m_xform[2][1], value.m_xform[3][1],
                        value.m_xform[0][2], value.m_xform[1][2], value.m_xform[2][2], value.m_xform[3][2],
                        value.m_xform[0][3], value.m_xform[1][3], value.m_xform[2][3], value.m_xform[3][3]);
                }

                std::shared_ptr<Geom::TriangleMesh> readMesh(const ON_Mesh* onMesh)
                {
                    auto out = std::make_shared<Geom::TriangleMesh>();

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
                                Geom::TriangleMesh::Triangle a;
                                a.v0 = Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                a.v1 = Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                a.v2 = Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                out->triangles.push_back(a);
                                Geom::TriangleMesh::Triangle b;
                                a.v0 = Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                a.v1 = Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                a.v2 = Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                out->triangles.push_back(a);
                            }
                            else
                            {
                                Geom::TriangleMesh::Triangle a;
                                a.v0 = Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                a.v1 = Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                                a.v2 = Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                out->triangles.push_back(a);
                                Geom::TriangleMesh::Triangle b;
                                a.v0 = Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                                a.v1 = Geom::TriangleMesh::Vertex(v + 3, hasTexCoord ? t + 3 : 0, hasNormals ? n + 3 : 0);
                                a.v2 = Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                                out->triangles.push_back(a);
                            }
                        }
                        else
                        {
                            Geom::TriangleMesh::Triangle a;
                            a.v0 = Geom::TriangleMesh::Vertex(v + 0, hasTexCoord ? t + 0 : 0, hasNormals ? n + 0 : 0);
                            a.v1 = Geom::TriangleMesh::Vertex(v + 1, hasTexCoord ? t + 1 : 0, hasNormals ? n + 1 : 0);
                            a.v2 = Geom::TriangleMesh::Vertex(v + 2, hasTexCoord ? t + 2 : 0, hasNormals ? n + 2 : 0);
                            out->triangles.push_back(a);
                        }
                    }
                    out->bboxUpdate();
                    return out;
                }

                struct ReadData
                {
                    std::shared_ptr<Scene> scene;
                    std::map<const ON_Layer*, std::shared_ptr<Layer> > onLayerToLayer;
                    std::map<const ON_Material*, std::shared_ptr<IMaterial> > onMaterialToMaterial;
                    std::map<const ON_InstanceDefinition*, std::shared_ptr<IPrimitive> > onInstanceDefToInstance;
                    std::map<std::shared_ptr<InstancePrimitive>, const ON_InstanceDefinition* > instanceToOnInstanceDef;
                    std::map<const ON_Mesh*, std::shared_ptr<Geom::TriangleMesh> > onMeshToMesh;
                };

                std::shared_ptr<IPrimitive> readGeometryComponent(
                    const ONX_Model& onModel,
                    const ON_ModelGeometryComponent* onModelGeometryComponent,
                    ReadData& data)
                {
                    std::shared_ptr<IPrimitive> out;
                    if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                    {
                        // Get the layer.
                        std::shared_ptr<Layer> layer;
                        auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                        if (!onModelComponentRef.IsEmpty())
                        {
                            if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                            {
                                const auto i = data.onLayerToLayer.find(onLayer);
                                if (i != data.onLayerToLayer.end())
                                {
                                    layer = i->second;
                                }
                            }
                        }

                        // Get the material.
                        const ON_Material* onMaterial = nullptr;
                        std::shared_ptr<IMaterial> material;
                        {
                            auto onModelComponentRef = onModel.RenderMaterialFromIndex(attr->m_material_index);
                            if (onMaterial = ON_Material::Cast(onModelComponentRef.ModelComponent()))
                            {
                                const auto i = data.onMaterialToMaterial.find(onMaterial);
                                if (i != data.onMaterialToMaterial.end())
                                {
                                    material = i->second;
                                }
                            }
                        }

                        // Read the primitive.
                        std::string name = ON_String(attr->Name());
                        if (auto onPoint = ON_Point::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            auto newPrimitive = PointListPrimitive::create();
                            auto pointList = std::shared_ptr<Geom::PointList>(new Geom::PointList);
                            pointList->v.push_back(fromON(*onPoint));
                            newPrimitive->setPointList(pointList);
                            out = newPrimitive;
                        }
                        else if (auto onCurve = ON_Curve::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            ON_SimpleArray<ON_3dPoint> onPoints;
                            ON_SimpleArray<double> onParameters;
                            if (onCurve->IsPolyline(&onPoints, &onParameters) >= 2)
                            {
                                auto newPrimitive = PolyLinePrimitive::create();
                                auto pointList = std::shared_ptr<Geom::PointList>(new Geom::PointList);
                                for (unsigned int i = 0; i < onPoints.Count(); ++i)
                                {
                                    pointList->v.push_back(fromON(onPoints[i]));
                                }
                                newPrimitive->addPointList(pointList);
                                out = newPrimitive;
                            }
                        }
                        else if (auto onMesh = ON_Mesh::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            std::shared_ptr<Geom::TriangleMesh> mesh;
                            const auto i = data.onMeshToMesh.find(onMesh);
                            if (i != data.onMeshToMesh.end())
                            {
                                mesh = i->second;
                            }
                            else
                            {
                                mesh = readMesh(onMesh);
                                data.onMeshToMesh[onMesh] = mesh;
                            }
                            if (mesh && mesh->triangles.size() > 0)
                            {
                                auto newPrimitive = MeshPrimitive::create();
                                newPrimitive->addMesh(mesh);
                                out = newPrimitive;
                            }
                        }
                        else if (auto onBrep = ON_Brep::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            ON_SimpleArray<const ON_Mesh*> onMeshes(onBrep->m_F.Count());
                            std::shared_ptr<MeshPrimitive> newPrimitive;
                            const int onMeshCount = onBrep->GetMesh(ON::render_mesh, onMeshes);
                            for (int i = 0; i < onMeshCount; ++i)
                            {
                                std::shared_ptr<Geom::TriangleMesh> mesh;
                                const auto j = data.onMeshToMesh.find(onMeshes[i]);
                                if (j != data.onMeshToMesh.end())
                                {
                                    mesh = j->second;
                                }
                                else
                                {
                                    mesh = readMesh(onMeshes[i]);
                                    data.onMeshToMesh[onMeshes[i]] = mesh;
                                }
                                if (!newPrimitive && mesh && mesh->triangles.size() > 0)
                                {
                                    newPrimitive = MeshPrimitive::create();
                                    out = newPrimitive;
                                }
                                if (newPrimitive && mesh && mesh->triangles.size() > 0)
                                {
                                    newPrimitive->addMesh(mesh);
                                }
                            }
                        }
                        else if (auto onExtrusion = ON_Extrusion::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            if (auto onMesh = onExtrusion->Mesh(ON::render_mesh))
                            {
                                std::shared_ptr<Geom::TriangleMesh> mesh;
                                const auto i = data.onMeshToMesh.find(onMesh);
                                if (i != data.onMeshToMesh.end())
                                {
                                    mesh = i->second;
                                }
                                else
                                {
                                    mesh = readMesh(onMesh);
                                    data.onMeshToMesh[onMesh] = mesh;
                                }
                                if (mesh && mesh->triangles.size() > 0)
                                {
                                    auto newPrimitive = MeshPrimitive::create();
                                    newPrimitive->addMesh(mesh);
                                    out = newPrimitive;
                                }
                            }
                        }
                        else if (auto onInstanceRef = ON_InstanceRef::Cast(onModelGeometryComponent->Geometry(nullptr)))
                        {
                            auto onModelComponentRef = onModel.ComponentFromId(ON_ModelComponent::Type::InstanceDefinition, onInstanceRef->m_instance_definition_uuid);
                            if (auto onInstanceDef = ON_InstanceDefinition::Cast(onModelComponentRef.ModelComponent()))
                            {
                                auto instancePrimitive = InstancePrimitive::create();
                                instancePrimitive->setXForm(fromON(onInstanceRef->m_xform));
                                data.instanceToOnInstanceDef[instancePrimitive] = onInstanceDef;
                                out = instancePrimitive;
                            }
                        }
                        if (out)
                        {
                            out->setName(name);
                            out->setVisible(attr->IsVisible());
                            out->setColor(fromONColor4(attr->m_color));
                            out->setMaterial(material);
                            if (layer)
                            {
                                layer->addItem(out);
                            }
                        }
                    }
                    return out;
                }

                void read(
                    const std::string& fileName,
                    ReadData& data,
                    const std::shared_ptr<System::TextSystem>& textSystem,
                    const std::shared_ptr<IPrimitive>& parent = nullptr)
                {
                    // Open the file.
                    ONX_Model onModel;
                    if (FILE * f = ON::OpenFile(String::toWide(fileName).c_str(), L"rb"))
                    {
                        ON_BinaryFile onFile(ON::archive_mode::read3dm, f);
                        ON_wString onErrorLogString;
                        ON_TextLog onErrorLog(onErrorLogString);
                        if (!onModel.Read(onFile, &onErrorLog))
                        {
                            throw std::runtime_error(String::Format("{0}: {1}").
                                arg(fileName).
                                arg(textSystem->getText(DJV_TEXT("error_file_open"))));
                        }
                        ON::CloseFile(f);
                    }

                    // Read the layers.
                    ONX_ModelComponentIterator layerIt(onModel, ON_ModelComponent::Type::Layer);
                    for (auto onModelComponent = layerIt.FirstComponent(); onModelComponent; onModelComponent = layerIt.NextComponent())
                    {
                        if (auto onLayer = ON_Layer::Cast(onModelComponent))
                        {
                            auto layer = Layer::create();
                            ON_wString onName;
                            layer->setName(std::string(ON_String(onLayer->GetName(onName))));
                            layer->setVisible(onLayer->IsVisible());
                            layer->setColor(fromONColor4(onLayer->Color()));
                            data.onLayerToLayer[onLayer] = layer;
                            if (onLayer->ParentIdIsNotNil())
                            {
                                auto onModelComponentRef = onModel.ComponentFromId(ON_ModelComponent::Type::Layer, onLayer->ParentId());
                                if (!onModelComponentRef.IsEmpty())
                                {
                                    if (auto onParentLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                    {
                                        data.onLayerToLayer[onParentLayer]->addItem(data.onLayerToLayer[onLayer]);
                                    }
                                }
                            }
                            else
                            {
                                data.scene->addLayer(data.onLayerToLayer[onLayer]);
                            }
                        }
                    }

                    // Read the materials.
                    ONX_ModelComponentIterator materialIt(onModel, ON_ModelComponent::Type::RenderMaterial);
                    for (auto onModelComponent = materialIt.FirstComponent(); onModelComponent; onModelComponent = materialIt.NextComponent())
                    {
                        if (auto onMaterial = ON_Material::Cast(onModelComponent))
                        {
                            auto material = DefaultMaterial::create();
                            material->setAmbient(fromONColor3(onMaterial->Ambient()));
                            material->setDiffuse(fromONColor3(onMaterial->Diffuse()));
                            material->setSpecular(fromONColor3(onMaterial->Specular()));
                            material->setShine(onMaterial->Shine() / 255.F);
                            material->setTransparency(onMaterial->Transparency());
                            material->setReflectivity(onMaterial->Reflectivity());
                            material->setDisableLighting(onMaterial->DisableLighting());
                            data.onMaterialToMaterial[onMaterial] = material;
                        }
                    }

                    // Read the instance definitions.
                    ONX_ModelComponentIterator instanceDefIt(onModel, ON_ModelComponent::Type::InstanceDefinition);
                    for (auto onModelComponent = instanceDefIt.FirstComponent(); onModelComponent; onModelComponent = instanceDefIt.NextComponent())
                    {
                        if (auto onInstanceDef = ON_InstanceDefinition::Cast(onModelComponent))
                        {
                            auto primitive = NullPrimitive::create();
                            primitive->setName(std::string(ON_String(onInstanceDef->Name())));
                            const auto& onGeometryIdList = onInstanceDef->InstanceGeometryIdList();
                            for (int i = 0; i < onGeometryIdList.Count(); ++i)
                            {
                                auto onGeometryRef = onModel.ComponentFromId(ON_ModelComponent::Type::ModelGeometry, onGeometryIdList[i]);
                                if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onGeometryRef.ModelComponent()))
                                {
                                    if (auto child = readGeometryComponent(onModel, onModelGeometryComponent, data))
                                    {
                                        primitive->addChild(child);
                                    }
                                }
                            }
                            if (ON_InstanceDefinition::IDEF_UPDATE_TYPE::Linked == onInstanceDef->InstanceDefinitionType())
                            {
                                //const std::string& fileName = std::string(ON_String(onInstanceDef->LinkedFilePath()));
                                const std::string& fileName = std::string(ON_String(onInstanceDef->LinkedFileReference().FullPath()));
                                //primitive->setName(fileName);
                                ReadData data2;
                                data2.scene = data.scene;
                                read(fileName, data2, textSystem, primitive);
                            }
                            data.onInstanceDefToInstance[onInstanceDef] = primitive;
                            data.scene->addDefinition(primitive);
                        }
                    }

                    // Read the primitives.
                    ONX_ModelComponentIterator geometryIt(onModel, ON_ModelComponent::Type::ModelGeometry);
                    for (auto onModelComponent = geometryIt.FirstComponent(); onModelComponent; onModelComponent = geometryIt.NextComponent())
                    {
                        if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onModelComponent))
                        {
                            if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                            {
                                if (attr->Mode() != ON::idef_object)
                                {
                                    if (auto primitive = readGeometryComponent(onModel, onModelGeometryComponent, data))
                                    {
                                        if (parent)
                                        {
                                            parent->addChild(primitive);
                                        }
                                        else
                                        {
                                            data.scene->addPrimitive(primitive);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Read the lights.
                    geometryIt = ONX_ModelComponentIterator(onModel, ON_ModelComponent::Type::RenderLight);
                    for (auto onModelComponent = geometryIt.FirstComponent(); onModelComponent; onModelComponent = geometryIt.NextComponent())
                    {
                        if (auto onModelGeometryComponent = ON_ModelGeometryComponent::Cast(onModelComponent))
                        {
                            if (auto attr = onModelGeometryComponent->Attributes(nullptr))
                            {
                                // Get the layer.
                                std::shared_ptr<Layer> layer;
                                auto onModelComponentRef = onModel.ComponentFromIndex(ON_ModelComponent::Type::Layer, attr->m_layer_index);
                                if (!onModelComponentRef.IsEmpty())
                                {
                                    if (auto onLayer = ON_Layer::Cast(onModelComponentRef.ModelComponent()))
                                    {
                                        layer = data.onLayerToLayer[onLayer];
                                    }
                                }

                                // Read the light.
                                std::shared_ptr<IPrimitive> primitive;
                                if (auto onLight = ON_Light::Cast(onModelGeometryComponent->Geometry(nullptr)))
                                {
                                    std::shared_ptr<ILight> light;
                                    if (onLight->IsDirectionalLight())
                                    {
                                        auto directionalLight = DirectionalLight::create();
                                        directionalLight->setDirection(fromON(onLight->Direction()));
                                        light = directionalLight;
                                    }
                                    else if (onLight->IsPointLight())
                                    {
                                        auto pointLight = PointLight::create();
                                        light = pointLight;
                                    }
                                    else if (onLight->IsSpotLight())
                                    {
                                        auto spotLight = SpotLight::create();
                                        spotLight->setConeAngle(onLight->SpotAngleDegrees());
                                        spotLight->setDirection(fromON(onLight->Direction()));
                                        light = spotLight;
                                    }
                                    if (light)
                                    {
                                        light->setXForm(glm::translate(glm::mat4x4(1.F), fromON(onLight->Location())));
                                        light->setEnabled(onLight->IsEnabled());
                                        light->setIntensity(onLight->Intensity());
                                        primitive = light;
                                    }
                                }
                                if (primitive)
                                {
                                    primitive->setName(std::string(ON_String(attr->Name())));
                                    primitive->setVisible(attr->IsVisible());
                                    if (layer)
                                    {
                                        layer->addItem(primitive);
                                    }
                                    if (parent)
                                    {
                                        parent->addChild(primitive);
                                    }
                                    else
                                    {
                                        data.scene->addPrimitive(primitive);
                                    }
                                }
                            }
                        }
                    }

                    // Assign the instances.
                    for (const auto& i : data.instanceToOnInstanceDef)
                    {
                        const auto j = data.onInstanceDefToInstance.find(i.second);
                        if (j != data.onInstanceDefToInstance.end())
                        {
                            if (i.first->getName().empty())
                            {
                                std::stringstream ss;
                                ss << j->second->getName() << " Instance";
                                i.first->setName(ss.str());
                            }
                            i.first->addInstance(j->second);
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
                const System::File::Info& fileInfo,
                const std::shared_ptr<System::TextSystem>& textSystem,
                const std::shared_ptr<System::ResourceSystem>& resourceSystem,
                const std::shared_ptr<System::LogSystem>& logSystem)
            {
                auto out = std::shared_ptr<Read>(new Read);
                out->_init(fileInfo, textSystem, resourceSystem, logSystem);
                return out;
            }

            std::future<IO::Info> Read::getInfo()
            {
                return std::async(
                    std::launch::async,
                    [this]
                    {
                        return IO::Info();
                    });
            }

            std::future<std::shared_ptr<Scene> > Read::getScene()
            {
                return std::async(
                    std::launch::async,
                    [this]
                    {
                        ReadData data;
                        auto scene = Scene::create();
                        scene->setSceneOrient(SceneOrient::ZUp);
                        data.scene = scene;
                        read(_fileInfo.getFileName(), data, _textSystem);
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

            std::shared_ptr<Plugin> Plugin::create(const std::shared_ptr<System::Context>& context)
            {
                auto out = std::shared_ptr<Plugin>(new Plugin);
                out->_init(
                    pluginName,
                    DJV_TEXT("opennurbs_plugin_description"),
                    fileExtensions,
                    context);
                return out;
            }

            rapidjson::Value Plugin::getOptions(rapidjson::Document::AllocatorType& allocator) const
            {
                return toJSON(_p->options, allocator);
            }

            void Plugin::setOptions(const rapidjson::Value& value)
            {
                fromJSON(value, _p->options);
            }

            std::shared_ptr<IO::IRead> Plugin::read(const System::File::Info& fileInfo) const
            {
                return Read::create(fileInfo, _textSystem, _resourceSystem, _logSystem);
            }

        } // namespace OpenNURBS
    } // namespace Scene3D
    
    rapidjson::Value toJSON(const Scene3D::OpenNURBS::Options& value, rapidjson::Document::AllocatorType&)
    {
        rapidjson::Value out(rapidjson::kObjectType);
        {
        }
        return out;
    }

    void fromJSON(const rapidjson::Value& value, Scene3D::OpenNURBS::Options& out)
    {
        if (value.IsObject())
        {
        }
        else
        {
            //! \todo How can we translate this?
            throw std::invalid_argument(DJV_TEXT("error_cannot_parse_the_value"));
        }
    }

} // namespace djv


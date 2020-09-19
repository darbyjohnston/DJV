// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvRender3D/Material.h>

#include <djvRender3D/Light.h>

#include <djvGL/Shader.h>
#include <djvGL/ShaderSystem.h>

#include <djvSystem/Context.h>

#include <array>

using namespace djv::Core;

namespace djv
{
    namespace Render3D
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t directionalLightCountMax   = 16;
            const size_t pointLightCountMax         = 16;
            const size_t spotLightCountMax          = 16;

        } // namespace

        void IMaterial::_init(
            const std::string& vertex,
            const std::string& fragment,
            const std::shared_ptr<System::Context>& context)
        {
            auto shaderSystem = context->getSystemT<GL::ShaderSystem>();
            _shader = shaderSystem->getShader(vertex, fragment);
        }

        void SolidColorMaterial::_init(const std::shared_ptr<System::Context>& context)
        {
            IMaterial::_init("djvRender3DSolidColorVertex.glsl", "djvRender3DSolidColorFragment.glsl", context);
            auto program = _shader->getProgram();
            _locations["transform.m"] = glGetUniformLocation(program, "transform.m");
            _locations["transform.mvp"] = glGetUniformLocation(program, "transform.mvp");
            _locations["color"] = glGetUniformLocation(program, "color");
        }

        std::shared_ptr<SolidColorMaterial> SolidColorMaterial::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<SolidColorMaterial>(new SolidColorMaterial);
            out->_init(context);
            return out;
        }

        void SolidColorMaterial::primitiveBind(const PrimitiveBindData& data)
        {
            _shader->setUniform(_locations["transform.m"], data.model);
            _shader->setUniform(_locations["transform.mvp"], data.camera * data.model);
            _shader->setUniform(_locations["color"], data.color);
        }

        void DefaultMaterial::_init(const std::shared_ptr<System::Context>& context)
        {
            IMaterial::_init("djvRender3DDefaultVertex.glsl", "djvRender3DDefaultFragment.glsl", context);

            auto program = _shader->getProgram();
            _locations["transform.m"] = glGetUniformLocation(program, "transform.m");
            _locations["transform.mvp"] = glGetUniformLocation(program, "transform.mvp");
            _locations["transform.normals"] = glGetUniformLocation(program, "transform.normals");

            _locations["hemisphereLight.intensity"] = glGetUniformLocation(program, "hemisphereLight.intensity");
            _locations["hemisphereLight.up"] = glGetUniformLocation(program, "hemisphereLight.up");
            _locations["hemisphereLight.topColor"] = glGetUniformLocation(program, "hemisphereLight.topColor");
            _locations["hemisphereLight.bottomColor"] = glGetUniformLocation(program, "hemisphereLight.bottomColor");
            _locations["hemisphereLightEnabled"] = glGetUniformLocation(program, "hemisphereLightEnabled");

            _locations["directionalLightsCount"] = glGetUniformLocation(program, "directionalLightsCount");
            _locations["pointLightsCount"] = glGetUniformLocation(program, "pointLightsCount");
            _locations["spotLightsCount"] = glGetUniformLocation(program, "spotLightsCount");

            _locations["defaultMaterial.mode"] = glGetUniformLocation(program, "defaultMaterial.mode");
            _locations["defaultMaterial.ambient"] = glGetUniformLocation(program, "defaultMaterial.ambient");
            _locations["defaultMaterial.diffuse"] = glGetUniformLocation(program, "defaultMaterial.diffuse");
            _locations["defaultMaterial.emission"] = glGetUniformLocation(program, "defaultMaterial.emission");
            _locations["defaultMaterial.specular"] = glGetUniformLocation(program, "defaultMaterial.specular");
            _locations["defaultMaterial.shine"] = glGetUniformLocation(program, "defaultMaterial.shine");
            _locations["defaultMaterial.transparency"] = glGetUniformLocation(program, "defaultMaterial.transparency");
            _locations["defaultMaterial.reflectivity"] = glGetUniformLocation(program, "defaultMaterial.reflectivity");
            _locations["defaultMaterial.disableLighting"] = glGetUniformLocation(program, "defaultMaterial.disableLighting");
        }

        std::shared_ptr<DefaultMaterial> DefaultMaterial::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
            out->_init(context);
            return out;
        }

        void DefaultMaterial::bind(const BindData& data)
        {
            size_t hemisphereLightCount = 0;
            size_t directionalLightCount = 0;
            size_t pointLightCount = 0;
            size_t spotLightCount = 0;
            for (auto light = data.lights.begin(); light != data.lights.end(); ++light)
            {
                if (auto hemisphereLight = std::dynamic_pointer_cast<HemisphereLight>(*light))
                {
                    _shader->setUniform(_locations["hemisphereLight.intensity"], hemisphereLight->getIntensity());
                    _shader->setUniform(_locations["hemisphereLight.up"], hemisphereLight->getUp());
                    _shader->setUniformF(_locations["hemisphereLight.topColor"], hemisphereLight->getTopColor());
                    _shader->setUniformF(_locations["hemisphereLight.bottomColor"], hemisphereLight->getBottomColor());
                    ++hemisphereLightCount;
                }
                else if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(*light))
                {
                    if (directionalLightCount < directionalLightCountMax)
                    {
                        {
                            std::stringstream ss;
                            ss << "directionalLights[" << directionalLightCount << "].intensity";
                            _shader->setUniform(ss.str(), directionalLight->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "directionalLights[" << directionalLightCount << "].direction";
                            _shader->setUniform(ss.str(), glm::normalize(directionalLight->getDirection()));
                        }
                        ++directionalLightCount;
                    }
                }
                else if (auto pointLight = std::dynamic_pointer_cast<PointLight>(*light))
                {
                    if (pointLightCount < pointLightCountMax)
                    {
                        {
                            std::stringstream ss;
                            ss << "pointLights[" << pointLightCount << "].intensity";
                            _shader->setUniform(ss.str(), pointLight->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "pointLights[" << pointLightCount << "].position";
                            _shader->setUniform(ss.str(), pointLight->getPosition());
                        }
                        ++pointLightCount;
                    }
                }
                else if (auto spotLight = std::dynamic_pointer_cast<SpotLight>(*light))
                {
                    if (spotLightCount < spotLightCountMax)
                    {
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << spotLightCount << "].intensity";
                            _shader->setUniform(ss.str(), spotLight->getIntensity());
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << spotLightCount << "].coneAngle";
                            _shader->setUniform(ss.str(), spotLight->getConeAngle());
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << spotLightCount << "].direction";
                            _shader->setUniform(ss.str(), glm::normalize(spotLight->getDirection()));
                        }
                        {
                            std::stringstream ss;
                            ss << "spotLights[" << spotLightCount << "].position";
                            _shader->setUniform(ss.str(), spotLight->getPosition());
                        }
                        ++spotLightCount;
                    }
                }
            }
            _shader->setUniform(_locations["hemisphereLightEnabled"], hemisphereLightCount ? 1 : 0);
            _shader->setUniform(_locations["directionalLightsCount"], static_cast<int>(directionalLightCount));
            _shader->setUniform(_locations["pointLightsCount"], static_cast<int>(pointLightCount));
            _shader->setUniform(_locations["spotLightsCount"], static_cast<int>(spotLightCount));

            _shader->setUniform(_locations["defaultMaterial.mode"], static_cast<int>(_mode));
            _shader->setUniform(_locations["defaultMaterial.ambient"], _ambient);
            _shader->setUniform(_locations["defaultMaterial.diffuse"], _diffuse);
            _shader->setUniform(_locations["defaultMaterial.emission"], _emission);
            _shader->setUniform(_locations["defaultMaterial.specular"], _specular);
            _shader->setUniform(_locations["defaultMaterial.shine"], _shine);
            _shader->setUniform(_locations["defaultMaterial.transparency"], _transparency);
            _shader->setUniform(_locations["defaultMaterial.reflectivity"], _reflectivity);
            _shader->setUniform(_locations["defaultMaterial.disableLighting"], _disableLighting);
        }

        void DefaultMaterial::primitiveBind(const PrimitiveBindData& data)
        {
            _shader->setUniform(_locations["transform.m"], data.model);
            _shader->setUniform(_locations["transform.mvp"], data.camera * data.model);
            _shader->setUniform(_locations["transform.normals"], glm::transpose(glm::inverse(glm::mat3x3(data.model))));
        }

        DJV_ENUM_HELPERS_IMPLEMENTATION(DefaultMaterialMode);

    } // namespace Render3D

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        Render3D,
        DefaultMaterialMode,
        DJV_TEXT("render3d_default_material_mode_default"),
        DJV_TEXT("render3d_default_material_mode_unlit"),
        DJV_TEXT("render3d_default_material_mode_normals"),
        DJV_TEXT("render3d_default_material_mode_uvs"));

} // namespace djv

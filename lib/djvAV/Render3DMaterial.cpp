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

#include <djvAV/Render3DMaterial.h>

#include <djvAV/OpenGLShader.h>
#include <djvAV/Render3DLight.h>
#include <djvAV/ShaderSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            void IMaterial::_init(
                const std::string& vertex,
                const std::string& fragment,
                const std::shared_ptr<Context>& context)
            {
                auto shaderSystem = context->getSystemT<AV::Render::ShaderSystem>();
                _shader = shaderSystem->getShader(vertex, fragment);
            }

            void SolidColorMaterial::_init(const std::shared_ptr<Context> & context)
            {
                IMaterial::_init("djvAVRender3DSolidColorVertex.glsl", "djvAVRender3DSolidColorFragment.glsl", context);
                auto program = _shader->getProgram();
                _modelLoc = glGetUniformLocation(program, "transform.m");
                _mvpLoc = glGetUniformLocation(program, "transform.mvp");
                _colorLoc = glGetUniformLocation(program, "color");
            }

            std::shared_ptr<SolidColorMaterial> SolidColorMaterial::create(const std::shared_ptr<Context> & context)
            {
                auto out = std::shared_ptr<SolidColorMaterial>(new SolidColorMaterial);
                out->_init(context);
                return out;
            }

            void SolidColorMaterial::primitiveBind(const PrimitiveBindData& data)
            {
                _shader->setUniform(_modelLoc, data.model);
                _shader->setUniform(_mvpLoc, data.camera * data.model);
                _shader->setUniform(_colorLoc, data.color);
            }

            void DefaultMaterial::_init(const std::shared_ptr<Context>& context)
            {
                IMaterial::_init("djvAVRender3DDefaultVertex.glsl", "djvAVRender3DDefaultFragment.glsl", context);
                auto program = _shader->getProgram();
                _modelLoc = glGetUniformLocation(program, "transform.m");
                _mvpLoc = glGetUniformLocation(program, "transform.mvp");
                _normalsLoc = glGetUniformLocation(program, "transform.normals");
                _modeLoc = glGetUniformLocation(program, "defaultMaterial.mode");
                _ambientLoc = glGetUniformLocation(program, "defaultMaterial.ambient");
                _diffuseLoc = glGetUniformLocation(program, "defaultMaterial.diffuse");
                _emissionLoc = glGetUniformLocation(program, "defaultMaterial.emission");
                _specularLoc = glGetUniformLocation(program, "defaultMaterial.specular");
                _shineLoc = glGetUniformLocation(program, "defaultMaterial.shine");
                _transparencyLoc = glGetUniformLocation(program, "defaultMaterial.transparency");
                _reflectivityLoc = glGetUniformLocation(program, "defaultMaterial.reflectivity");
                _disableLightingLoc = glGetUniformLocation(program, "defaultMaterial.disableLighting");
            }

            std::shared_ptr<DefaultMaterial> DefaultMaterial::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
                out->_init(context);
                return out;
            }

            void DefaultMaterial::bind(const BindData& data)
            {
                size_t directionalLightCount = 0;
                size_t pointLightCount = 0;
                size_t spotLightCount = 0;
                for (auto light = data.lights.begin(); light != data.lights.end(); ++light)
                {
                    if (auto hemisphereLight = std::dynamic_pointer_cast<HemisphereLight>(*light))
                    {
                        _shader->setUniform("hemisphereLight.intensity", hemisphereLight->getIntensity());
                        _shader->setUniform("hemisphereLight.up", hemisphereLight->getUp());
                        _shader->setUniformF("hemisphereLight.topColor", hemisphereLight->getTopColor());
                        _shader->setUniformF("hemisphereLight.bottomColor", hemisphereLight->getBottomColor());
                        _shader->setUniform("hemisphereLightEnabled", 1);
                    }
                    else if (auto directionalLight = std::dynamic_pointer_cast<DirectionalLight>(*light))
                    {
                        if (directionalLightCount < 16)
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
                        if (pointLightCount < 16)
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
                        if (spotLightCount < 16)
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
                _shader->setUniform("directionalLightsCount", static_cast<int>(directionalLightCount));
                _shader->setUniform("pointLightsCount", static_cast<int>(pointLightCount));
                _shader->setUniform("spotLightsCount", static_cast<int>(spotLightCount));

                _shader->setUniform(_modeLoc, static_cast<int>(_mode));
                _shader->setUniform(_ambientLoc, _ambient);
                _shader->setUniform(_diffuseLoc, _diffuse);
                _shader->setUniform(_emissionLoc, _emission);
                _shader->setUniform(_specularLoc, _specular);
                _shader->setUniform(_shineLoc, _shine);
                _shader->setUniform(_transparencyLoc, _transparency);
                _shader->setUniform(_reflectivityLoc, _reflectivity);
                _shader->setUniform(_disableLightingLoc, _disableLighting);
            }

            void DefaultMaterial::primitiveBind(const PrimitiveBindData& data)
            {
                _shader->setUniform(_modelLoc, data.model);
                _shader->setUniform(_mvpLoc, data.camera * data.model);
                _shader->setUniform(_normalsLoc, glm::transpose(glm::inverse(glm::mat3x3(data.model))));
            }

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS_IMPLEMENTATION(
        AV::Render3D,
        DefaultMaterialMode,
        DJV_TEXT("Default"),
        DJV_TEXT("Unlit"),
        DJV_TEXT("Normals"),
        DJV_TEXT("UVs"));


} // namespace djv

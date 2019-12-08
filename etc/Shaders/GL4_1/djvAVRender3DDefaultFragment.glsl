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

#version 410

struct HemisphereLight
{
    float intensity;
    vec3 up;
    vec3 topColor;
    vec3 bottomColor;
};

struct DirectionalLight
{
    float intensity;
    vec3  direction;
};

struct PointLight
{
    float intensity;
    vec3  position;
};

struct SpotLight
{
    float intensity;
    float coneAngle;
    vec3  direction;
    vec3  position;
};

vec3 getHemisphereLight(HemisphereLight light, vec3 normal)
{
    float w = 0.5 * (1.0 + dot(light.up, normal));
    return light.intensity * ((w * light.topColor) + ((1.0 - w) * light.bottomColor));
}

vec3 getDirectionalLight(DirectionalLight light, vec3 normal)
{
    float ln = max(dot(-light.direction, normal), 0.0);
    return light.intensity * vec3(ln, ln, ln);
} 

vec3 getPointLight(PointLight light, vec3 position, vec3 normal)
{
    float ln = max(dot(normalize(position - light.position), normal), 0.0);
    return light.intensity * vec3(ln, ln, ln);
} 

vec3 getSpotLight(SpotLight light, vec3 position, vec3 normal)
{
    return light.intensity * vec3(0.0, 0.0, 0.0);
}

struct DefaultMaterial
{
    vec4  ambient;
    vec4  diffuse;
    vec4  emission;
    vec4  specular;
    float shine;
    float transparency;
    float reflectivity;
    int   disableLighting;
};

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Texture;
layout(location = 2) in vec3 Normal;
layout(location = 0) out vec4 FragColor;

uniform HemisphereLight  hemisphereLight;
uniform int              hemisphereLightEnabled = 0;
uniform DirectionalLight directionalLights[16];
uniform int              directionalLightsCount = 0;
uniform PointLight       pointLights[16];
uniform int              pointLightsCount = 0;
uniform SpotLight        spotLights[16];
uniform int              spotLightsCount = 0;
uniform DefaultMaterial  defaultMaterial;

void main()
{
    //FragColor.r = Normal.x;
    //FragColor.g = Normal.y;
    //FragColor.b = Normal.z;
    //FragColor.a = 1.0;
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    if (hemisphereLightEnabled != 0)
    {
        vec3 light = getHemisphereLight(hemisphereLight, Normal);
        FragColor.r += defaultMaterial.diffuse.r * light.r;
        FragColor.g += defaultMaterial.diffuse.g * light.g;
        FragColor.b += defaultMaterial.diffuse.b * light.b;
    }
    for (int i = 0; i < directionalLightsCount; ++i)
    {
        vec3 light = getDirectionalLight(directionalLights[i], Normal);
        FragColor.r += defaultMaterial.diffuse.r * light.r;
        FragColor.g += defaultMaterial.diffuse.g * light.g;
        FragColor.b += defaultMaterial.diffuse.b * light.b;
    }
    for (int i = 0; i < pointLightsCount; ++i)
    {
        vec3 light = getPointLight(pointLights[i], Position, Normal);
        FragColor.r += defaultMaterial.diffuse.r * light.r;
        FragColor.g += defaultMaterial.diffuse.g * light.g;
        FragColor.b += defaultMaterial.diffuse.b * light.b;
    }
    for (int i = 0; i < spotLightsCount; ++i)
    {
        vec3 light = getSpotLight(spotLights[i], Position, Normal);
        FragColor.r += defaultMaterial.diffuse.r * light.r;
        FragColor.g += defaultMaterial.diffuse.g * light.g;
        FragColor.b += defaultMaterial.diffuse.b * light.b;
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

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
    int   mode;
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

// djv::AV::Render3D::DefaultMaterialMode
#define DEFAULT_MATERIAL_MODE_DEFAULT 0
#define DEFAULT_MATERIAL_MODE_UNLIT   1
#define DEFAULT_MATERIAL_MODE_NORMALS 2
#define DEFAULT_MATERIAL_MODE_UVS     3

void main()
{
    if (DEFAULT_MATERIAL_MODE_DEFAULT == defaultMaterial.mode)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        vec3 n = Normal;
        if (!gl_FrontFacing)
        {
            n = -n;
        }
        if (hemisphereLightEnabled != 0)
        {
            vec3 light = getHemisphereLight(hemisphereLight, n);
            FragColor.r += defaultMaterial.diffuse.r * light.r;
            FragColor.g += defaultMaterial.diffuse.g * light.g;
            FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < directionalLightsCount; ++i)
        {
            vec3 light = getDirectionalLight(directionalLights[i], n);
            FragColor.r += defaultMaterial.diffuse.r * light.r;
            FragColor.g += defaultMaterial.diffuse.g * light.g;
            FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < pointLightsCount; ++i)
        {
            vec3 light = getPointLight(pointLights[i], Position, n);
            FragColor.r += defaultMaterial.diffuse.r * light.r;
            FragColor.g += defaultMaterial.diffuse.g * light.g;
            FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < spotLightsCount; ++i)
        {
            vec3 light = getSpotLight(spotLights[i], Position, n);
            FragColor.r += defaultMaterial.diffuse.r * light.r;
            FragColor.g += defaultMaterial.diffuse.g * light.g;
            FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
    }
    else if (DEFAULT_MATERIAL_MODE_UNLIT == defaultMaterial.mode)
    {
        FragColor.r = defaultMaterial.diffuse.r;
        FragColor.g = defaultMaterial.diffuse.g;
        FragColor.b = defaultMaterial.diffuse.b;
        FragColor.a = 1.0;
    }
    else if (DEFAULT_MATERIAL_MODE_NORMALS == defaultMaterial.mode)
    {
        FragColor.r = Normal.x;
        FragColor.g = Normal.y;
        FragColor.b = Normal.z;
        FragColor.a = 1.0;
    }
    else if (DEFAULT_MATERIAL_MODE_UVS == defaultMaterial.mode)
    {
        FragColor.r = Texture.x;
        FragColor.g = Texture.y;
        FragColor.b = 0.0;
        FragColor.a = 1.0;
    }
}

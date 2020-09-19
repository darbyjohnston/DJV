// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#version 100

precision mediump float;

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

varying vec3 Position;
varying vec2 Texture;
varying vec3 Normal;

uniform HemisphereLight  hemisphereLight;
uniform int              hemisphereLightEnabled;
uniform DirectionalLight directionalLights[16];
uniform int              directionalLightsCount;
uniform PointLight       pointLights[16];
uniform int              pointLightsCount;
uniform SpotLight        spotLights[16];
uniform int              spotLightsCount;
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
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        vec3 n = Normal;
        if (!gl_FrontFacing)
        {
            n = -n;
        }
        if (hemisphereLightEnabled != 0)
        {
            vec3 light = getHemisphereLight(hemisphereLight, n);
            gl_FragColor.r += defaultMaterial.diffuse.r * light.r;
            gl_FragColor.g += defaultMaterial.diffuse.g * light.g;
            gl_FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < directionalLightsCount; ++i)
        {
            vec3 light = getDirectionalLight(directionalLights[i], n);
            gl_FragColor.r += defaultMaterial.diffuse.r * light.r;
            gl_FragColor.g += defaultMaterial.diffuse.g * light.g;
            gl_FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < pointLightsCount; ++i)
        {
            vec3 light = getPointLight(pointLights[i], Position, n);
            gl_FragColor.r += defaultMaterial.diffuse.r * light.r;
            gl_FragColor.g += defaultMaterial.diffuse.g * light.g;
            gl_FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
        for (int i = 0; i < spotLightsCount; ++i)
        {
            vec3 light = getSpotLight(spotLights[i], Position, n);
            gl_FragColor.r += defaultMaterial.diffuse.r * light.r;
            gl_FragColor.g += defaultMaterial.diffuse.g * light.g;
            gl_FragColor.b += defaultMaterial.diffuse.b * light.b;
        }
    }
    else if (DEFAULT_MATERIAL_MODE_UNLIT == defaultMaterial.mode)
    {
        gl_FragColor.r = defaultMaterial.diffuse.r;
        gl_FragColor.g = defaultMaterial.diffuse.g;
        gl_FragColor.b = defaultMaterial.diffuse.b;
        gl_FragColor.a = 1.0;
    }
    else if (DEFAULT_MATERIAL_MODE_NORMALS == defaultMaterial.mode)
    {
        gl_FragColor.r = Normal.x;
        gl_FragColor.g = Normal.y;
        gl_FragColor.b = Normal.z;
        gl_FragColor.a = 1.0;
    }
    else if (DEFAULT_MATERIAL_MODE_UVS == defaultMaterial.mode)
    {
        gl_FragColor.r = Texture.x;
        gl_FragColor.g = Texture.y;
        gl_FragColor.b = 0.0;
        gl_FragColor.a = 1.0;
    }
}

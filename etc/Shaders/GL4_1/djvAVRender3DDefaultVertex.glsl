// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#version 410

in vec3 aPos;
in vec2 aTexture;
in vec3 aNormal;

layout(location = 0) out vec3 Position;
layout(location = 1) out vec2 Texture;
layout(location = 2) out vec3 Normal;

uniform struct Transform
{
    mat4 m;
    mat4 mvp;
    mat3 normals;
} transform;

void main()
{
    gl_Position = transform.mvp * vec4(aPos, 1.0);
    Position = vec3(transform.m * vec4(aPos, 1.0));
    Texture = aTexture;
    Normal = vec3(transform.normals * aNormal);
}

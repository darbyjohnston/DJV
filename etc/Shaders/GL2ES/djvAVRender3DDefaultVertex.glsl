// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#version 100

attribute vec3 aPos;
attribute vec2 aTexture;
attribute vec3 aNormal;

varying vec3 Position;
varying vec2 Texture;
varying vec3 Normal;

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

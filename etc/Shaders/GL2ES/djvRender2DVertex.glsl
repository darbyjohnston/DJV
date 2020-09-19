// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 20018-2020 Darby Johnston
// All rights reserved.

#version 100

attribute vec3 aPos;
attribute vec2 aTexture;

varying vec2 Texture;

uniform struct Transform
{
    mat4 mvp;
} transform;

void main()
{
    gl_Position = transform.mvp * vec4(aPos, 1.0);
    Texture = aTexture;
}

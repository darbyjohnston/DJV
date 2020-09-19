// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 20018-2020 Darby Johnston
// All rights reserved.

#version 410

in vec3 aPos;
in vec2 aTexture;

out vec2 Texture;

uniform struct Transform
{
    mat4 mvp;
} transform;

void main()
{
    gl_Position = transform.mvp * vec4(aPos, 1.0);
    Texture = aTexture;
}

//! Copyright (c) 2017-2018 Darby Johnston
//! All rights reserved.
//
// See LICENSE.txt for licensing information.

#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexture;

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

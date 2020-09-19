// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#version 100

attribute vec3 aPos;

varying vec3 Position;

uniform struct Transform
{
    mat4 m;
    mat4 mvp;
} transform;

void main()
{
    gl_Position = transform.mvp * vec4(aPos, 1.0);
    Position = vec3(transform.m * vec4(aPos, 1.0));
}

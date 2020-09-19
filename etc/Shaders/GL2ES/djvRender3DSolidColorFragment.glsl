// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#version 100

precision mediump float;

varying vec3 Position;

uniform vec4 color;

void main()
{
    gl_FragColor = color;
}

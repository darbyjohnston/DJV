// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 20018-2020 Darby Johnston
// All rights reserved.

#version 100

precision mediump float;

varying vec2 Texture;

uniform sampler2D textureSampler;

void main()
{
    vec4 t = texture2D(textureSampler, Texture);
    gl_FragColor = t;
}

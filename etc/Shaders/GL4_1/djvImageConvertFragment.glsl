// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 20018-2020 Darby Johnston
// All rights reserved.

#version 410

in vec2 Texture;

out vec4 FragColor;

uniform sampler2D textureSampler;

void main()
{
    vec4 t = texture(textureSampler, Texture);
    FragColor = t;
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#version 410

layout(location = 0) in vec3 Position;

layout(location = 0) out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = color;
}

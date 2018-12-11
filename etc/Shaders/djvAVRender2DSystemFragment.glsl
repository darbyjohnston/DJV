//! Copyright (c) 2017-2018 Darby Johnston
//! All rights reserved.
//
// See LICENSE.txt for licensing information.

#version 400 core

out vec4 FragColor;

in vec2 Texture;

uniform int pixelFormat;
uniform int colorMode;
uniform vec4 color;
uniform sampler2D textureSampler;

void main()
{
    vec4 t = texture(textureSampler, Texture);
    switch (pixelFormat)
    {
    case 0:
        t.g = t.r;
        t.b = t.r;
        t.a = 1.0;
        break;
    case 1:
        t.a = t.g;
        t.g = t.r;
        t.b = t.r;
        break;
    case 2:
        t.a = 1.0;
        break;
    case 3:
        break;
    }

    switch (colorMode)
    {
    case 0:
        FragColor = color;
        break;
    case 1:
        FragColor.r = color.r;
        FragColor.g = color.g;
        FragColor.b = color.b;
        FragColor.a = color.a * t.x;
        break;
    case 2:
        FragColor = color * t;
        break;
    }
}

//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#version 400 core

out vec4 FragColor;

in vec2 Texture;

uniform int imageFormat;
uniform int colorMode;
uniform vec4 color;
uniform sampler2D textureSampler;

// djv::AV::Render::ImageFormat
#define IMAGE_FORMAT_L    0
#define IMAGE_FORMAT_LA   1
#define IMAGE_FORMAT_RGB  2
#define IMAGE_FORMAT_RGBA 3

// djv::AV::Render::ColorMode
#define COLOR_MODE_SOLID_COLOR                0
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA   1
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R 2
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G 3
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B 4
#define COLOR_MODE_COLOR_AND_TEXTURE          5

void main()
{
    vec4 t = texture(textureSampler, Texture);
    switch (imageFormat)
    {
    case IMAGE_FORMAT_L:
        t.g = t.r;
        t.b = t.r;
        t.a = 1.0;
        break;
    case IMAGE_FORMAT_LA:
        t.a = t.g;
        t.g = t.r;
        t.b = t.r;
        break;
    case IMAGE_FORMAT_RGB:
        t.a = 1.0;
        break;
    case IMAGE_FORMAT_RGBA:
        break;
    }
    
    switch (colorMode)
    {
    case COLOR_MODE_SOLID_COLOR:
        FragColor = color;
        break;
    case COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA:
        FragColor.r = color.r;
        FragColor.g = color.g;
        FragColor.b = color.b;
        FragColor.a = color.a * t.r;
        break;
    case COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R:
        FragColor.r = color.r;
        FragColor.g = 0.f;
        FragColor.b = 0.f;
        FragColor.a = color.a * t.r;
        break;
    case COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G:
        FragColor.r = 0.f;
        FragColor.g = color.g;
        FragColor.b = 0.f;
        FragColor.a = color.a * t.g;
        break;
    case COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B:
        FragColor.r = 0.f;
        FragColor.g = 0.f;
        FragColor.b = color.b;
        FragColor.a = color.a * t.b;
        break;
    case COLOR_MODE_COLOR_AND_TEXTURE:
        FragColor = color * t;
        break;
    }
}

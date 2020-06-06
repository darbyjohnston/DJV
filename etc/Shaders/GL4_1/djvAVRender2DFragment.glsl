// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 20018-2020 Darby Johnston
// All rights reserved.

#version 410

struct Levels
{
    float inLow;
    float inHigh;
    float gamma;
    float outLow;
    float outHigh;
};

struct Exposure
{
    float v;
    float d;
    float k;
    float f;
    float g;
};

in vec2 Texture;
out vec4 FragColor;

uniform int         imageChannels       = 0;
uniform int         colorMode           = 0;
uniform vec4        color;
uniform int         colorSpace          = 0;
uniform sampler3D   colorSpaceSampler;
uniform mat4        colorMatrix;
uniform bool        colorMatrixEnabled  = false;
uniform bool        colorInvert         = false;
uniform Levels      levels;
uniform bool        levelsEnabled       = false;
uniform Exposure    exposure;
uniform bool        exposureEnabled     = false;
uniform float       softClip            = 0.0;
uniform int         imageChannelDisplay = 0;
uniform sampler2D   textureSampler;

// djv::AV::Image::Channels
#define IMAGE_CHANNELS_L    1
#define IMAGE_CHANNELS_LA   2
#define IMAGE_CHANNELS_RGB  3
#define IMAGE_CHANNELS_RGBA 4

// djv::AV::Render::ImageChannelDisplay
#define IMAGE_CHANNEL_DISPLAY_COLOR 0
#define IMAGE_CHANNEL_DISPLAY_RED   1
#define IMAGE_CHANNEL_DISPLAY_GREEN 2
#define IMAGE_CHANNEL_DISPLAY_BLUE  3
#define IMAGE_CHANNEL_DISPLAY_ALPHA 4

// djv::AV::Render::ColorMode
#define COLOR_MODE_SOLID_COLOR                0
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA   1
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R 2
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G 3
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B 4
#define COLOR_MODE_COLOR_AND_TEXTURE          5
#define COLOR_MODE_SHADOW                     6

//$colorSpaceFunctions

vec4 colorMatrixFunc(vec4 value, mat4 color)
{
    vec4 tmp;
    tmp[0] = value[0];
    tmp[1] = value[1];
    tmp[2] = value[2];
    tmp[3] = 1.0;
    tmp *= color;
    tmp[3] = value[3];
    return tmp;
}

vec4 levelsFunc(vec4 value, Levels data)
{
    vec4 tmp;
    tmp[0] = (value[0] - data.inLow) / data.inHigh;
    tmp[1] = (value[1] - data.inLow) / data.inHigh;
    tmp[2] = (value[2] - data.inLow) / data.inHigh;
    if (tmp[0] >= 0.0)
        tmp[0] = pow(tmp[0], data.gamma);
    if (tmp[1] >= 0.0)
        tmp[1] = pow(tmp[1], data.gamma);
    if (tmp[2] >= 0.0)
        tmp[2] = pow(tmp[2], data.gamma);
    value[0] = tmp[0] * data.outHigh + data.outLow;
    value[1] = tmp[1] * data.outHigh + data.outLow;
    value[2] = tmp[2] * data.outHigh + data.outLow;
    return value;
}

vec4 softClipFunc(vec4 value, float softClip)
{
    float tmp = 1.0 - softClip;
    if (value[0] > tmp)
        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) * softClip;
    if (value[1] > tmp)
        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) * softClip;
    if (value[2] > tmp)
        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) * softClip;
    return value;
}

float knee(float value, float f)
{
    return log(value * f + 1.0) / f;
}

vec4 exposureFunc(vec4 value, Exposure data)
{
    value[0] = max(0.0, value[0] - data.d) * data.v;
    value[1] = max(0.0, value[1] - data.d) * data.v;
    value[2] = max(0.0, value[2] - data.d) * data.v;
    if (value[0] > data.k)
        value[0] = data.k + knee(value[0] - data.k, data.f);
    if (value[1] > data.k)
        value[1] = data.k + knee(value[1] - data.k, data.f);
    if (value[2] > data.k)
        value[2] = data.k + knee(value[2] - data.k, data.f);
    value[0] *= 0.332;
    value[1] *= 0.332;
    value[2] *= 0.332;
    return value;
}

void main()
{
    if (COLOR_MODE_SOLID_COLOR == colorMode)
    {
        FragColor = color;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA == colorMode)
    {
        vec4 t = texture(textureSampler, Texture);
        FragColor.r = color.r;
        FragColor.g = color.g;
        FragColor.b = color.b;
        FragColor.a = color.a * t.r;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R == colorMode)
    {
        vec4 t = texture(textureSampler, Texture);
        FragColor.r = color.r;
        FragColor.g = 0.0;
        FragColor.b = 0.0;
        FragColor.a = color.a * t.r;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G == colorMode)
    {
        vec4 t = texture(textureSampler, Texture);
        FragColor.r = 0.0;
        FragColor.g = color.g;
        FragColor.b = 0.0;
        FragColor.a = color.a * t.g;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B == colorMode)
    {
        vec4 t = texture(textureSampler, Texture);
        FragColor.r = 0.0;
        FragColor.g = 0.0;
        FragColor.b = color.b;
        FragColor.a = color.a * t.b;
    }
    else if (COLOR_MODE_COLOR_AND_TEXTURE == colorMode)
    {
        // Sample the texture.
        vec4 t = texture(textureSampler, Texture);

        // Swizzle the channels for the given image format.
        if (IMAGE_CHANNELS_L == imageChannels)
        {
            t.g = t.b = t.r;
            t.a = 1.0;
        }
        else if (IMAGE_CHANNELS_LA == imageChannels)
        {
            t.a = t.g;
            t.g = t.b = t.r;
        }
        else if (IMAGE_CHANNELS_RGB == imageChannels)
        {
            t.a = 1.0;
        }
        
		// Apply color transformations.
        if (colorMatrixEnabled)
        {
            t = colorMatrixFunc(t, colorMatrix);
        }
        if (colorInvert)
        {
            t.r = 1.0 - t.r;
            t.g = 1.0 - t.g;
            t.b = 1.0 - t.b;
        }
        if (levelsEnabled)
        {
            t = levelsFunc(t, levels);
        }
        if (exposureEnabled)
        {
            t = exposureFunc(t, exposure);
        }
        if (softClip > 0.0)
        {
            t = softClipFunc(t, softClip);
        }

		// Apply color management.
		//$colorSpaceBody

        // Swizzle the channels for the given image channel display.
        if (IMAGE_CHANNEL_DISPLAY_RED == imageChannelDisplay)
        {
            t.g = t.r;
            t.b = t.r;
        }
        else if (IMAGE_CHANNEL_DISPLAY_GREEN == imageChannelDisplay)
        {
            t.r = t.g;
            t.b = t.g;
        }
        else if (IMAGE_CHANNEL_DISPLAY_BLUE == imageChannelDisplay)
        {
            t.r = t.b;
            t.g = t.b;
        }
        else if (IMAGE_CHANNEL_DISPLAY_ALPHA == imageChannelDisplay)
        {
            t.r = t.a;
            t.g = t.a;
            t.b = t.a;
        }

        FragColor = t * color;
    }
    else if (COLOR_MODE_SHADOW == colorMode)
    {
        FragColor = color * Texture.x;
    }
}

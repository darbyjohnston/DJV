//------------------------------------------------------------------------------
// Copyright (c) 20018-2019 Darby Johnston
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

#version 100

precision mediump float;
varying vec2 Texture;

uniform int imageChannels;
uniform int imageChannel;
uniform int colorMode;
uniform vec4 color;
uniform sampler2D textureSampler;

// djv::AV::Image::Channels
#define IMAGE_CHANNELS_L    1
#define IMAGE_CHANNELS_LA   2
#define IMAGE_CHANNELS_RGB  3
#define IMAGE_CHANNELS_RGBA 4

// djv::AV::Render::ImageChannel
#define IMAGE_CHANNEL_COLOR 0
#define IMAGE_CHANNEL_RED   1
#define IMAGE_CHANNEL_GREEN 2
#define IMAGE_CHANNEL_BLUE  3
#define IMAGE_CHANNEL_ALPHA 4

// djv::AV::Render::ColorMode
#define COLOR_MODE_SOLID_COLOR                0
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA   1
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R 2
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G 3
#define COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B 4
#define COLOR_MODE_COLOR_AND_TEXTURE          5
#define COLOR_MODE_SHADOW                     6

void main()
{
    if (COLOR_MODE_SOLID_COLOR == colorMode)
    {
        gl_FragColor = color;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA == colorMode)
    {
		vec4 t = texture(textureSampler, Texture);
        gl_FragColor.r = color.r;
        gl_FragColor.g = color.g;
        gl_FragColor.b = color.b;
        gl_FragColor.a = color.a * t.r;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_R == colorMode)
    {
		vec4 t = texture(textureSampler, Texture);
        gl_FragColor.r = color.r;
        gl_FragColor.g = 0.0;
        gl_FragColor.b = 0.0;
        gl_FragColor.a = color.a * t.r;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_G == colorMode)
    {
		vec4 t = texture(textureSampler, Texture);
        gl_FragColor.r = 0.0;
        gl_FragColor.g = color.g;
        gl_FragColor.b = 0.0;
        gl_FragColor.a = color.a * t.g;
    }
    else if (COLOR_MODE_COLOR_WITH_TEXTURE_ALPHA_B == colorMode)
    {
		vec4 t = texture(textureSampler, Texture);
        gl_FragColor.r = 0.0;
        gl_FragColor.g = 0.0;
        gl_FragColor.b = color.b;
        gl_FragColor.a = color.a * t.b;
    }
    else if (COLOR_MODE_COLOR_AND_TEXTURE == colorMode)
    {
	    vec4 t = texture2D(textureSampler, Texture);
		if (IMAGE_CHANNELS_L == imageChannels)
		{
			t.g = t.r;
			t.b = t.r;
			t.a = 1.0;
		}
		else if (IMAGE_CHANNELS_LA == imageChannels)
		{
			t.a = t.g;
			t.g = t.r;
			t.b = t.r;
		}
		else if (IMAGE_CHANNELS_RGB == imageChannels)
		{
			t.a = 1.0;
		}
        gl_FragColor = color * t;
    }
	else if (COLOR_MODE_SHADOW == colorMode)
	{
        gl_FragColor = color * Texture.x;
	}
}

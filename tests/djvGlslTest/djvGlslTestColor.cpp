//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
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

//! \file djvGlslTestColor.cpp

#include <djvGlslTestColor.h>

#include <djvFloatEditSlider.h>

#include <djvOpenGlImage.h>
#include <djvMatrixUtil.h>

#include <QGroupBox>
#include <QVBoxLayout>

djvGlslTestColorOp::djvGlslTestColorOp() :
    _init(false)
{
    // Create the widgets.
    
    QGroupBox * brightnessGroup = new QGroupBox("Brightness");
    
    djvFloatEditSlider * brightness = new djvFloatEditSlider;
    brightness->setRange(0.0, 5.0);
    brightness->setDefaultValue(1.0);
    
    QGroupBox * contrastGroup = new QGroupBox("Contrast");

    djvFloatEditSlider * contrast = new djvFloatEditSlider;
    contrast->setRange(0.0, 5.0);
    contrast->setDefaultValue(1.0);
    
    QGroupBox * saturationGroup = new QGroupBox("Saturation");

    djvFloatEditSlider * saturation = new djvFloatEditSlider;
    saturation->setRange(0.0, 5.0);
    saturation->setDefaultValue(1.0);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);

    QVBoxLayout * vLayout = new QVBoxLayout(brightnessGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(brightness);
    layout->addWidget(brightnessGroup);

    vLayout = new QVBoxLayout(contrastGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(contrast);
    layout->addWidget(contrastGroup);

    vLayout = new QVBoxLayout(saturationGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(saturation);
    layout->addWidget(saturationGroup);

    layout->addStretch();

    // Initialize.

    brightness->setValue(_value.brightness);
    contrast->setValue(_value.contrast);
    saturation->setValue(_value.saturation);

    // Setup the callbacks.

    connect(
        brightness,
        SIGNAL(valueChanged(double)),
        SLOT(brightnessCallback(double)));
    
    connect(
        contrast,
        SIGNAL(valueChanged(double)),
        SLOT(contrastCallback(double)));
    
    connect(
        saturation,
        SIGNAL(valueChanged(double)),
        SLOT(saturationCallback(double)));
}

namespace
{

const QString src =
"vec4 colorFnc(vec4 value, mat4 color)\n"
"{\n"
"    vec4 tmp;\n"
"    tmp[0] = value[0];\n"
"    tmp[1] = value[1];\n"
"    tmp[2] = value[2];\n"
"    tmp[3] = 1.0;\n"
"    tmp *= color;\n"
"    tmp[3] = value[3];\n"
"    return tmp;\n"
"}\n"
"\n"
"uniform sampler2DRect texture;\n"
"uniform mat4 color;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(texture, gl_TexCoord[0].st);\n"
"    gl_FragColor = colorFnc(gl_FragColor, color);\n"
"}\n";

} // namespace

void djvGlslTestColorOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestColorOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.

    begin();

    _texture.init(in);

    if (! _init)
    {
        _shader.init(src);

        _init = true;
    }

    // Render.

    _shader.bind();

    const double b = _value.brightness;
    const double c = _value.contrast;
    const double s = _value.saturation;

    const djvMatrix4f value =
        djvOpenGlImageColor::brightnessMatrix(b, b, b) *
        djvOpenGlImageColor::contrastMatrix(c, c, c) *
        djvOpenGlImageColor::saturationMatrix(s, s, s);
    glUniformMatrix4fv(
        glGetUniformLocation(_shader.program(), "color"), 1, false,
        djvMatrixUtil::convert<double, GLfloat>(djvMatrixUtil::transpose(value)).e);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_shader.program(), "texture"), 0);
    _texture.bind();

    const djvPixelDataInfo & info = in.info();
    djvOpenGlUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    glClear(GL_COLOR_BUFFER_BIT);
    djvGlslTestUtil::quad(info);

    end();
}

void djvGlslTestColorOp::brightnessCallback(double in)
{
    _value.brightness = in;

    Q_EMIT changed();
}

void djvGlslTestColorOp::contrastCallback(double in)
{
    _value.contrast = in;

    Q_EMIT changed();
}

void djvGlslTestColorOp::saturationCallback(double in)
{
    _value.saturation = in;

    Q_EMIT changed();
}

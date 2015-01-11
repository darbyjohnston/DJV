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

//! \file djvGlslTestExposure.cpp

#include <djvGlslTestExposure.h>

#include <djvFloatEditSlider.h>

#include <djvOpenGlImage.h>

#include <QGroupBox>
#include <QVBoxLayout>

djvGlslTestExposureOp::djvGlslTestExposureOp() :
    _init(false)
{
    // Create the widgets.

    QGroupBox * exposureGroup = new QGroupBox("Exposure");
    djvFloatEditSlider * exposure = new djvFloatEditSlider;
    exposure->setRange(-10.0, 10.0);
    exposure->setDefaultValue(0.0);
    
    QGroupBox * defogGroup = new QGroupBox("Defog");
    djvFloatEditSlider * defog = new djvFloatEditSlider;
    defog->setRange(0.0, 0.01);
    defog->setDefaultValue(0.0);
    
    QGroupBox * kneeGroup = new QGroupBox("Knee");
    djvFloatEditSlider * kneeLow = new djvFloatEditSlider;
    kneeLow->setRange(-3.0, 3.0);
    kneeLow->setDefaultValue(0.0);
    
    djvFloatEditSlider * kneeHigh = new djvFloatEditSlider;
    kneeHigh->setRange(3.5, 7.5);
    kneeHigh->setDefaultValue(3.5);
    
    QGroupBox * gammaGroup = new QGroupBox("Gamma");
    djvFloatEditSlider * gamma = new djvFloatEditSlider;
    gamma->setRange(0.1, 4.0);
    gamma->setDefaultValue(1.0);

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    
    QVBoxLayout * vLayout = new QVBoxLayout(exposureGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(exposure);
    layout->addWidget(exposureGroup);

    vLayout = new QVBoxLayout(defogGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(defog);
    layout->addWidget(defogGroup);

    vLayout = new QVBoxLayout(kneeGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(kneeLow);
    vLayout->addWidget(kneeHigh);
    layout->addWidget(kneeGroup);

    vLayout = new QVBoxLayout(gammaGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(gamma);
    layout->addWidget(gammaGroup);

    layout->addStretch();

    // Initialize.

    exposure->setValue(_value.exposure);
    defog->setValue(_value.defog);
    kneeLow->setValue(_value.kneeLow);
    kneeHigh->setValue(_value.kneeHigh);
    gamma->setValue(_value.gamma);

    // Setup the callbacks.

    connect(
        exposure,
        SIGNAL(valueChanged(double)),
        SLOT(exposureCallback(double)));
    
    connect(
        defog,
        SIGNAL(valueChanged(double)),
        SLOT(defogCallback(double)));
    
    connect(
        kneeLow,
        SIGNAL(valueChanged(double)),
        SLOT(kneeLowCallback(double)));
    
    connect(
        kneeHigh,
        SIGNAL(valueChanged(double)),
        SLOT(kneeHighCallback(double)));
    
    connect(
        gamma,
        SIGNAL(valueChanged(double)),
        SLOT(gammaCallback(double)));
}

namespace
{

const QString src =
"struct Exposure { float v, d, k, f, g; };\n""\n"
"\n"
"float knee(float value, float f)\n"
"{\n"
"    return log(value * f + 1.0) / f;\n"
"}\n"
"\n"
"vec4 exposureFnc(vec4 value, Exposure exposure)\n"
"{\n"
"    value[0] = max(0.0, value[0] - exposure.d) * exposure.v;\n"
"    value[1] = max(0.0, value[1] - exposure.d) * exposure.v;\n"
"    value[2] = max(0.0, value[2] - exposure.d) * exposure.v;\n"
"\n"
"    if (value[0] > exposure.k)"
"        value[0] = exposure.k + knee(value[0] - exposure.k, exposure.f);\n"
"    if (value[1] > exposure.k)"
"        value[1] = exposure.k + knee(value[1] - exposure.k, exposure.f);\n"
"    if (value[2] > exposure.k)"
"        value[2] = exposure.k + knee(value[2] - exposure.k, exposure.f);\n"
"\n"
"    value[0] *= 0.332;\n"
"    value[1] *= 0.332;\n"
"    value[2] *= 0.332;\n"
"\n"
"    value[0] = pow(value[0], exposure.g);\n"
"    value[1] = pow(value[1], exposure.g);\n"
"    value[2] = pow(value[2], exposure.g);\n"
"\n"
"    return value;\n"
"}\n"
"\n"
"uniform sampler2DRect texture;\n"
"uniform Exposure      exposure;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(texture, gl_TexCoord[0].st);\n"
"    gl_FragColor = exposureFnc(gl_FragColor, exposure);\n"
"}\n";

double knee(double x, double f)
{
    return djvMath::log(x * f + 1.0) / f;
}

double kneeF(double x, double y)
{
    double f0 = 0.0, f1 = 1.0;

    while (knee(x, f1) > y)
    {
        f0 = f1;
        f1 = f1 * 2.0;
    }

    for (int i = 0; i < 30; ++i)
    {
        const double f2 = (f0 + f1) / 2.0;

        if (knee(x, f2) < y)
        {
            f1 = f2;
        }
        else
        {
            f0 = f2;
        }
    }

    return (f0 + f1) / 2.0;
}

} // namespace

void djvGlslTestExposureOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestExposureOp::render");
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
    const GLhandleARB program = _shader.program();

    const float v =
        static_cast<float>(djvMath::pow(2.0, _value.exposure + 2.47393));
    const float d = static_cast<float>(_value.defog);
    const float k = static_cast<float>(djvMath::pow(2.0, _value.kneeLow));
    const float f = static_cast<float>(
        kneeF(djvMath::pow(2.0, _value.kneeHigh) - k, djvMath::pow(2.0, 3.5) - k));
    const float g = static_cast<float>(1.0 / _value.gamma);
    //DJV_DEBUG_PRINT("v = " << v);
    //DJV_DEBUG_PRINT("d = " << d);
    //DJV_DEBUG_PRINT("k = " << k);
    //DJV_DEBUG_PRINT("f = " << f);
    //DJV_DEBUG_PRINT("g = " << g);
    glUniform1f(glGetUniformLocation(program, "exposure.v"), v);
    glUniform1f(glGetUniformLocation(program, "exposure.d"), d);
    glUniform1f(glGetUniformLocation(program, "exposure.k"), k);
    glUniform1f(glGetUniformLocation(program, "exposure.f"), f);
    glUniform1f(glGetUniformLocation(program, "exposure.g"), g);

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

void djvGlslTestExposureOp::exposureCallback(double in)
{
    _value.exposure = in;

    Q_EMIT changed();
}

void djvGlslTestExposureOp::defogCallback(double in)
{
    _value.defog = in;

    Q_EMIT changed();
}

void djvGlslTestExposureOp::kneeLowCallback(double in)
{
    _value.kneeLow = in;

    Q_EMIT changed();
}

void djvGlslTestExposureOp::kneeHighCallback(double in)
{
    _value.kneeHigh = in;

    Q_EMIT changed();
}

void djvGlslTestExposureOp::gammaCallback(double in)
{
    _value.gamma = in;

    Q_EMIT changed();
}

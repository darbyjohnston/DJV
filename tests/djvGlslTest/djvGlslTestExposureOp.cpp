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

#include <djvGlslTestExposureOp.h>

#include <djvGlslTestContext.h>

#include <djvFloatEditSlider.h>

#include <djvImage.h>
#include <djvOpenGlImage.h>

#include <QGroupBox>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// djvGlslTestExposureOp
//------------------------------------------------------------------------------

djvGlslTestExposureOp::djvGlslTestExposureOp(djvGlslTestContext * context) :
    djvGlslTestAbstractOp(context),
    _init(false)
{}

const djvGlslTestExposureOp::Values & djvGlslTestExposureOp::values() const
{
    return _values;
}

namespace
{
const QString vertexSource =
"void main(void)\n"
"{\n"
"    gl_FrontColor  = gl_Color;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const QString fragmentSource =
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

float knee(float x, float f)
{
    return djvMath::log(x * f + 1.f) / f;
}

float kneeF(float x, float y)
{
    float f0 = 0.f, f1 = 1.f;
    while (knee(x, f1) > y)
    {
        f0 = f1;
        f1 = f1 * 2.f;
    }
    for (int i = 0; i < 30; ++i)
    {
        const float f2 = (f0 + f1) / 2.f;
        if (knee(x, f2) < y)
        {
            f1 = f2;
        }
        else
        {
            f0 = f2;
        }
    }
    return (f0 + f1) / 2.f;
}

} // namespace

void djvGlslTestExposureOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestExposureOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.
    begin();
    _texture.init(in, GL_TEXTURE_RECTANGLE);
    if (! _init)
    {
        _shader.init(vertexSource, fragmentSource);
        _init = true;
    }

    // Render.
    _shader.bind();
    const GLuint program = _shader.program();
    const float v =
        static_cast<float>(djvMath::pow(2.f, _values.exposure + 2.47393f));
    const float d = static_cast<float>(_values.defog);
    const float k = static_cast<float>(djvMath::pow(2.f, _values.kneeLow));
    const float f = static_cast<float>(
        kneeF(djvMath::pow(2.f, _values.kneeHigh) - k, djvMath::pow(2.f, 3.5f) - k));
    const float g = static_cast<float>(1.f / _values.gamma);
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

void djvGlslTestExposureOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// djvGlslTestExposureOpWidget
//------------------------------------------------------------------------------

djvGlslTestExposureOpWidget::djvGlslTestExposureOpWidget(
    djvGlslTestExposureOp * op,
    djvGlslTestContext *    context) :
    djvGlslTestAbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * exposureGroup = new QGroupBox("Exposure");
    djvFloatEditSlider * exposure = new djvFloatEditSlider(context);
    exposure->setRange(-10.f, 10.f);
    exposure->setDefaultValue(0.f);
    
    QGroupBox * defogGroup = new QGroupBox("Defog");
    djvFloatEditSlider * defog = new djvFloatEditSlider(context);
    defog->setRange(0.f, .01f);
    defog->setDefaultValue(0.f);
    
    QGroupBox * kneeGroup = new QGroupBox("Knee");
    djvFloatEditSlider * kneeLow = new djvFloatEditSlider(context);
    kneeLow->setRange(-3.f, 3.f);
    kneeLow->setDefaultValue(0.f);
    
    djvFloatEditSlider * kneeHigh = new djvFloatEditSlider(context);
    kneeHigh->setRange(3.5f, 7.5f);
    kneeHigh->setDefaultValue(3.5f);
    
    QGroupBox * gammaGroup = new QGroupBox("Gamma");
    djvFloatEditSlider * gamma = new djvFloatEditSlider(context);
    gamma->setRange(.1f, 4.f);
    gamma->setDefaultValue(1.f);

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
    exposure->setValue(op->values().exposure);
    defog->setValue(op->values().defog);
    kneeLow->setValue(op->values().kneeLow);
    kneeHigh->setValue(op->values().kneeHigh);
    gamma->setValue(op->values().gamma);

    // Setup the callbacks.
    connect(
        exposure,
        SIGNAL(valueChanged(float)),
        SLOT(exposureCallback(float)));
    connect(
        defog,
        SIGNAL(valueChanged(float)),
        SLOT(defogCallback(float)));
    connect(
        kneeLow,
        SIGNAL(valueChanged(float)),
        SLOT(kneeLowCallback(float)));
    connect(
        kneeHigh,
        SIGNAL(valueChanged(float)),
        SLOT(kneeHighCallback(float)));
    connect(
        gamma,
        SIGNAL(valueChanged(float)),
        SLOT(gammaCallback(float)));
}

void djvGlslTestExposureOpWidget::exposureCallback(float in)
{
    djvGlslTestExposureOp::Values values = _op->values();
    values.exposure = in;
    _op->setValues(values);
}

void djvGlslTestExposureOpWidget::defogCallback(float in)
{
    djvGlslTestExposureOp::Values values = _op->values();
    values.defog = in;
    _op->setValues(values);
}

void djvGlslTestExposureOpWidget::kneeLowCallback(float in)
{
    djvGlslTestExposureOp::Values values = _op->values();
    values.kneeLow = in;
    _op->setValues(values);
}

void djvGlslTestExposureOpWidget::kneeHighCallback(float in)
{
    djvGlslTestExposureOp::Values values = _op->values();
    values.kneeHigh = in;
    _op->setValues(values);
}

void djvGlslTestExposureOpWidget::gammaCallback(float in)
{
    djvGlslTestExposureOp::Values values = _op->values();
    values.gamma = in;
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// djvGlslTestExposureOpFactory
//------------------------------------------------------------------------------

djvGlslTestExposureOpFactory::djvGlslTestExposureOpFactory(
    djvGlslTestContext * context) :
    djvGlslTestAbstractOpFactory(context)
{}

djvGlslTestAbstractOp * djvGlslTestExposureOpFactory::createOp() const
{
    return new djvGlslTestExposureOp(context());
}
    
djvGlslTestAbstractOpWidget * djvGlslTestExposureOpFactory::createWidget(
    djvGlslTestAbstractOp * op) const
{
    return new djvGlslTestExposureOpWidget(
        dynamic_cast<djvGlslTestExposureOp *>(op), context());
}


//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <LevelsOp.h>

#include <Context.h>

#include <djvUI/FloatEditSlider.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>

#include <QGroupBox>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// LevelsOp::State
//------------------------------------------------------------------------------

LevelsOp::State::State() :
    _init(false)
{}

LevelsOp::State::State(const Values & in) :
    _softClip(in.softClip),
    _init    (true)
{}

bool LevelsOp::State::operator != (const State & in) const
{
    return
        _softClip != in._softClip ||
        ! _init;
}

//------------------------------------------------------------------------------
// LevelsOp
//------------------------------------------------------------------------------

LevelsOp::LevelsOp(Context * context) :
    AbstractOp(context)
{}

const LevelsOp::Values & LevelsOp::values() const
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

const QString levelsSource =
"    gl_FragColor = levelsFnc(\n"
"        texture2DRect(texture, gl_TexCoord[0].st), levels);\n";

const QString levelsSoftClipSource =
"    gl_FragColor = softClipFnc(levelsFnc(\n"
"        texture2DRect(texture, gl_TexCoord[0].st), levels), softClip);\n";

const QString fragmentSource =
"struct Levels\n"
"{\n"
"    float inputMin, inputMax;\n"
"    float gamma;\n"
"    float outputMin, outputMax;\n"
"};\n"
"\n"
"vec4 levelsFnc(vec4 value, Levels levels)\n"
"{\n"
"    value[0] = "
"        pow(max(value[0] - levels.inputMin, 0.0) / levels.inputMax, levels.gamma) * "
"            levels.outputMax + levels.outputMin;\n"
"    value[1] = "
"        pow(max(value[1] - levels.inputMin, 0.0) / levels.inputMax, levels.gamma) * "
"            levels.outputMax + levels.outputMin;\n"
"    value[2] = "
"        pow(max(value[2] - levels.inputMin, 0.0) / levels.inputMax, levels.gamma) * "
"            levels.outputMax + levels.outputMin;\n"
"    return value;\n"
"}\n"
"\n"
"vec4 softClipFnc(vec4 value, float softClip)\n"
"{\n"
"    float tmp = 1.0 - softClip;\n"
"    if (value[0] > tmp)\n"
"        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) * softClip;\n"
"    if (value[1] > tmp)\n"
"        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) * softClip;\n"
"    if (value[2] > tmp)\n"
"        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) * softClip;\n"
"    return value;\n"
"}\n"
"\n"
"uniform sampler2DRect texture;\n"
"uniform Levels levels;\n"
"uniform float softClip;\n"
"\n"
"void main(void)\n"
"{\n"
"    %1\n"
"\n"
"}\n";

} // namespace

void LevelsOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("LevelsOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.
    begin();
    _texture.init(in, GL_TEXTURE_RECTANGLE);
    const State state(_values);
    if (_state != state)
    {
        //DJV_DEBUG_PRINT("init");
        _render.shader.init(
            vertexSource,
            QString(fragmentSource).
                arg(_values.softClip > 0.f ? levelsSoftClipSource : levelsSource));
        _state = state;
    }

    // Render.
    _render.shader.bind();
    const GLuint program = _render.shader.program();
    glUniform1f(glGetUniformLocation(program, "levels.inputMin"),
        static_cast<GLfloat>(_values.inputMin));
    glUniform1f(glGetUniformLocation(program, "levels.inputMax"),
        static_cast<GLfloat>(_values.inputMax - _values.inputMin));
    glUniform1f(glGetUniformLocation(program, "levels.gamma"),
        static_cast<GLfloat>(1.f / _values.gamma));
    glUniform1f(glGetUniformLocation(program, "levels.outputMin"),
        static_cast<GLfloat>(_values.outputMin));
    glUniform1f(glGetUniformLocation(program, "levels.outputMax"),
        static_cast<GLfloat>(_values.outputMax - _values.outputMin));
    glUniform1f(glGetUniformLocation(program, "softClip"),
        static_cast<GLfloat>(_values.softClip));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program, "texture"), 0);
    _texture.bind();
    const djvPixelDataInfo & info = in.info();
    djvOpenGLUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    glClear(GL_COLOR_BUFFER_BIT);
    Util::quad(info);
    end();
}

void LevelsOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// LevelsOpWidget
//------------------------------------------------------------------------------

LevelsOpWidget::LevelsOpWidget(
    LevelsOp * op,
    Context *  context) :
    AbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * inputGroup = new QGroupBox("Input");
    djvFloatEditSlider * inputMin = new djvFloatEditSlider(context);
    inputMin->setRange(0.f, 1.f);
    inputMin->setDefaultValue(0.f);
    djvFloatEditSlider * inputMax = new djvFloatEditSlider(context);
    inputMax->setRange(0.f, 1.f);
    inputMax->setDefaultValue(1.f);

    QGroupBox * gammaGroup = new QGroupBox("Gamma");
    djvFloatEditSlider * gamma = new djvFloatEditSlider(context);
    gamma->setRange(0.f, 4.f);
    gamma->setDefaultValue(1.f);

    QGroupBox * outputGroup = new QGroupBox("Output");
    djvFloatEditSlider * outputMin = new djvFloatEditSlider(context);
    outputMin->setRange(0.f, 1.f);
    outputMin->setDefaultValue(0.f);
    djvFloatEditSlider * outputMax = new djvFloatEditSlider(context);
    outputMax->setRange(0.f, 1.f);
    outputMax->setDefaultValue(1.f);

    QGroupBox * softClipGroup = new QGroupBox("Soft Clip");
    djvFloatEditSlider * softClip = new djvFloatEditSlider(context);
    softClip->setRange(0.f, 1.f);
    softClip->setDefaultValue(0.f);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    QVBoxLayout * vLayout = new QVBoxLayout(inputGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(inputMin);
    vLayout->addWidget(inputMax);
    layout->addWidget(inputGroup);

    vLayout = new QVBoxLayout(gammaGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(gamma);
    layout->addWidget(gammaGroup);

    vLayout = new QVBoxLayout(outputGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(outputMin);
    vLayout->addWidget(outputMax);
    layout->addWidget(outputGroup);

    vLayout = new QVBoxLayout(softClipGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(softClip);
    layout->addWidget(softClipGroup);

    layout->addStretch();

    // Initialize.
    inputMin->setValue(op->values().inputMin);
    inputMax->setValue(op->values().inputMax);
    gamma->setValue(op->values().gamma);
    outputMin->setValue(op->values().outputMin);
    outputMax->setValue(op->values().outputMax);
    softClip->setValue(op->values().softClip);

    // Setup the callbacks.
    connect(
        inputMin,
        SIGNAL(valueChanged(float)),
        SLOT(inputMinCallback(float)));
    connect(
        inputMax,
        SIGNAL(valueChanged(float)),
        SLOT(inputMaxCallback(float)));
    connect(
        gamma,
        SIGNAL(valueChanged(float)),
        SLOT(gammaCallback(float)));
    connect(
        outputMin,
        SIGNAL(valueChanged(float)),
        SLOT(outputMinCallback(float)));
    connect(
        outputMax,
        SIGNAL(valueChanged(float)),
        SLOT(outputMaxCallback(float)));
    connect(
        softClip,
        SIGNAL(valueChanged(float)),
        SLOT(softClipCallback(float)));
}

void LevelsOpWidget::inputMinCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.inputMin = in;
    _op->setValues(values);
}

void LevelsOpWidget::inputMaxCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.inputMax = in;
    _op->setValues(values);
}

void LevelsOpWidget::gammaCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.gamma = in;
    _op->setValues(values);
}

void LevelsOpWidget::outputMinCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.outputMin = in;
    _op->setValues(values);
}

void LevelsOpWidget::outputMaxCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.outputMax = in;
    _op->setValues(values);
}

void LevelsOpWidget::softClipCallback(float in)
{
    LevelsOp::Values values = _op->values();
    values.softClip = in;
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// LevelsOpFactory
//------------------------------------------------------------------------------

LevelsOpFactory::LevelsOpFactory(Context * context) :
    AbstractOpFactory(context)
{}

AbstractOp * LevelsOpFactory::createOp() const
{
    return new LevelsOp(context());
}

AbstractOpWidget * LevelsOpFactory::createWidget(AbstractOp * op) const
{
    return new LevelsOpWidget(dynamic_cast<LevelsOp *>(op), context());
}



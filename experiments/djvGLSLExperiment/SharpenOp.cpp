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

#include <SharpenOp.h>

#include <Context.h>

#include <djvUI/FloatEditSlider.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/OpenGLImage.h>

#include <QGroupBox>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// SharpenOp
//------------------------------------------------------------------------------

SharpenOp::SharpenOp(Context * context) :
    AbstractOp(context)
{}

const SharpenOp::Values & SharpenOp::values() const
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
"%1"
"\n"
"uniform sampler2DRect texture;\n"
"uniform float value;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(texture, gl_TexCoord[0].st);\n"
"    gl_FragColor += value * kernel(texture);\n"
"}\n";

} // namespace

void SharpenOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("SharpenOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.
    begin();
    _texture.init(in, GL_TEXTURE_RECTANGLE);
    if (! _init)
    {
        //DJV_DEBUG_PRINT("init");
        Kernel kernel;
        kernel.init(9);
        _shader.init(
            vertexSource,
            QString(fragmentSource).arg(kernel.src()));
        _shader.bind();
        const float value [] =
        {
             0.f, -1.f,  0.f,
            -1.f,  4.f, -1.f,
             0.f, -1.f,  0.f
        };
        const float offset [] =
        {
            -1.f, -1.f,  0.f, -1.f,  1.f, -1.f,
            -1.f,  0.f,  0.f,  0.f,  1.f,  0.f,
            -1.f,  1.f,  0.f,  1.f,  1.f,  1.f
        };
        kernel.value(_shader.program(), value);
        kernel.offset(_shader.program(), offset);
        _init = true;
    }

    // Render.
    _shader.bind();
    glUniform1f(glGetUniformLocation(_shader.program(), "value"),
        static_cast<GLfloat>(_values.value));
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_shader.program(), "texture"), 0);
    _texture.bind();
    const djvPixelDataInfo & info = in.info();
    djvOpenGLUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    glClear(GL_COLOR_BUFFER_BIT);
    Util::quad(in.info());

    end();
}

void SharpenOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// SharpenOpWidget
//------------------------------------------------------------------------------

SharpenOpWidget::SharpenOpWidget(
    SharpenOp * op,
    Context *   context) :
    AbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * valueGroup = new QGroupBox("Value");
    djvFloatEditSlider * value = new djvFloatEditSlider(context);
    value->setRange(0.f, 2.f);
    value->setDefaultValue(1.f);

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);
    
    QVBoxLayout * vLayout = new QVBoxLayout(valueGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(value);
    layout->addWidget(valueGroup);

    layout->addStretch();

    // Initialize.
    value->setValue(op->values().value);

    // Setup the callbacks.
    connect(
        value,
        SIGNAL(valueChanged(float)),
        SLOT(valueCallback(float)));
}

void SharpenOpWidget::valueCallback(float in)
{
    SharpenOp::Values values = _op->values();
    values.value = in;
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// SharpenOpFactory
//------------------------------------------------------------------------------

SharpenOpFactory::SharpenOpFactory(
    Context * context) :
    AbstractOpFactory(context)
{}

AbstractOp * SharpenOpFactory::createOp() const
{
    return new SharpenOp(context());
}
    
AbstractOpWidget * SharpenOpFactory::createWidget(AbstractOp * op) const
{
    return new SharpenOpWidget(dynamic_cast<SharpenOp *>(op), context());
}

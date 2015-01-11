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

//! \file djvGlslTestBlur.cpp

#include <djvGlslTestBlur.h>

#include <djvIntEditSlider.h>

#include <djvOpenGlImage.h>

#include <QComboBox>
#include <QGroupBox>
#include <QVBoxLayout>

const QStringList & djvGlslTestBlurOp::filterLabels()
{
    static const QStringList data = QStringList() <<
        "Box" <<
        "Gaussian";

    return data;
}

djvGlslTestBlurOp::State::State() :
    _init(false)
{}

djvGlslTestBlurOp::State::State(const Value & in) :
    _value(in),
    _init (true)
{}

bool djvGlslTestBlurOp::State::operator != (const State & in) const
{
    return
        _value.radius != in._value.radius ||
        _value.filter != in._value.filter ||
        ! _init;
}

djvGlslTestBlurOp::djvGlslTestBlurOp()
{
    // Create the widgets.

    QGroupBox * radiusGroup = new QGroupBox("Radius");
    
    djvIntEditSlider * radius = new djvIntEditSlider;
    radius->setRange(0, 127);
    radius->setDefaultValue(0);

    QGroupBox * filterGroup = new QGroupBox("Filter");
    
    QComboBox * filter = new QComboBox;
    filter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filter->addItems(filterLabels());

    // Layout the widgets.

    QVBoxLayout * layout = new QVBoxLayout(this);
    
    QVBoxLayout * vLayout = new QVBoxLayout(radiusGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(radius);
    layout->addWidget(radiusGroup);
    
    vLayout = new QVBoxLayout(filterGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(filter);
    layout->addWidget(filterGroup);
    
    layout->addStretch();

    // Initialize.

    radius->setValue(_value.radius);
    
    filter->setCurrentIndex(_value.filter);

    // Setup the callbacks.

    connect(radius, SIGNAL(valueChanged(int)), SLOT(radiusCallback(int)));
    
    connect(filter, SIGNAL(currentIndexChanged(int)), SLOT(filterCallback(int)));
}

namespace
{

const QString src =
"%1"
"\n"
"uniform sampler2DRect texture;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = kernel(texture);\n"
"}\n";

} // namespace

void djvGlslTestBlurOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestBlurOp::render");
    //DJV_DEBUG_PRINT("in = " << in);

    // Initialize.

    begin();

    _texture.init(in);
    _render.textureTmp.init(djvPixelDataInfo(in.size(), in.pixel()));

    const int size = _value.radius * 2 + 1;

    const State state(_value);

    if (_state != state)
    {
        _render.offscreen.init();
        _render.kernel.init(size);
        _render.shader.init(QString(src).arg(_render.kernel.src()));
        _render.shader.bind();

        // Kernel weights.

        djvMemoryBuffer<float> value(size);
        float * p = value();

        switch (_value.filter)
        {
            case BOX:
                for (int i = 0; i < size; ++i)
                {
                    p[i] = 1;
                }

                break;

            case GAUSSIAN:
            {
                const double theta = size / 6.0;
                double x = -_value.radius;

                for (int i = 0; i < size; ++i, x += 1.0)
                {
                    p[i] = static_cast<float>(
                        djvMath::sqrt(1.0 / (djvMath::sqrt(2.0 * djvMath::pi * theta))) *
                        djvMath::exp(-(x * x) / (2 * theta * theta)));
                }
            }
            break;
        }

        float sum = 0.0;

        for (int i = 0; i < size; ++i)
        {
            sum += p[i];
        }

        for (int i = 0; i < size; ++i)
        {
            //DJV_DEBUG_PRINT("i = " << p[i]);
            p[i] = p[i] / sum;
        }

        _render.kernel.value(_render.shader.program(), value());

        _state = state;
    }

    // Horizontal.

    _render.offscreen.bind();
    _render.offscreen.set(_render.textureTmp);

    _render.shader.bind();
    const GLuint progam = _render.shader.program();

    djvMemoryBuffer<float> offset(size * 2);
    offset.zero();
    float * p = offset();

    for (int i = -_value.radius; i <= _value.radius; ++i, p += 2)
    {
        p[0] = static_cast<float>(i);
    }

    _render.kernel.offset(progam, offset());

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(progam, "texture"), 0);
    _texture.bind();

    const djvPixelDataInfo & info = in.info();
    djvOpenGlUtil::ortho(info.size);
    glViewport(0, 0, info.size.x, info.size.y);
    djvGlslTestUtil::quad(info);

    _render.offscreen.unbind();

    // Vertical.

    offset.zero();
    p = offset();

    for (int i = -_value.radius; i <= _value.radius; ++i, p += 2)
    {
        p[1] = static_cast<float>(i);
    }

    _render.kernel.offset(progam, offset());

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(progam, "texture"), 0);
    _render.textureTmp.bind();

    glClear(GL_COLOR_BUFFER_BIT);
    djvGlslTestUtil::quad(djvPixelDataInfo(info.size, info.pixel));

    end();
}

void djvGlslTestBlurOp::radiusCallback(int in)
{
    _value.radius = in;

    Q_EMIT changed();
}

void djvGlslTestBlurOp::filterCallback(int in)
{
    _value.filter = FILTER(in);

    Q_EMIT changed();
}

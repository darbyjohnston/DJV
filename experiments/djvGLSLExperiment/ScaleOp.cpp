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

#include <ScaleOp.h>

#include <Context.h>

#include <djvUI/IntEditSlider.h>

#include <djvGraphics/Image.h>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace gl;

//------------------------------------------------------------------------------
// ScaleOp::Values
//------------------------------------------------------------------------------

ScaleOp::Values::Values() :
    size      (500, 500),
    type      (CUSTOM),
    defaultMin(LINEAR),
    defaultMag(LINEAR),
    customMin (LANCZOS3),
    customMag (MITCHELL)
{}

bool ScaleOp::Values::operator == (const Values & other) const
{
    return
        other.size       == size       &&
        other.type       == type       &&
        other.defaultMin == defaultMin &&
        other.defaultMag == defaultMag &&
        other.customMin  == customMin  &&
        other.customMag  == customMag;
}

//------------------------------------------------------------------------------
// ScaleOp::StateDefault
//------------------------------------------------------------------------------

ScaleOp::StateDefault::StateDefault() :
    _init(false)
{}

ScaleOp::StateDefault::StateDefault(const Values & in) :
    _filterMin(in.defaultMin),
    _filterMag(in.defaultMag),
    _init(true)
{}

bool ScaleOp::StateDefault::operator != (const StateDefault & in)
    const
{
    return
        _filterMin != in._filterMin ||
        _filterMag != in._filterMag ||
        ! _init;
}

//------------------------------------------------------------------------------
// ScaleOp::StateCustom
//------------------------------------------------------------------------------

ScaleOp::StateCustom::StateCustom() :
    _init(false)
{}

ScaleOp::StateCustom::StateCustom(const Values & in) :
    _size(in.size),
    _filterMin(in.customMin),
    _filterMag(in.customMag),
    _init(true)
{}

bool ScaleOp::StateCustom::operator != (const StateCustom & in) const
{
    return
        _size      != in._size      ||
        _filterMin != in._filterMin ||
        _filterMag != in._filterMag ||
        ! _init;
}

//------------------------------------------------------------------------------
// ScaleOp::StateDefault
//------------------------------------------------------------------------------

const QStringList & ScaleOp::typeLabels()
{
    static const QStringList data = QStringList() <<
        "Default" <<
        "Custom";
    return data;
}

const QStringList & ScaleOp::filterDefaultLabels()
{
    static const QStringList data = QStringList() <<
        "Nearest" <<
        "Linear";
    return data;
}

const QStringList & ScaleOp::filterCustomLabels()
{
    static const QStringList data = QStringList() <<
        "Box" <<
        "Triangle" <<
        "Bell" <<
        "B-Spline" <<
        "Lanczos3" <<
        "Cubic" <<
        "Mitchell";
    return data;
}

//------------------------------------------------------------------------------
// ScaleOp
//------------------------------------------------------------------------------

int ScaleOp::edge(int in, int size)
{
    return djvMath::clamp(in, 0, size - 1);
}

GLenum ScaleOp::filterToGl(FILTER_DEFAULT in)
{
    switch (in)
    {
        case NEAREST: return GL_NEAREST;
        case LINEAR:  return GL_LINEAR;
    }
    return GL_NONE;
}

ScaleOp::ScaleOp(Context * context) :
    AbstractOp(context)
{}

const ScaleOp::Values & ScaleOp::values() const
{
    return _values;
}

namespace
{
typedef float (FilterFnc)(const float t);

const float supportBox = .5f;

float filterBox(float t)
{
    if (t > -.5f && t <= .5f)
    {
        return 1.f;
    }
    return 0.f;
}

const float supportTriangle = 1.f;

float filterTriangle(float t)
{
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < 1.f)
    {
        return 1.f - t;
    }
    return 0.f;
}

const float supportBell = 1.5f;

float filterBell(float t)
{
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < .5f)
    {
        return .75f - t * t;
    }
    if (t < 1.5f)
    {
        t = t - 1.5f;
        
        return .5f * t * t;
    }
    return 0.f;
}

const float supportBspline = 2.f;

float filterBspline(float t)
{
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < 1.f)
    {
        const float tt = t * t;
        
        return (.5f * tt * t) - tt + 2.f / 3.f;
    }
    else if (t < 2.f)
    {
        t = 2.f - t;
        
        return (1.f / 6.f) * (t * t * t);
    }
    return 0.f;
}

float sinc(float x)
{
    x *= djvMath::pi;
    if (x != 0.f)
    {
        return djvMath::sin(x) / x;
    }
    return 1.f;
}

const float supportLanczos3 = 3.f;

float filterLanczos3(float t)
{
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < 3.f)
    {
        return sinc(t) * sinc(t / 3.f);
    }
    return 0.f;
}

const float supportCubic = 1.f;

float filterCubic(float t)
{
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < 1.f)
    {
        return (2.f * t - 3.f) * t * t + 1.f;
    }
    return 0.f;
}

const float supportMitchell = 2.f;

float filterMitchell(float t)
{
    const float        tt = t * t;
    static const float b  = 1.f / 3.f;
    static const float c  = 1.f / 3.f;
    if (t < 0.f)
    {
        t = -t;
    }
    if (t < 1.f)
    {
        t = ((12.f - 9.f * b - 6.f * c) * (t * tt)) +
            ((-18.f + 12.f * b + 6.f * c) * tt) +
            (6.f - 2.f * b);
        return t / 6.f;
    }
    else if (t < 2.f)
    {
        t = ((-1.f * b - 6.f * c) * (t * tt)) +
            ((6.f * b + 30.f * c) * tt) +
            ((-12.f * b - 48.f * c) * t) +
            (8.f * b + 24.f * c);
        return t / 6.f;
    }

    return 0.f;
}

FilterFnc * filterFnc(ScaleOp::FILTER_CUSTOM in)
{
    static FilterFnc * tmp [] =
    {
        filterBox,
        filterTriangle,
        filterBell,
        filterBspline,
        filterLanczos3,
        filterCubic,
        filterMitchell
    };
    return tmp[in];
}

float filterSupport(ScaleOp::FILTER_CUSTOM in)
{
    static const float tmp [] =
    {
        supportBox,
        supportTriangle,
        supportBell,
        supportBspline,
        supportLanczos3,
        supportCubic,
        supportMitchell
    };
    return tmp[in];
}

void contribFnc(
    int input,
    int output,
    ScaleOp::FILTER_CUSTOM filterMin,
    ScaleOp::FILTER_CUSTOM filterMag,
    djvPixelData * out)
{
    //DJV_DEBUG("_contrib");
    //DJV_DEBUG_PRINT("scale = " << input << " " << output);
    //DJV_DEBUG_PRINT("filter = " << filterMin << " " << filterMag);

    // Filter function.
    ScaleOp::FILTER_CUSTOM filter =
        input < output ? filterMag : filterMin;
    FilterFnc * fnc = filterFnc(filter);
    const float support = filterSupport(filter);
    //DJV_DEBUG_PRINT("support = " << support);

    //! \todo Edges?
    //const float scale =
    //  static_cast<float>(output) / static_cast<float>(input);
    const float scale =
        (output > 1 ? (output - 1) : 1) /
        static_cast<float>(input > 1 ? (input - 1) : 1);
    //DJV_DEBUG_PRINT("scale = " << scale);
    const float radius = support * ((scale >= 1.f) ? 1.f : (1.f / scale));
    //DJV_DEBUG_PRINT("radius = " << radius);
    const int width = djvMath::ceil(radius * 2 + 1);
    //DJV_DEBUG_PRINT("width = " << width);
    out->set(djvPixelDataInfo(output, width, djvPixel::LA_F32));

    for (int i = 0; i < output; ++i)
    {
        const float center = i / scale;
        int left  = djvMath::ceil (center - radius);
        int right = djvMath::floor(center + radius);
        //DJV_DEBUG_PRINT(i << " = " << left << " " << center << " " << right);

        float sum   = 0.f;
        int   pixel = 0;
        int   j     = 0;
        for (int k = left; j < width && k <= right; ++j, ++k)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(out->data(i, j));
            pixel = ScaleOp::edge(k, input);
            const float x = static_cast<float>(
                (center - k) * (scale < 1.f ? scale : 1.f));
            const float w = static_cast<float>(
                (scale < 1.f) ? ((*fnc)(x) * scale) : (*fnc)(x));
            //DJV_DEBUG_PRINT("w = " << w);
            p[0] = static_cast<float>(pixel);
            p[1] = w;
            sum += w;
        }

        for (; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(out->data(i, j));
            p[0] = static_cast<float>(pixel);
            p[1] = 0.f;
        }

        /*for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(out->data(i, j));
          //DJV_DEBUG_PRINT(p[0] << " = " << p[1]);
        }*/
        //DJV_DEBUG_PRINT("sum = " << sum);

        for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(out->data(i, j));

            p[1] = p[1] / sum;
        }
    }
}

const QString vertexSource =
"void main(void)\n"
"{\n"
"    gl_FrontColor  = gl_Color;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"}\n";

const QString fragmentSourceDefault =
"uniform sampler2DRect texture;\n"
"uniform vec2          scaleInput;\n"
"uniform vec2          scaleOutput;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = texture2DRect(\n"
"        texture,\n"
"        gl_TexCoord[0].st / scaleOutput * scaleInput);\n"
"}\n";

const QString fragmentSourceX =
"    tmp = texture2DRect(\n"
"        contrib,\n"
"        vec2(gl_TexCoord[0].s, float(i)));\n"
"    value += tmp[3] * texture2DRect(\n"
"        texture,\n"
"        vec2(tmp[0] + 0.5, gl_TexCoord[0].t));\n";

const QString fragmentSourceY =
"    tmp = texture2DRect(\n"
"        contrib,\n"
"        vec2(gl_TexCoord[0].t, float(i)));\n"
"    value += tmp[3] * texture2DRect(\n"
"        texture,\n"
"        vec2(gl_TexCoord[0].s, tmp[0] + 0.5));\n";

const QString fragmentSourceCustom =
"const int scale_width = %1;\n"
"\n"
"vec4 scale(sampler2DRect texture, sampler2DRect contrib)\n"
"{\n"
"    vec4 value = vec4(0.0);\n"
"    vec4 tmp;\n"
"    for (int i = 0; i < scale_width; ++i)\n"
"    {\n"
"%2"
"    }\n"
"    return value;\n"
"}\n"
"\n"
"uniform sampler2DRect texture;\n"
"uniform sampler2DRect contrib;\n"
"\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = scale(texture, contrib);\n"
"}\n";

} // namespace

void ScaleOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("ScaleOp::render");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("size = " << _values.size);
    //DJV_DEBUG_PRINT("type = " << _values.type);

    // Initialize.
    djvPixelDataInfo info = in.info();
    begin();
    if (DEFAULT == _values.type)
    {
        _texture.init(
            info,
            GL_TEXTURE_RECTANGLE,
            filterToGl(_values.defaultMin),
            filterToGl(_values.defaultMag));
        const StateDefault state(_values);
        if (_stateDefault != state)
        {
            //DJV_DEBUG_PRINT("init");
            _render.shader.init(vertexSource, fragmentSourceDefault);
            _stateDefault = state;
        }
    }
    else
    {
        _texture.init(info, GL_TEXTURE_RECTANGLE);
        _render.textureTmp.init(
            djvPixelDataInfo(_values.size.x, in.h(), info.pixel),
            GL_TEXTURE_RECTANGLE);
        const StateCustom state(_values);
        if (_stateCustom != state)
        {
            //DJV_DEBUG_PRINT("init");
            _render.offscreen.init();
            djvPixelData contrib;
            contribFnc(
                in.w(),
                _values.size.x,
                _values.customMin,
                _values.customMag,
                &contrib);
            _render.contribX.init(contrib, GL_TEXTURE_RECTANGLE);
            _render.shaderX.init(
                vertexSource,
                QString(fragmentSourceCustom).
                    arg(contrib.h()).
                    arg(fragmentSourceX));
            contribFnc(
                in.h(),
                _values.size.y,
                _values.customMin,
                _values.customMag,
                &contrib);
            _render.contribY.init(contrib, GL_TEXTURE_RECTANGLE);
            _render.shaderY.init(
                vertexSource,
                QString(fragmentSourceCustom).
                    arg(contrib.h()).
                    arg(fragmentSourceY));
            _stateCustom = state;
        }
    }

    // Render.
    if (DEFAULT == _values.type)
    {
        _render.shader.bind();
        const GLuint program = _render.shader.program();
        glUniform2f(glGetUniformLocation(program, "scaleInput"),
            static_cast<GLfloat>(in.w()), static_cast<GLfloat>(in.h()));
        glUniform2f(glGetUniformLocation(program, "scaleOutput"),
            static_cast<GLfloat>(_values.size.x),
            static_cast<GLfloat>(_values.size.y));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(program, "texture"), 0);
        _texture.bind();
        _texture.copy(in);
        info = in.info();
        info.size = _values.size;
        djvOpenGLUtil::ortho(_values.size);
        glViewport(0, 0, _values.size.x, _values.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        Util::quad(info);
    }
    else
    {
        // Horizontal.
        _render.offscreen.bind();
        _render.offscreen.set(_render.textureTmp);
        _render.shaderX.bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(
            glGetUniformLocation(_render.shaderX.program(), "texture"), 0);
        _texture.bind();
        _texture.copy(in);
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(
            glGetUniformLocation(_render.shaderX.program(), "contrib"), 1);
        _render.contribX.bind();
        info = in.info();
        info.size.x = _values.size.x;
        djvOpenGLUtil::ortho(info.size);
        glViewport(0, 0, info.size.x, info.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        Util::quad(info);
        _render.offscreen.unbind();

        // Vertical.
        _render.shaderY.bind();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(
            glGetUniformLocation(_render.shaderY.program(), "texture"), 0);
        _render.textureTmp.bind();
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(
            glGetUniformLocation(_render.shaderY.program(), "contrib"), 1);
        _render.contribY.bind();
        djvOpenGLUtil::ortho(_values.size);
        glViewport(0, 0, _values.size.x, _values.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        Util::quad(djvPixelDataInfo(_values.size, in.pixel()));
    }

    end();
}

void ScaleOp::setValues(const Values & values)
{
    if (values == _values)
        return;
    _values = values;
    Q_EMIT opChanged();
}

//------------------------------------------------------------------------------
// ScaleOpWidget
//------------------------------------------------------------------------------

ScaleOpWidget::ScaleOpWidget(
    ScaleOp * op,
    Context * context) :
    AbstractOpWidget(context),
    _op(op)
{
    // Create the widgets.
    QGroupBox * sizeGroup = new QGroupBox("Size");
    djvIntEditSlider * width = new djvIntEditSlider(context);
    djvIntEditSlider * height = new djvIntEditSlider(context);

    QGroupBox * filterGroup = new QGroupBox("Filter");

    for (int i = 0; i < ScaleOp::typeLabels().count(); ++i)
        _typeButton += new QRadioButton(ScaleOp::typeLabels()[i]);

    QComboBox * filterDefaultMin = new QComboBox;
    filterDefaultMin->addItems(ScaleOp::filterDefaultLabels());

    QComboBox * filterDefaultMag = new QComboBox;
    filterDefaultMag->addItems(ScaleOp::filterDefaultLabels());

    QComboBox * filterCustomMin = new QComboBox;
    filterCustomMin->addItems(ScaleOp::filterCustomLabels());

    QComboBox * filterCustomMag = new QComboBox;
    filterCustomMag->addItems(ScaleOp::filterCustomLabels());

    // Layout the widgets.
    QVBoxLayout * layout = new QVBoxLayout(this);

    QVBoxLayout * vLayout = new QVBoxLayout(sizeGroup);
    vLayout->setMargin(0);
    vLayout->addWidget(width);
    vLayout->addWidget(height);
    layout->addWidget(sizeGroup);

    QGridLayout * gLayout = new QGridLayout(filterGroup);
    gLayout->setMargin(0);
    gLayout->addWidget(_typeButton[0], 0, 0);
    gLayout->addWidget(filterDefaultMin, 0, 1);
    gLayout->addWidget(filterDefaultMag, 0, 2);
    gLayout->addWidget(_typeButton[1], 1, 0);
    gLayout->addWidget(filterCustomMin, 1, 1);
    gLayout->addWidget(filterCustomMag, 1, 2);
    gLayout->setColumnStretch(3, 1);
    layout->addWidget(filterGroup);

    layout->addStretch();

    // Initialize.
    width->setRange(1, 2048);
    width->setValue(op->values().size.x);

    height->setRange(1, 2048);
    height->setValue(op->values().size.y);

    _typeButton[op->values().type]->setChecked(true);

    filterDefaultMin->setCurrentIndex(op->values().defaultMin);
    filterDefaultMag->setCurrentIndex(op->values().defaultMag);

    filterCustomMin->setCurrentIndex(op->values().customMin);
    filterCustomMag->setCurrentIndex(op->values().customMag);

    // Setup the callbacks.
    connect(
        width,
        SIGNAL(valueChanged(int)),
        SLOT(widthCallback(int)));
    connect(
        height,
        SIGNAL(valueChanged(int)),
        SLOT(heightCallback(int)));
    for (int i = 0; i < _typeButton.count(); ++i)
    {
        connect(
            _typeButton[i],
            SIGNAL(toggled(bool)),
            SLOT(typeCallback()));
    }
    connect(
        filterDefaultMin,
        SIGNAL(activated(int)),
        SLOT(defaultMinCallback(int)));
    connect(
        filterDefaultMag,
        SIGNAL(activated(int)),
        SLOT(defaultMagCallback(int)));
    connect(
        filterCustomMin,
        SIGNAL(activated(int)),
        SLOT(customMinCallback(int)));
    connect(
        filterCustomMag,
        SIGNAL(activated(int)),
        SLOT(customMagCallback(int)));
}

void ScaleOpWidget::widthCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.size.x = in;
    _op->setValues(values);
}

void ScaleOpWidget::heightCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.size.y = in;
    _op->setValues(values);
}

void ScaleOpWidget::typeCallback()
{
    ScaleOp::Values values = _op->values();
    for (int i = 0; i < _typeButton.count(); ++i)
    {
        if (_typeButton[i]->isChecked())
        {
            values.type = static_cast<ScaleOp::TYPE>(i);
            break;
        }
    }
    _op->setValues(values);
}

void ScaleOpWidget::defaultMinCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.defaultMin = static_cast<ScaleOp::FILTER_DEFAULT>(in);
    _op->setValues(values);
}

void ScaleOpWidget::defaultMagCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.defaultMag = static_cast<ScaleOp::FILTER_DEFAULT>(in);
    _op->setValues(values);
}

void ScaleOpWidget::customMinCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.customMin = static_cast<ScaleOp::FILTER_CUSTOM>(in);
    _op->setValues(values);
}

void ScaleOpWidget::customMagCallback(int in)
{
    ScaleOp::Values values = _op->values();
    values.customMag = static_cast<ScaleOp::FILTER_CUSTOM>(in);
    _op->setValues(values);
}

//------------------------------------------------------------------------------
// ScaleOpFactory
//------------------------------------------------------------------------------

ScaleOpFactory::ScaleOpFactory(Context * context) :
    AbstractOpFactory(context)
{}

AbstractOp * ScaleOpFactory::createOp() const
{
    return new ScaleOp(context());
}

AbstractOpWidget * ScaleOpFactory::createWidget(AbstractOp * op) const
{
    return new ScaleOpWidget(dynamic_cast<ScaleOp *>(op), context());
}



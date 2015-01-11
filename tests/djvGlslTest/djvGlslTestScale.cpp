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

//! \file djvGlslTestScale.cpp

#include <djvGlslTestScale.h>

#include <djvIntEditSlider.h>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

const QStringList & djvGlslTestScaleOp::typeLabels()
{
    static const QStringList data = QStringList() <<
        "Default" <<
        "Custom";
    
    return data;
}

const QStringList & djvGlslTestScaleOp::filterDefaultLabels()
{
    static const QStringList data = QStringList() <<
        "Nearest" <<
        "Linear";
    
    return data;
}

const QStringList & djvGlslTestScaleOp::filterCustomLabels()
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

djvGlslTestScaleOp::StateDefault::StateDefault() :
    _init(false)
{}

djvGlslTestScaleOp::StateDefault::StateDefault(const Value & in) :
    _filterMin(in.defaultMin),
    _filterMag(in.defaultMag),
    _init(true)
{}

bool djvGlslTestScaleOp::StateDefault::operator != (const StateDefault & in)
    const
{
    return
        _filterMin != in._filterMin ||
        _filterMag != in._filterMag ||
        ! _init;
}

djvGlslTestScaleOp::StateCustom::StateCustom() :
    _init(false)
{}

djvGlslTestScaleOp::StateCustom::StateCustom(const Value & in) :
    _size(in.size),
    _filterMin(in.customMin),
    _filterMag(in.customMag),
    _init(true)
{}

bool djvGlslTestScaleOp::StateCustom::operator != (const StateCustom & in)
    const
{
    return
        _size != in._size ||
        _filterMin != in._filterMin ||
        _filterMag != in._filterMag ||
        ! _init;
}

djvGlslTestScaleOp::djvGlslTestScaleOp()
{
    // Create the widgets.

    QGroupBox * sizeGroup = new QGroupBox("Size");
    djvIntEditSlider * width = new djvIntEditSlider;
    djvIntEditSlider * height = new djvIntEditSlider;

    QGroupBox * filterGroup = new QGroupBox("Filter");
    
    for (int i = 0; i < typeLabels().count(); ++i)
        _typeButton += new QRadioButton(typeLabels()[i]);
    
    QComboBox * filterDefaultMin = new QComboBox;
    filterDefaultMin->addItems(filterDefaultLabels());

    QComboBox * filterDefaultMag = new QComboBox;
    filterDefaultMag->addItems(filterDefaultLabels());

    QComboBox * filterCustomMin = new QComboBox;
    filterCustomMin->addItems(filterCustomLabels());

    QComboBox * filterCustomMag = new QComboBox;
    filterCustomMag->addItems(filterCustomLabels());

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
    width->setValue(_value.size.x);

    height->setRange(1, 2048);
    height->setValue(_value.size.y);
    
    _typeButton[_value.type]->setChecked(true);

    filterDefaultMin->setCurrentIndex(_value.defaultMin);
    filterDefaultMag->setCurrentIndex(_value.defaultMag);

    filterCustomMin->setCurrentIndex(_value.customMin);
    filterCustomMag->setCurrentIndex(_value.customMag);

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

namespace
{

typedef double (FilterFnc)(const double t);

const double supportBox = 0.5;

double filterBox(double t)
{
    if (t > -0.5 && t <= 0.5)
    {
        return 1.0;
    }

    return 0.0;
}

const double supportTriangle = 1.0;

double filterTriangle(double t)
{
    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 1.0)
    {
        return 1.0 - t;
    }

    return 0.0;
}

const double supportBell = 1.5;

double filterBell(double t)
{
    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 0.5)
    {
        return 0.75 - t * t;
    }

    if (t < 1.5)
    {
        t = t - 1.5;
        
        return 0.5 * t * t;
    }

    return 0.0;
}

const double supportBspline = 2.0;

double filterBspline(double t)
{
    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 1.0)
    {
        const double tt = t * t;
        
        return (0.5 * tt * t) - tt + 2.0 / 3.0;
    }
    else if (t < 2.0)
    {
        t = 2.0 - t;
        
        return (1.0 / 6.0) * (t * t * t);
    }

    return 0.0;
}

double sinc(double x)
{
    x *= djvMath::pi;

    if (x != 0.0)
    {
        return djvMath::sin(x) / x;
    }

    return 1.0;
}

const double supportLanczos3 = 3.0;

double filterLanczos3(double t)
{
    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 3.0)
    {
        return sinc(t) * sinc(t / 3.0);
    }

    return 0.0;
}

const double supportCubic = 1.0;

double filterCubic(double t)
{
    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 1.0)
    {
        return (2.0 * t - 3.0) * t * t + 1.0;
    }

    return 0.0;
}

const double supportMitchell = 2.0;

double filterMitchell(double t)
{
    const double        tt = t * t;
    static const double b  = 1.0 / 3.0;
    static const double c  = 1.0 / 3.0;

    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 1.0)
    {
        t = ((12.0 - 9.0 * b - 6.0 * c) * (t * tt)) +
            ((-18.0 + 12.0 * b + 6.0 * c) * tt) +
            (6.0 - 2.0 * b);
        
        return t / 6.0;
    }
    else if (t < 2.0)
    {
        t = ((-1.0 * b - 6.0 * c) * (t * tt)) +
            ((6.0 * b + 30.0 * c) * tt) +
            ((-12.0 * b - 48.0 * c) * t) +
            (8.0 * b + 24.0 * c);
        
        return t / 6.0;
    }

    return 0.0;
}

FilterFnc * filterFnc(djvGlslTestScaleOp::FILTER_CUSTOM in)
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

double filterSupport(djvGlslTestScaleOp::FILTER_CUSTOM in)
{
    static const double tmp [] =
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
    int                               input,
    int                               output,
    djvGlslTestScaleOp::FILTER_CUSTOM filterMin,
    djvGlslTestScaleOp::FILTER_CUSTOM filterMag,
    djvPixelData *                    out)
{
    //DJV_DEBUG("_contrib");
    //DJV_DEBUG_PRINT("scale = " << input << " " << output);
    //DJV_DEBUG_PRINT("filter = " << filterMin << " " << filterMag);

    // Filter function.

    djvGlslTestScaleOp::FILTER_CUSTOM filter =
        input < output ? filterMag : filterMin;
    
    FilterFnc * fnc = filterFnc(filter);
    
    const double support = filterSupport(filter);
    
    //DJV_DEBUG_PRINT("support = " << support);

    //! \todo Edges?

    //const double scale =
    //  static_cast<double>(output) / static_cast<double>(input);
    
    const double scale =
        (output > 1 ? (output - 1) : 1) /
        static_cast<double>(input > 1 ? (input - 1) : 1);
    
    //DJV_DEBUG_PRINT("scale = " << scale);

    const double radius = support * ((scale >= 1.0) ? 1.0 : (1.0 / scale));
    
    //DJV_DEBUG_PRINT("radius = " << radius);

    const int width = djvMath::ceil(radius * 2 + 1);
    
    //DJV_DEBUG_PRINT("width = " << width);

    out->set(djvPixelDataInfo(output, width, djvPixel::LA_F32));

    for (int i = 0; i < output; ++i)
    {
        const double center = i / scale;
        
        int left  = djvMath::ceil (center - radius);
        int right = djvMath::floor(center + radius);
        
        //DJV_DEBUG_PRINT(i << " = " << left << " " << center << " " << right);

        float sum   = 0.0;
        int   pixel = 0;
        int   j     = 0;

        for (int k = left; j < width && k <= right; ++j, ++k)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(out->data(i, j));

            pixel = djvGlslTestScaleOp::edge(k, input);

            const float x = static_cast<float>(
                (center - k) * (scale < 1.0 ? scale : 1.0));
            
            const float w = static_cast<float>(
                (scale < 1.0) ? ((*fnc)(x) * scale) : (*fnc)(x));
            
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
            p[1] = 0.0;
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

const QString srcDefault =
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

const QString srcX =
"    tmp = texture2DRect(\n"
"        contrib,\n"
"        vec2(gl_TexCoord[0].s, float(i)));\n"
"    value += tmp[3] * texture2DRect(\n"
"        texture,\n"
"        vec2(tmp[0] + 0.5, gl_TexCoord[0].t));\n";

const QString srcY =
"    tmp = texture2DRect(\n"
"        contrib,\n"
"        vec2(gl_TexCoord[0].t, float(i)));\n"
"    value += tmp[3] * texture2DRect(\n"
"        texture,\n"
"        vec2(gl_TexCoord[0].s, tmp[0] + 0.5));\n";

const QString srcCustom =
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

void djvGlslTestScaleOp::render(const djvImage & in) throw (djvError)
{
    //DJV_DEBUG("djvGlslTestScaleOp::render");
    //DJV_DEBUG_PRINT("in = " << in);
    //DJV_DEBUG_PRINT("size = " << _value.size);
    //DJV_DEBUG_PRINT("type = " << _value.type);

    // Initialize.

    djvPixelDataInfo info = in.info();

    begin();

    if (DEFAULT == _value.type)
    {
        _texture.init(
            info,
            filterToGl(_value.defaultMin),
            filterToGl(_value.defaultMag));

        const StateDefault state(_value);

        if (_stateDefault != state)
        {
            //DJV_DEBUG_PRINT("init");

            _render.shader.init(srcDefault);

            _stateDefault = state;
        }
    }
    else
    {
        _texture.init(info);

        _render.textureTmp.init(
            djvPixelDataInfo(_value.size.x, in.h(), info.pixel));

        const StateCustom state(_value);

        if (_stateCustom != state)
        {
            //DJV_DEBUG_PRINT("init");

            _render.offscreen.init();

            djvPixelData contrib;
            contribFnc(
                in.w(),
                _value.size.x,
                _value.customMin,
                _value.customMag,
                &contrib);
            _render.contribX.init(contrib);
            _render.shaderX.init(QString(srcCustom).
                arg(contrib.h()).
                arg(srcX));

            contribFnc(
                in.h(),
                _value.size.y,
                _value.customMin,
                _value.customMag,
                &contrib);
            _render.contribY.init(contrib);
            _render.shaderY.init(QString(srcCustom).
                arg(contrib.h()).
                arg(srcY));

            _stateCustom = state;
        }
    }

    // Render.

    if (DEFAULT == _value.type)
    {
        _render.shader.bind();
        const GLuint program = _render.shader.program();

        glUniform2f(glGetUniformLocation(program, "scaleInput"),
            static_cast<GLfloat>(in.w()), static_cast<GLfloat>(in.h()));
        glUniform2f(glGetUniformLocation(program, "scaleOutput"),
            static_cast<GLfloat>(_value.size.x),
            static_cast<GLfloat>(_value.size.y));

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(program, "texture"), 0);
        _texture.bind();
        _texture.copy(in);

        info = in.info();
        info.size = _value.size;
        djvOpenGlUtil::ortho(_value.size);
        glViewport(0, 0, _value.size.x, _value.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        djvGlslTestUtil::quad(info);
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
        info.size.x = _value.size.x;
        djvOpenGlUtil::ortho(info.size);
        glViewport(0, 0, info.size.x, info.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        djvGlslTestUtil::quad(info);

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

        djvOpenGlUtil::ortho(_value.size);
        glViewport(0, 0, _value.size.x, _value.size.y);
        glClear(GL_COLOR_BUFFER_BIT);
        djvGlslTestUtil::quad(djvPixelDataInfo(_value.size, in.pixel()));
    }

    end();
}

int djvGlslTestScaleOp::edge(int in, int size)
{
    return djvMath::clamp(in, 0, size - 1);
}

GLuint djvGlslTestScaleOp::filterToGl(FILTER_DEFAULT in)
{
    switch (in)
    {
        case NEAREST: return GL_NEAREST;
        case LINEAR:  return GL_LINEAR;
    }

    return 0;
}

void djvGlslTestScaleOp::widthCallback(int in)
{
    _value.size.x = in;

    Q_EMIT changed();
}

void djvGlslTestScaleOp::heightCallback(int in)
{
    _value.size.y = in;

    Q_EMIT changed();
}

void djvGlslTestScaleOp::typeCallback()
{
    for (int i = 0; i < _typeButton.count(); ++i)
    {
        if (_typeButton[i]->isChecked())
        {
            _value.type = static_cast<TYPE>(i);
            
            break;
        }
    }

    Q_EMIT changed();
}

void djvGlslTestScaleOp::defaultMinCallback(int in)
{
    _value.defaultMin = static_cast<FILTER_DEFAULT>(in);

    Q_EMIT changed();
}

void djvGlslTestScaleOp::defaultMagCallback(int in)
{
    _value.defaultMag = static_cast<FILTER_DEFAULT>(in);

    Q_EMIT changed();
}

void djvGlslTestScaleOp::customMinCallback(int in)
{
    _value.customMin = static_cast<FILTER_CUSTOM>(in);

    Q_EMIT changed();
}

void djvGlslTestScaleOp::customMagCallback(int in)
{
    _value.customMag = static_cast<FILTER_CUSTOM>(in);

    Q_EMIT changed();
}

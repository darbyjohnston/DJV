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

//! \file djvOpenGlImageDraw.cpp

#include <djvOpenGlImage.h>

#include <djvOpenGlOffscreenBuffer.h>

#include <djvDebug.h>
#include <djvError.h>
#include <djvMatrixUtil.h>
#include <djvPixelDataUtil.h>
#include <djvVector.h>

#include <QCoreApplication>

//------------------------------------------------------------------------------
//! \class djvOpenGlImageLut
//!
//! This class provides an OpenGL lookup table.
//------------------------------------------------------------------------------

class djvOpenGlImageLut
{
public:

    djvOpenGlImageLut();

    ~djvOpenGlImageLut();

    void init(const djvPixelDataInfo &) throw (djvError);

    void init(const djvPixelData &) throw (djvError);

    void copy(const djvPixelData &);

    void bind();

    void unbind();

    const djvPixelDataInfo & info() const;

    GLuint id() const;

private:

    void del();

    djvPixelDataInfo _info;
    int              _size;
    GLuint           _id;
};

djvOpenGlImageLut::djvOpenGlImageLut() :
    _size(0),
    _id  (0)
{}

djvOpenGlImageLut::~djvOpenGlImageLut()
{
    del();
}

void djvOpenGlImageLut::init(const djvPixelDataInfo & info) throw (djvError)
{
    if (info == _info)
        return;

    //DJV_DEBUG("djvOpenGlImageLut::init");
    //DJV_DEBUG_PRINT("info = " << info);

    del();

    _info = info;
    _size = djvMath::toPow2(_info.size.x);

    //DJV_DEBUG_PRINT("size = " << _size);

    DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));

    if (! _id)
    {
        throw djvError(
            "djvOpenGlImageLut",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_TEXTURE]);
    }

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_1D, _id));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    GLenum format = GL_RGBA;

    if (djvPixel::F16 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA16F;
    }
    else if (djvPixel::F32 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA32F;
    }

    DJV_DEBUG_OPEN_GL(
        glTexImage1D(
            GL_TEXTURE_1D,
            0,
            format,
            _size,
            0,
            djvOpenGlUtil::format(_info.pixel, _info.bgr),
            djvOpenGlUtil::type(_info.pixel),
            0));
}

void djvOpenGlImageLut::init(const djvPixelData & data) throw (djvError)
{
    init(data.info());

    bind();

    copy(data);
}

void djvOpenGlImageLut::copy(const djvPixelData & in)
{
    //DJV_DEBUG("djvOpenGlImageLut::copy");
    //DJV_DEBUG_PRINT("in = " << in);

    const djvPixelDataInfo & info = in.info();

    djvOpenGlImage::stateUnpack(in.info());

    DJV_DEBUG_OPEN_GL(
        glTexSubImage1D(
            GL_TEXTURE_1D,
            0,
            0,
            info.size.x,
            djvOpenGlUtil::format(info.pixel, info.bgr),
            djvOpenGlUtil::type(info.pixel),
            in.data()));
}

void djvOpenGlImageLut::bind()
{
    //DJV_DEBUG("djvOpenGlImageLut::bind");

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_1D, _id));
}

const djvPixelDataInfo & djvOpenGlImageLut::info() const
{
    return _info;
}

GLuint djvOpenGlImageLut::id() const
{
    return _id;
}

void djvOpenGlImageLut::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);

        _id = 0;
    }
}

//------------------------------------------------------------------------------
//! \class djvOpenGlImageTexture
//!
//! This class provides an OpenGL texture.
//------------------------------------------------------------------------------

class djvOpenGlImageTexture
{
public:

    djvOpenGlImageTexture();

    ~djvOpenGlImageTexture();

    void init(
        const djvPixelDataInfo &,
        GLenum = GL_LINEAR,
        GLenum = GL_LINEAR) throw (djvError);

    void init(const djvPixelData &, GLenum = GL_LINEAR, GLenum = GL_LINEAR)
        throw (djvError);

    void copy(const djvPixelData &);

    void copy(const djvVector2i &);

    void bind();

    const djvPixelDataInfo & info() const;

    GLenum min() const;

    GLenum mag() const;

    GLuint id() const;

private:

    void del();

    djvPixelDataInfo _info;
    GLenum           _min;
    GLenum           _mag;
    GLuint           _id;
};

djvOpenGlImageTexture::djvOpenGlImageTexture() :
    _min(GL_LINEAR),
    _mag(GL_LINEAR),
    _id (0)
{}

djvOpenGlImageTexture::~djvOpenGlImageTexture()
{
    del();
}

void djvOpenGlImageTexture::init(
    const djvPixelDataInfo & info,
    GLenum                   min,
    GLenum                   mag) throw (djvError)
{
    if (info == _info && min == _min && mag == _mag)
        return;

    del();

    //DJV_DEBUG("djvOpenGlImageTexture::init");
    //DJV_DEBUG_PRINT("info = " << info);

    _info = info;
    _min  = min;
    _mag  = mag;

    DJV_DEBUG_OPEN_GL(glGenTextures(1, &_id));

    //DJV_DEBUG_PRINT("id = " << int(_id));

    if (! _id)
    {
        throw djvError(
            "djvOpenGlImageTexture",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_TEXTURE]);
    }

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min));
    DJV_DEBUG_OPEN_GL(
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mag));

    GLenum format = GL_RGBA;

    if (djvPixel::F16 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA16F;
    }
    else if (djvPixel::F32 == djvPixel::type(_info.pixel))
    {
        format = GL_RGBA32F;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        _info.size.x,
        _info.size.y,
        0,
        djvOpenGlUtil::format(_info.pixel, _info.bgr),
        djvOpenGlUtil::type(_info.pixel),
        0);
    
    GLenum error = glGetError();
    
#if ! defined(DJV_OSX)

    //! \todo On OS X this error is triggered the first time djv_view is
    //! started, though it doesn't actually seem to be a problem? If we
    //! throw here the image is not displayed (start djv_view from the
    //! command line with an image), but if we igore the error the image is
    //! displayed OK? Is this related to the "invalid drawable" message we
    //! are also getting on start up?
    
    if (error != GL_NO_ERROR)
    {
        throw djvError(
            "djvOpenGlImageTexture",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_TEXTURE2].
            arg((char *)gluErrorString(error)));
    }
    
#endif // DJV_OSX
}

void djvOpenGlImageTexture::init(
    const djvPixelData & data,
    GLenum               min,
    GLenum               mag) throw (djvError)
{
    init(data.info(), min, mag);

    bind();

    copy(data);
}

void djvOpenGlImageTexture::copy(const djvPixelData & in)
{
    //DJV_DEBUG("djvOpenGlImageTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));

    const djvPixelDataInfo & info = in.info();

    djvOpenGlImage::stateUnpack(in.info());

    DJV_DEBUG_OPEN_GL(
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            info.size.x,
            info.size.y,
            djvOpenGlUtil::format(info.pixel, info.bgr),
            djvOpenGlUtil::type(info.pixel),
            in.data()));
}

void djvOpenGlImageTexture::copy(const djvVector2i & in)
{
    //DJV_DEBUG("djvOpenGlImageTexture::copy");
    //DJV_DEBUG_PRINT("in = " << in);

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));

    DJV_DEBUG_OPEN_GL(
        glCopyTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            0,
            0,
            in.x,
            in.y));
}

void djvOpenGlImageTexture::bind()
{
    //DJV_DEBUG("djvOpenGlImageTexture::bind");

    DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, _id));
}

const djvPixelDataInfo & djvOpenGlImageTexture::info() const
{
    return _info;
}

GLenum djvOpenGlImageTexture::min() const
{
    return _min;
}

GLenum djvOpenGlImageTexture::mag() const
{
    return _mag;
}

GLuint djvOpenGlImageTexture::id() const
{
    return _id;
}

void djvOpenGlImageTexture::del()
{
    if (_id)
    {
        glDeleteTextures(1, &_id);

        _id = 0;
    }
}

//------------------------------------------------------------------------------
//! \class djvOpenGlImageShader
//!
//! This class provides an OpenGL shader.
//------------------------------------------------------------------------------

class djvOpenGlImageShader
{
public:

    djvOpenGlImageShader();

    ~djvOpenGlImageShader();

    void init(const QString & vertex, const QString & fragment) throw (djvError);

    void bind();

    GLuint program() const;

private:

    void del();

    QString _vertex;
    QString _fragment;

    GLuint _vertexId;
    GLuint _fragmentId;
    GLuint _program;
};

djvOpenGlImageShader::djvOpenGlImageShader() :
    _vertexId  (0),
    _fragmentId(0),
    _program   (0)
{}

djvOpenGlImageShader::~djvOpenGlImageShader()
{
    del();
}

namespace
{

GLuint shaderCreate(GLenum type) throw (djvError)
{
    //DJV_DEBUG("shaderCreate");

    GLuint r = 0;

    DJV_DEBUG_OPEN_GL(r = glCreateShader(type));

    if (! r)
    {
        throw djvError(
            "djvOpenGlImageShader",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_SHADER]);
    }

    return r;
}

void shaderCompile(GLuint id, const QString & source) throw (djvError)
{
    //DJV_DEBUG("shaderCompile");
    //DJV_DEBUG_PRINT("source = " << source);

    djvMemoryBuffer<char> buf(source.length());
    djvMemory::copy(source.toLatin1().data(), buf.data(), buf.size());

    const char * sources       [] = { buf.data() };
    const GLint  sourceLengths [] = { buf.size() };

    DJV_DEBUG_OPEN_GL(glShaderSource(id, 1, sources, sourceLengths));

    DJV_DEBUG_OPEN_GL(glCompileShader(id));

    GLint error = GL_FALSE;

    glGetShaderiv(id, GL_COMPILE_STATUS, &error);

    char log [4096] = "";
    GLsizei logSize = 0;
    glGetShaderInfoLog(id, 4096, &logSize, log);

    //DJV_DEBUG_PRINT("log = " << QString(log));

    if (error != GL_TRUE)
    {
        throw djvError(
            "djvOpenGlImageShader",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_COMPILE_SHADER].
            arg(log));
    }
}

} // namespace

void djvOpenGlImageShader::init(
    const QString & vertex,
    const QString & fragment) throw (djvError)
{
    if (vertex == _vertex && fragment == _fragment)
        return;

    //DJV_DEBUG("djvOpenGlImageShader::init");
    //DJV_DEBUG_PRINT("fragment = " << fragment);

    del();

    _vertex   = vertex;
    _fragment = fragment;

    GLint error = GL_FALSE;

    _vertexId   = shaderCreate(GL_VERTEX_SHADER);
    _fragmentId = shaderCreate(GL_FRAGMENT_SHADER);

    shaderCompile(_vertexId,   _vertex);
    shaderCompile(_fragmentId, _fragment);

    DJV_DEBUG_OPEN_GL(_program = glCreateProgram());

    if (! _program)
    {
        throw djvError(
            "djvOpenGlImageShader",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_PROGRAM]);
    }

    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _vertexId));
    DJV_DEBUG_OPEN_GL(glAttachShader(_program, _fragmentId));
    DJV_DEBUG_OPEN_GL(glLinkProgram (_program));

    glGetProgramiv(_program, GL_LINK_STATUS, &error);

    char log [4096] = "";
    GLsizei logSize = 0;
    glGetProgramInfoLog(_program, 4096, &logSize, log);

    //DJV_DEBUG_PRINT("log = " << QString(log));

    if (error != GL_TRUE)
    {
        throw djvError(
            "djvOpenGlImageShader",
            djvOpenGlImage::errorLabels()[djvOpenGlImage::ERROR_CREATE_PROGRAM].
            arg(log));
    }
}

void djvOpenGlImageShader::bind()
{
    //DJV_DEBUG("djvOpenGlImageShader::bind");

    DJV_DEBUG_OPEN_GL(glUseProgram(_program));
}

GLuint djvOpenGlImageShader::program() const
{
    return _program;
}

void djvOpenGlImageShader::del()
{
    if (_vertexId)
    {
        glDeleteShader(_vertexId);
    }

    if (_fragmentId)
    {
        glDeleteShader(_fragmentId);
    }

    if (_program)
    {
        glDeleteProgram(_program);
    }
}

//------------------------------------------------------------------------------
// djvOpenGlImageState
//------------------------------------------------------------------------------

djvOpenGlImageState::djvOpenGlImageState() :
    _init             (false),
    _texture          (new djvOpenGlImageTexture),
    _shader           (new djvOpenGlImageShader),
    _scaleXContrib    (new djvOpenGlImageTexture),
    _scaleYContrib    (new djvOpenGlImageTexture),
    _scaleXShader     (new djvOpenGlImageShader),
    _scaleYShader     (new djvOpenGlImageShader),
    _lutColorProfile  (new djvOpenGlImageLut),
    _lutDisplayProfile(new djvOpenGlImageLut)
{}

djvOpenGlImageState::~djvOpenGlImageState()
{
    delete _lutDisplayProfile;
    delete _lutColorProfile;
    delete _scaleXShader;
    delete _scaleYShader;
    delete _scaleXContrib;
    delete _scaleYContrib;
    delete _shader;
    delete _texture;
}

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

namespace
{

void activeTexture(GLenum in)
{
    //DJV_DEBUG("activeTexture");

    DJV_DEBUG_OPEN_GL(glActiveTexture(in));
}

void uniform1i(GLuint program, const QString & name, int value)
{
    glUniform1i(
        glGetUniformLocation(program, name.toLatin1().data()),
        static_cast<GLint>(value));
}

void uniform1f(GLuint program, const QString & name, double value)
{
    glUniform1f(
        glGetUniformLocation(program, name.toLatin1().data()),
        static_cast<GLfloat>(value));
}

void uniformMatrix4f(
    GLuint              program,
    const QString &     name,
    const djvMatrix4f & value)
{
    glUniformMatrix4fv(
        glGetUniformLocation(program, name.toLatin1().data()),
        1,
        false,
        djvMatrixUtil::convert<double, GLfloat>(djvMatrixUtil::transpose(value)).e);
}

typedef double (FilterFnc)(const double t);

static const double supportBox = 0.5;

static double filterBox(double t)
{
    if (t > -0.5 && t <= 0.5)
    {
        return 1.0;
    }

    return 0.0;
}

static const double supportTriangle = 1.0;

static double filterTriangle(double t)
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

static const double supportBell = 1.5;

static double filterBell(double t)
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

static const double supportBSpline = 2.0;

static double filterBSpline(double t)
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

static double sinc(double x)
{
    x *= djvMath::pi;

    if (x != 0.0)
    {
        return djvMath::sin(x) / x;
    }

    return 1.0;
}

static const double supportLanczos3 = 3.0;

static double filterLanczos3(double t)
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

static const double supportCubic = 1.0;

static double filterCubic(double t)
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

static const double supportMitchell = 2.0;

static double filterMitchell(double t)
{
    const double tt = t * t;
    static const double b = 1.0 / 3.0;
    static const double c = 1.0 / 3.0;

    if (t < 0.0)
    {
        t = -t;
    }

    if (t < 1.0)
    {
        t =
            ((12.0 - 9.0 * b - 6.0 * c) * (t * tt)) +
            ((-18.0 + 12.0 * b + 6.0 * c) * tt) +
            (6.0 - 2.0 * b);
        return t / 6.0;
    }
    else if (t < 2.0)
    {
        t =
            ((-1.0 * b - 6.0 * c) * (t * tt)) +
            ((6.0 * b + 30.0 * c) * tt) +
            ((-12.0 * b - 48.0 * c) * t) +
            (8.0 * b + 24.0 * c);
        return t / 6.0;
    }

    return 0.0;
}

FilterFnc * filterFnc(djvOpenGlImageFilter::FILTER in)
{
    static FilterFnc * tmp [] =
    {
        filterBox,
        filterBox,
        filterBox,
        filterTriangle,
        filterBell,
        filterBSpline,
        filterLanczos3,
        filterCubic,
        filterMitchell
    };

    return tmp[in];
}

static double filterSupport(djvOpenGlImageFilter::FILTER in)
{
    static const double tmp [] =
    {
        supportBox,
        supportBox,
        supportBox,
        supportTriangle,
        supportBell,
        supportBSpline,
        supportLanczos3,
        supportCubic,
        supportMitchell
    };

    return tmp[in];
}

int edge(int in, int size)
{
    return djvMath::clamp(in, 0, size - 1);
}

void scaleContrib(
    int                          input,
    int                          output,
    djvOpenGlImageFilter::FILTER filter,
    djvPixelData &               data)
{
    //DJV_DEBUG("scaleContrib");
    //DJV_DEBUG_PRINT("scale = " << input << " " << output);
    //DJV_DEBUG_PRINT("filter = " << filter);

    // Filter function.

    FilterFnc * fnc = filterFnc(filter);

    const double support = filterSupport(filter);

    //DJV_DEBUG_PRINT("support = " << support);

    const double scale =
        static_cast<double>(output) / static_cast<double>(input);

    //DJV_DEBUG_PRINT("scale = " << scale);

    const double radius =
        support * (scale >= 1.0 ? 1.0 : (1.0 / scale));

    //DJV_DEBUG_PRINT("radius = " << radius);

    // Initialize.

    const int width = djvMath::ceil(radius * 2.0 + 1.0);

    //DJV_DEBUG_PRINT("width = " << width);

    data.set(djvPixelDataInfo(output, width, djvPixel::LA_F32));

    // Work.

    for (int i = 0; i < output; ++i)
    {
        const double center = i / scale;
        const int    left   = djvMath::ceil (center - radius);
        const int    right  = djvMath::floor(center + radius);

        //DJV_DEBUG_PRINT(i << " = " << left << " " << center << " " << right);

        double sum   = 0.0;
        int    pixel = 0;

        int j = 0;

        for (int k = left; j < width && k <= right; ++j, ++k)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));

            pixel = edge(k, input);

            const double x = (center - k) * (scale < 1.0 ? scale : 1.0);
            const double w = (scale < 1.0) ? ((*fnc)(x) * scale) : (*fnc)(x);

            //DJV_DEBUG_PRINT("w = " << w);

            p[0] = static_cast<djvPixel::F32_T>(pixel / double(input));
            p[1] = static_cast<djvPixel::F32_T>(w);

            sum += w;
        }

        for (; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));

            p[0] = static_cast<djvPixel::F32_T>(pixel / double(input));
            p[1] = 0.0f;
        }

        /*for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<Pixel::F32_T *>(data.data(i, j));

            //DJV_DEBUG_PRINT(p[0] << " = " << p[1]);
        }
        //DJV_DEBUG_PRINT("sum = " << sum);*/

        //! \todo Why is it necessary to average the scale contributions?
        //! Without this the values don't always add up to zero causing image
        //! artifacts.

        for (j = 0; j < width; ++j)
        {
            djvPixel::F32_T * p =
                reinterpret_cast<djvPixel::F32_T *>(data.data(i, j));

            p[1] /= static_cast<djvPixel::F32_T>(sum);
        }
    }
}

void quad(
    const djvVector2i &              size,
    const djvPixelDataInfo::Mirror & mirror     = djvPixelDataInfo::Mirror(),
    int                              proxyScale = 1)
{
    //DJV_DEBUG("quad");

    double u [] = { 0, 0 };
    double v [] = { 0, 0 };

    u[! mirror.x] = 1.0;
    v[! mirror.y] = 1.0;

    //DJV_DEBUG_PRINT("u = " << u[0] << " " << u[1]);
    //DJV_DEBUG_PRINT("v = " << v[0] << " " << v[1]);

    const djvVector2f uv[] =
    {
        djvVector2f(u[0], v[0]),
        djvVector2f(u[0], v[1]),
        djvVector2f(u[1], v[1]),
        djvVector2f(u[1], v[0])
    };

    glBegin(GL_QUADS);

    djvOpenGlUtil::drawBox(size * proxyScale, uv);

    glEnd();
}

} // namespace

//------------------------------------------------------------------------------
// Shader Source
//------------------------------------------------------------------------------

namespace
{

const QString sourceVertex =
    "void main(void)\n"
    "{\n"
    "    gl_FrontColor  = gl_Color;\n"
    "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
    "    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n";

const QString sourceGamma =
    "    if (tmp[0] >= 0.0)\n"
    "        tmp[0] = pow(tmp[0], data.gamma);\n"
    "    if (tmp[1] >= 0.0)\n"
    "        tmp[1] = pow(tmp[1], data.gamma);\n"
    "    if (tmp[2] >= 0.0)\n"
    "        tmp[2] = pow(tmp[2], data.gamma);\n";

//! \todo How should we handle negative numbers when making color adjustments?
//! Should they be clamped to zero?

const QString sourceFragmentHeader =
    "struct Levels\n"
    "{\n"
    "    float in0, in1;\n"
    "    float gamma;\n"
    "    float out0, out1;\n"
    "};\n"
    "\n"
    "struct Exposure\n"
    "{\n"
    "    float v, d, k, f, g;\n"
    "};\n"
    "\n"
    "float knee(float value, float f)\n"
    "{\n"
    "    return log(value * f + 1.0) / f;\n"
    "}\n"
    "\n"
    "vec4 lut(vec4 value, sampler1D lut)\n"
    "{\n"
    "    value[0] = texture1D(lut, value[0])[0];\n"
    "    value[1] = texture1D(lut, value[1])[1];\n"
    "    value[2] = texture1D(lut, value[2])[2];\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 gamma(vec4 value, float gamma)\n"
    "{\n"
    "    if (value[0] >= 0.0)\n"
    "        value[0] = pow(value[0], gamma);\n"
    "    if (value[1] >= 0.0)\n"
    "        value[1] = pow(value[1], gamma);\n"
    "    if (value[2] >= 0.0)\n"
    "        value[2] = pow(value[2], gamma);\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 displayProfileColor(vec4 value, mat4 color)\n"
    "{\n"
    "    vec4 tmp;\n"
    "\n"
    "    tmp[0] = value[0];\n"
    "    tmp[1] = value[1];\n"
    "    tmp[2] = value[2];\n"
    "    tmp[3] = 1.0;\n"
    "\n"
    "    tmp *= color;\n"
    "\n"
    "    tmp[3] = value[3];\n"
    "\n"
    "    return tmp;\n"
    "}\n"
    "\n"
    "vec4 levels(vec4 value, Levels data)\n"
    "{\n"
    "    vec4 tmp;\n"
    "\n"
    "    tmp[0] = (value[0] - data.in0) / data.in1;\n"
    "    tmp[1] = (value[1] - data.in0) / data.in1;\n"
    "    tmp[2] = (value[2] - data.in0) / data.in1;\n"
    "\n"
    "%1"
    "\n"
    "    value[0] = tmp[0] * data.out1 + data.out0;\n"
    "    value[1] = tmp[1] * data.out1 + data.out0;\n"
    "    value[2] = tmp[2] * data.out1 + data.out0;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 exposure(vec4 value, Exposure data)\n"
    "{\n"
    "    value[0] = max(0.0, value[0] - data.d) * data.v;\n"
    "    value[1] = max(0.0, value[1] - data.d) * data.v;\n"
    "    value[2] = max(0.0, value[2] - data.d) * data.v;\n"
    "\n"
    "    if (value[0] > data.k)\n"
    "        value[0] = data.k + knee(value[0] - data.k, data.f);\n"
    "    if (value[1] > data.k)\n"
    "        value[1] = data.k + knee(value[1] - data.k, data.f);\n"
    "    if (value[2] > data.k)\n"
    "        value[2] = data.k + knee(value[2] - data.k, data.f);\n"
    "\n"
    "    value[0] *= 0.332;\n"
    "    value[1] *= 0.332;\n"
    "    value[2] *= 0.332;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n"
    "vec4 softClip(vec4 value, float softClip)\n"
    "{\n"
    "    float tmp = 1.0 - softClip;\n"
    "\n"
    "    if (value[0] > tmp)\n"
    "        value[0] = tmp + (1.0 - exp(-(value[0] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    if (value[1] > tmp)\n"
    "        value[1] = tmp + (1.0 - exp(-(value[1] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    if (value[2] > tmp)\n"
    "        value[2] = tmp + (1.0 - exp(-(value[2] - tmp) / softClip)) *\n"
    "            softClip;\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentScaleX =
    "vec4 scaleX(sampler2D texture, sampler2D contrib)\n"
    "{\n"
    "    vec4 value = vec4(0.0);\n"
    "\n"
    "    for (int i = 0; i < %1; ++i)\n"
    "    {\n"
    "        float t = float(i) / float(%2 - 1);\n"
    "\n"
    "        vec4 tmp = texture2D(contrib, vec2(gl_TexCoord[0].s, t));\n"
    "\n"
    "        vec2 position = vec2(tmp[0], gl_TexCoord[0].t);\n"
    "\n"
    "        value += tmp[3] * %3;\n"
    "  }\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentScaleY =
    "vec4 scaleY(sampler2D texture, sampler2D contrib)\n"
    "{\n"
    "    vec4 value = vec4(0.0);\n"
    "\n"
    "    for (int i = 0; i < %1; ++i)\n"
    "    {\n"
    "        float t = float(i) / float(%2 - 1);\n"
    "\n"
    "        vec4 tmp = texture2D(contrib, vec2(gl_TexCoord[0].t, t));\n"
    "\n"
    "        vec2 position = vec2(gl_TexCoord[0].s, tmp[0]);\n"
    "\n"
    "        value += tmp[3] * texture2D(texture, position);\n"
    "    }\n"
    "\n"
    "    return value;\n"
    "}\n"
    "\n";

const QString sourceFragmentMain =
    "void main(void)\n"
    "{\n"
    "    vec4 color;\n"
    "\n"
    "%1"
    "\n"
    "    gl_FragColor = color;\n"
    "}\n";

} // namespace

//------------------------------------------------------------------------------
// sourceFragment
//
// Generate the fragment shader source code.
//------------------------------------------------------------------------------

namespace
{

/*QString sourceFragment()
{
    QString header;
    QString main;

    header = "uniform sampler2D inTexture;\n";

    main +=
        "vec2 position = gl_TexCoord[0].st;\n";
    main +=
        "color = texture2D(inTexture, position);\n";

    return
        header + "\n" +
        QString(sourceFragmentMain).arg(main);
}*/

QString sourceFragment(
    djvColorProfile::PROFILE             colorProfile,
    const djvOpenGlImageDisplayProfile & displayProfile,
    djvOpenGlImageOptions::CHANNEL       channel,
    bool                                 multipassFilter,
    int                                  scaleSize,
    bool                                 scaleX)
{
    //DJV_DEBUG("sourceFragment");
    //DJV_DEBUG_PRINT("colorProfile = " << colorProfile);
    //DJV_DEBUG_PRINT("displayProfile = " << displayProfile);
    //DJV_DEBUG_PRINT("channel = " << channel);
    //DJV_DEBUG_PRINT("multipass filter = " << multipassFilter);
    //DJV_DEBUG_PRINT("scale size = " << scaleSize);
    //DJV_DEBUG_PRINT("scale x = " << scaleX);
    
    QString header;
    QString main;

    // Initialize header.

    header = sourceFragmentHeader.
        arg(! djvMath::fuzzyCompare(displayProfile.levels.gamma, 1.0) ?
            sourceGamma : "");
    header += "uniform sampler2D inTexture;\n";

    // Color profile.

    QString sample;

    switch (colorProfile)
    {
        case djvColorProfile::LUT:

            header += "uniform sampler1D inColorProfileLut;\n";

            sample =
                "lut(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileLut)";

            break;

        case djvColorProfile::GAMMA:

            header += "uniform float inColorProfileGamma;\n";

            sample =
                "gamma(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileGamma)";

            break;

        case djvColorProfile::EXPOSURE:

            header += "uniform Exposure inColorProfileExposure;\n";

            sample =
                "exposure(\n"
                "    texture2D(inTexture, position),\n"
                "    inColorProfileExposure)";

            break;

        default:

            sample = "texture2D(inTexture, position)";

            break;
    }

    // Image filter.

    if (! multipassFilter)
    {
        main += "vec2 position = gl_TexCoord[0].st;\n";
        main += QString("color = %1;\n").arg(sample);
    }
    else
    {
        header += "uniform sampler2D inScaleContrib;\n";

        if (scaleX)
        {
            header += QString(sourceFragmentScaleX).
                arg(scaleSize).
                arg(scaleSize).
                arg(sample);

            main += "color = scaleX(inTexture, inScaleContrib);\n";
        }
        else
        {
            header += QString(sourceFragmentScaleY).
                arg(scaleSize).
                arg(scaleSize);

            main += "color = scaleY(inTexture, inScaleContrib);\n";
        }
    }

    // Display profile.

    if (djvVectorUtil::isSizeValid(displayProfile.lut.size()))
    {
        header += "uniform sampler1D inDisplayProfileLut;\n";
        main += "color = lut(color, inDisplayProfileLut);\n";
    }

    if (displayProfile.color != djvOpenGlImageDisplayProfile().color)
    {
        header += "uniform mat4 inDisplayProfileColor;\n";
        main += "color = displayProfileColor(color, inDisplayProfileColor);\n";
    }

    if (displayProfile.levels != djvOpenGlImageDisplayProfile().levels)
    {
        header += "uniform Levels inDisplayProfileLevels;\n";
        main += "color = levels(color, inDisplayProfileLevels);\n";
    }

    if (displayProfile.softClip != djvOpenGlImageDisplayProfile().softClip)
    {
        header += "uniform float inDisplayProfileSoftClip;\n";
        main += "color = softClip(color, inDisplayProfileSoftClip);\n";
    }

    // Image channel.

    if (channel)
    {
        main += QString("color = vec4(color[%1]);\n").arg(channel - 1);
    }

    // Clamp pixel values.

    //if (clamp)
    //    main += "color = clamp(color, vec4(0.0), vec4(1.0));\n";

    return
        header + "\n" +
        QString(sourceFragmentMain).arg(main);
}

} // namespace

//------------------------------------------------------------------------------
// colorProfileInit
//
// This function initializes the color profile.
//------------------------------------------------------------------------------

namespace
{

double knee(double x, double f)
{
    return djvMath::log(x * f + 1.0) / f;
}

double knee2(double x, double y)
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

void colorProfileInit(
    const djvOpenGlImageOptions & options,
    GLuint                        program,
    djvOpenGlImageLut &           colorProfile)
{
    //DJV_DEBUG("colorProfileInit");
    //DJV_DEBUG_PRINT("type = " << options.colorProfile.type);
    
    switch (options.colorProfile.type)
    {
        case djvColorProfile::LUT:
        {
            activeTexture(GL_TEXTURE2);

            uniform1i(program, "inColorProfileLut", 2);

            colorProfile.init(options.colorProfile.lut);
        }
        break;

        case djvColorProfile::GAMMA:
        {
            uniform1f(
                program,
                "inColorProfileGamma",
                1.0 / options.colorProfile.gamma);
        }
        break;

        case djvColorProfile::EXPOSURE:
        {
            struct Exposure
            {
                double v, d, k, f;
            };
            
            Exposure exposure;

            exposure.v = djvMath::pow(
                2.0,
                options.colorProfile.exposure.value + 2.47393);
            exposure.d = options.colorProfile.exposure.defog;
            exposure.k = djvMath::pow(
                2.0,
                options.colorProfile.exposure.kneeLow);
            exposure.f = knee2(
                djvMath::pow(2.0, options.colorProfile.exposure.kneeHigh) -
                exposure.k,
                djvMath::pow(2.0, 3.5) - exposure.k);

            //DJV_DEBUG_PRINT("exposure");
            //DJV_DEBUG_PRINT("  v = " << exposure.v);
            //DJV_DEBUG_PRINT("  d = " << exposure.d);
            //DJV_DEBUG_PRINT("  k = " << exposure.k);
            //DJV_DEBUG_PRINT("  f = " << exposure.f);

            uniform1f(program, "inColorProfileExposure.v", exposure.v);
            uniform1f(program, "inColorProfileExposure.d", exposure.d);
            uniform1f(program, "inColorProfileExposure.k", exposure.k);
            uniform1f(program, "inColorProfileExposure.f", exposure.f);
        }
        break;

        default: break;
    }
}

} // namespace

//------------------------------------------------------------------------------
// displayProfileInit
//
// This function initializes the display profile.
//------------------------------------------------------------------------------

namespace
{

void displayProfileInit(
    const djvOpenGlImageOptions & options,
    GLuint                        program,
    djvOpenGlImageLut &           displayProfile)
{
    //DJV_DEBUG("displayProfileInit");

    // Color matrix.

    uniformMatrix4f(
        program,
        "inDisplayProfileColor",
        djvOpenGlImageColor::colorMatrix(options.displayProfile.color));

    // Levels in.

    uniform1f(
        program,
        "inDisplayProfileLevels.in0",
        options.displayProfile.levels.inLow);

    uniform1f(
        program,
        "inDisplayProfileLevels.in1",
        options.displayProfile.levels.inHigh -
            options.displayProfile.levels.inLow);

    // Gamma.

    uniform1f(
        program,
        "inDisplayProfileLevels.gamma",
        1.0 / options.displayProfile.levels.gamma);

    // Levels out.

    uniform1f(
        program,
        "inDisplayProfileLevels.out0",
        options.displayProfile.levels.outLow);

    uniform1f(
        program,
        "inDisplayProfileLevels.out1",
        options.displayProfile.levels.outHigh -
            options.displayProfile.levels.outLow);

    // Soft-clip.

    uniform1f(
        program,
        "inDisplayProfileSoftClip",
        options.displayProfile.softClip);

    // Lookup table.

    if (djvVectorUtil::isSizeValid(options.displayProfile.lut.size()))
    {
        activeTexture(GL_TEXTURE3);
        uniform1i(program, "inDisplayProfileLut", 3);
        displayProfile.init(options.displayProfile.lut);
    }
}

} // namespace

//------------------------------------------------------------------------------
// djvOpenGlImage::draw
//------------------------------------------------------------------------------

namespace
{

struct RestoreState
{
    ~RestoreState()
    {
        glUseProgram(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

} // namespace

void djvOpenGlImage::draw(
    const djvPixelData &          data,
    const djvOpenGlImageOptions & options,
    djvOpenGlImageState *         state) throw (djvError)
{
    //DJV_DEBUG("djvOpenGlImage::draw");
    //DJV_DEBUG_PRINT("data = " << data);
    //DJV_DEBUG_PRINT("color profile = " << options.colorProfile);
    
    RestoreState restoreState;

    djvOpenGlImageState defaultState;

    if (! state)
    {
        state = &defaultState;
    }

    const djvPixelDataInfo & info = data.info();

    const int proxyScale =
        options.proxyScale ?
        djvPixelDataUtil::proxyScale(info.proxy) :
        1;

    const djvVector2i scale = djvVectorUtil::ceil<double, int>(
        options.xform.scale * djvVector2f(info.size * proxyScale));

    const djvVector2i scaleTmp(scale.x, data.h());

    //DJV_DEBUG_PRINT("scale = " << scale);
    //DJV_DEBUG_PRINT("scale tmp = " << scaleTmp);

    // Initialize.

    const djvOpenGlImageFilter::FILTER filter =
        info.size == scale ? djvOpenGlImageFilter::NEAREST :
        (djvVectorUtil::area(scale) < djvVectorUtil::area(info.size) ?
         options.filter.min : options.filter.mag);

    //DJV_DEBUG_PRINT("filter min = " << options.filter.min);
    //DJV_DEBUG_PRINT("filter mag = " << options.filter.mag);
    //DJV_DEBUG_PRINT("filter = " << filter);

    if (! state->_init || state->_info != info || state->_options != options)
    {
        switch (filter)
        {
            case djvOpenGlImageFilter::NEAREST:
            case djvOpenGlImageFilter::LINEAR:
            {
                //DJV_DEBUG_PRINT("init single pass");

                state->_texture->init(
                    data.info(),
                    djvOpenGlImageFilter::toGl(filter),
                    djvOpenGlImageFilter::toGl(filter));

                state->_shader->init(
                    sourceVertex,
                    sourceFragment(
                        options.colorProfile.type,
                        options.displayProfile,
                        options.channel,
                        false,
                        0,
                        false));
            }
            break;

            case djvOpenGlImageFilter::BOX:
            case djvOpenGlImageFilter::TRIANGLE:
            case djvOpenGlImageFilter::BELL:
            case djvOpenGlImageFilter::BSPLINE:
            case djvOpenGlImageFilter::LANCZOS3:
            case djvOpenGlImageFilter::CUBIC:
            case djvOpenGlImageFilter::MITCHELL:
            {
                //DJV_DEBUG_PRINT("init two pass");

                state->_texture->init(
                    data.info(),
                    GL_NEAREST,
                    GL_NEAREST);

                // Initialize horizontal pass.

                djvPixelData contrib;

                scaleContrib(
                    data.w(),
                    scale.x,
                    filter,
                    contrib);

                state->_scaleXContrib->init(
                    contrib,
                    GL_NEAREST,
                    GL_NEAREST);

                state->_scaleXShader->init(
                    sourceVertex,
                    sourceFragment(
                        options.colorProfile.type,
                        djvOpenGlImageDisplayProfile(),
                        static_cast<djvOpenGlImageOptions::CHANNEL>(0),
                        true,
                        contrib.h(),
                        true));

                // Initialize vertical pass.

                scaleContrib(
                    data.h(),
                    scale.y,
                    filter,
                    contrib);

                state->_scaleYContrib->init(
                    contrib,
                    GL_NEAREST,
                    GL_NEAREST);

                state->_scaleYShader->init(
                    sourceVertex,
                    sourceFragment(
                        static_cast<djvColorProfile::PROFILE>(0),
                        options.displayProfile,
                        options.channel,
                        true,
                        contrib.h(),
                        false));
            }
            break;

            default: break;
        }

        state->_init    = true;
        state->_info    = info;
        state->_options = options;
    }

    // Render.

    const djvPixelDataInfo::Mirror mirror(
        info.mirror.x ? (! options.xform.mirror.x) : options.xform.mirror.x,
        info.mirror.y ? (! options.xform.mirror.y) : options.xform.mirror.y);

    //DJV_DEBUG_PRINT("mirror = " << mirror.x << " " << mirror.y);

    switch (filter)
    {
        case djvOpenGlImageFilter::NEAREST:
        case djvOpenGlImageFilter::LINEAR:
        {
            //DJV_DEBUG_PRINT("draw single pass");

            state->_shader->bind();

            // Initialize color and display profiles.

            colorProfileInit(
                options,
                state->_shader->program(),
                *state->_lutColorProfile);

            displayProfileInit(
                options,
                state->_shader->program(),
                *state->_lutDisplayProfile);

            // Draw.

            activeTexture(GL_TEXTURE0);

            uniform1i(state->_shader->program(), "inTexture", 0);

            state->_texture->copy(data);
            state->_texture->bind();

            DJV_DEBUG_OPEN_GL(glPushMatrix());
            const djvMatrix3f m = djvOpenGlImageXform::xformMatrix(options.xform);
            //DJV_DEBUG_PRINT("m = " << m);
            DJV_DEBUG_OPEN_GL(glLoadMatrixd(djvMatrixUtil::matrix4(m).e));

            quad(info.size, mirror, proxyScale);

            DJV_DEBUG_OPEN_GL(glPopMatrix());
        }
        break;

        case djvOpenGlImageFilter::BOX:
        case djvOpenGlImageFilter::TRIANGLE:
        case djvOpenGlImageFilter::BELL:
        case djvOpenGlImageFilter::BSPLINE:
        case djvOpenGlImageFilter::LANCZOS3:
        case djvOpenGlImageFilter::CUBIC:
        case djvOpenGlImageFilter::MITCHELL:
        {
            //DJV_DEBUG_PRINT("draw two pass");

            // Horizontal pass.

            djvOpenGlOffscreenBuffer buffer(
                djvPixelDataInfo(scaleTmp, data.pixel()));

            {
                djvOpenGlOffscreenBufferScope bufferScope(&buffer);

                state->_scaleXShader->bind();

                colorProfileInit(
                    options,
                    state->_scaleXShader->program(),
                    *state->_lutColorProfile);

                activeTexture(GL_TEXTURE0);

                uniform1i(state->_scaleXShader->program(), "inTexture", 0);

                state->_texture->copy(data);
                state->_texture->bind();

                activeTexture(GL_TEXTURE1);

                uniform1i(
                    state->_scaleXShader->program(), "inScaleContrib", 1);

                state->_scaleXContrib->bind();

                glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();

                djvOpenGlUtil::ortho(scaleTmp);
                glViewport(0, 0, scaleTmp.x, scaleTmp.y);
                quad(scaleTmp, mirror);

                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
                glPopAttrib();
            }

            // Vertical pass.

            state->_scaleYShader->bind();

            displayProfileInit(
                options,
                state->_scaleYShader->program(),
                *state->_lutDisplayProfile);

            activeTexture(GL_TEXTURE0);

            uniform1i(state->_scaleYShader->program(), "inTexture", 0);

            DJV_DEBUG_OPEN_GL(glBindTexture(GL_TEXTURE_2D, buffer.texture()));

            activeTexture(GL_TEXTURE1);

            uniform1i(state->_scaleYShader->program(), "inScaleContrib", 1);

            state->_scaleYContrib->bind();

            djvOpenGlImageXform xform = options.xform;
            xform.scale = djvVector2f(1.0);
            const djvMatrix3f m = djvOpenGlImageXform::xformMatrix(xform);

            DJV_DEBUG_OPEN_GL(glPushMatrix());
            DJV_DEBUG_OPEN_GL(glLoadMatrixd(djvMatrixUtil::matrix4(m).e));

            quad(scale);

            DJV_DEBUG_OPEN_GL(glPopMatrix());
        }
        break;

        default: break;
    }
}

const QStringList & djvOpenGlImage::errorLabels()
{
    static const QStringList data = QStringList() <<
        qApp->translate("djvOpenGlImage", "Cannot create texture") <<
        qApp->translate("djvOpenGlImage", "Cannot create texture: %1") <<
        qApp->translate("djvOpenGlImage", "Cannot create shader") <<
        qApp->translate("djvOpenGlImage", "Cannot compile shader:\n%1") <<
        qApp->translate("djvOpenGlImage", "Cannot create shader program") <<
        qApp->translate("djvOpenGlImage", "Cannot link shader:\n%1");

    DJV_ASSERT(ERROR_COUNT == data.count());
    
    return data;
}


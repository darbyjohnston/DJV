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

//! \file djvGlslTestScale.h

#ifndef DJV_GLSL_TEST_SCALE_H
#define DJV_GLSL_TEST_SCALE_H

#include <djvGlslTestOp.h>

class QRadioButton;

//------------------------------------------------------------------------------
// djvGlslTestScaleOp
//------------------------------------------------------------------------------

class djvGlslTestScaleOp : public djvGlslTestOp
{
    Q_OBJECT
    
public:

    enum TYPE
    {
        DEFAULT,
        CUSTOM
    };

    static const QStringList & typeLabels();

    enum FILTER_DEFAULT
    {
        NEAREST,
        LINEAR
    };

    static const QStringList & filterDefaultLabels();

    enum FILTER_CUSTOM
    {
        BOX,
        TRIANGLE,
        BELL,
        BSPLINE,
        LANCZOS3,
        CUBIC,
        MITCHELL
    };

    static const QStringList & filterCustomLabels();

    struct Value
    {
        Value() :
            size      (500, 500),
            type      (CUSTOM),
            defaultMin(LINEAR),
            defaultMag(LINEAR),
            customMin (LANCZOS3),
            customMag (MITCHELL)
        {}

        djvVector2i    size;
        TYPE           type;
        FILTER_DEFAULT defaultMin;
        FILTER_DEFAULT defaultMag;
        FILTER_CUSTOM  customMin;
        FILTER_CUSTOM  customMag;
    };

    struct Render
    {
        djvGlslTestOffscreen offscreen;
        djvGlslTestTexture   textureTmp;
        djvGlslTestTexture   contribX;
        djvGlslTestTexture   contribY;
        djvGlslTestShader    shader;
        djvGlslTestShader    shaderX;
        djvGlslTestShader    shaderY;
    };

    class StateDefault
    {
    public:

        StateDefault();

        StateDefault(const Value &);

        bool operator != (const StateDefault &) const;

    private:

        FILTER_DEFAULT _filterMin;
        FILTER_DEFAULT _filterMag;
        bool           _init;
    };

    class StateCustom
    {
    public:

        StateCustom();

        StateCustom(const Value &);

        bool operator != (const StateCustom &) const;

    private:

        djvVector2i   _size;
        FILTER_CUSTOM _filterMin;
        FILTER_CUSTOM _filterMag;
        bool          _init;
    };

    djvGlslTestScaleOp();

    QString name() const { return "Scale"; }

    void render(const djvImage &) throw (djvError);

    static int edge(int, int size);

    static GLuint filterToGl(FILTER_DEFAULT);

private Q_SLOTS:

    void widthCallback(int);
    void heightCallback(int);
    void typeCallback();
    void defaultMinCallback(int);
    void defaultMagCallback(int);
    void customMinCallback(int);
    void customMagCallback(int);

private:

    Value        _value;
    Render       _render;
    StateDefault _stateDefault;
    StateCustom  _stateCustom;
    
    QVector<QRadioButton *> _typeButton;
};

#endif // DJV_GLSL_TEST_SCALE_H

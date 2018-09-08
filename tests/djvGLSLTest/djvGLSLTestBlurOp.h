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

#pragma once

#include <djvGLSLTestAbstractOp.h>

#include <djvOpenGLShader.h>

//------------------------------------------------------------------------------
// djvGLSLTestBlurOp
//------------------------------------------------------------------------------

class djvGLSLTestBlurOp : public djvGLSLTestAbstractOp
{
    Q_OBJECT
    
public:

    enum FILTER
    {
        BOX,
        GAUSSIAN
    };

    static const QStringList & filterLabels();

    struct Values
    {
        Values();

        int    radius;
        FILTER filter;
        
        bool operator == (const Values &) const;
    };

    struct Render
    {

        djvGLSLTestOffscreen offscreen;
        djvOpenGLShader      shader;
        djvGLSLTestKernel    kernel;
        djvOpenGLTexture     textureTmp;
    };

    class State
    {
    public:

        State();
        
        State(const Values &);

        bool operator != (const State &) const;

    private:

        Values _values;
        bool   _init;
    };

    explicit djvGLSLTestBlurOp(djvGLSLTestContext *);

    const Values & values() const;

    virtual void render(const djvImage &) throw (djvError);

public Q_SLOTS:

    void setValues(const Values &);

private:

    Values _values;
    Render _render;
    State  _state;
};

//------------------------------------------------------------------------------
// djvGLSLTestBlurOpWidget
//------------------------------------------------------------------------------

class djvGLSLTestBlurOpWidget : public djvGLSLTestAbstractOpWidget
{
    Q_OBJECT
    
public:

    djvGLSLTestBlurOpWidget(djvGLSLTestBlurOp *, djvGLSLTestContext *);

private Q_SLOTS:

    void radiusCallback(int);
    void filterCallback(int);

private:

    djvGLSLTestBlurOp * _op;
};

//------------------------------------------------------------------------------
// djvGLSLTestBlurOpFactory
//------------------------------------------------------------------------------

class djvGLSLTestBlurOpFactory : public djvGLSLTestAbstractOpFactory
{
    Q_OBJECT
    
public:

    explicit djvGLSLTestBlurOpFactory(djvGLSLTestContext *);
    
    virtual QString name() const { return "Blur"; }

    virtual djvGLSLTestAbstractOp * createOp() const;
    
    virtual djvGLSLTestAbstractOpWidget * createWidget(
        djvGLSLTestAbstractOp *) const;
};


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
// djvGLSLTestLevelsOp
//------------------------------------------------------------------------------

class djvGLSLTestLevelsOp : public djvGLSLTestAbstractOp
{
    Q_OBJECT
    
public:

    struct Values
    {
        Values() :
            inputMin (0.f),
            inputMax (1.f),
            gamma    (1.f),
            outputMin(0.f),
            outputMax(1.f),
            softClip (0.f)
        {}

        float inputMin;
        float inputMax;
        float gamma;
        float outputMin;
        float outputMax;
        float softClip;
        
        bool operator == (const Values & other) const
        {
            return
                other.inputMin  == inputMin  &&
                other.inputMax  == inputMax  &&
                other.gamma     == gamma     &&
                other.outputMin == outputMin &&
                other.outputMax == outputMax &&
                other.softClip  == softClip;
        }
    };

    struct Render
    {
        djvOpenGLShader shader;
    };

    class State
    {
    public:

        State();
        
        State(const Values &);

        bool operator != (const State &) const;

    private:

        float _softClip;
        bool  _init;
    };

    explicit djvGLSLTestLevelsOp(djvGLSLTestContext *);

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
// djvGLSLTestLevelsOpWidget
//------------------------------------------------------------------------------

class djvGLSLTestLevelsOpWidget : public djvGLSLTestAbstractOpWidget
{
    Q_OBJECT
    
public:

    djvGLSLTestLevelsOpWidget(djvGLSLTestLevelsOp *, djvGLSLTestContext *);

private Q_SLOTS:

    void inputMinCallback(float);
    void inputMaxCallback(float);
    void gammaCallback(float);
    void outputMinCallback(float);
    void outputMaxCallback(float);
    void softClipCallback(float);

private:

    djvGLSLTestLevelsOp * _op;
};

//------------------------------------------------------------------------------
// djvGLSLTestLevelsOpFactory
//------------------------------------------------------------------------------

class djvGLSLTestLevelsOpFactory : public djvGLSLTestAbstractOpFactory
{
    Q_OBJECT
    
public:

    explicit djvGLSLTestLevelsOpFactory(djvGLSLTestContext *);
    
    virtual QString name() const { return "Levels"; }

    virtual djvGLSLTestAbstractOp * createOp() const;
    
    virtual djvGLSLTestAbstractOpWidget * createWidget(
        djvGLSLTestAbstractOp *) const;
};


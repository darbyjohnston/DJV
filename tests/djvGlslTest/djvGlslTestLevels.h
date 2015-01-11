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

//! \file djvGlslTestLevels.h

#ifndef DJV_GLSL_TEST_LEVELS_H
#define DJV_GLSL_TEST_LEVELS_H

#include <djvGlslTestOp.h>

//------------------------------------------------------------------------------
// djvGlslTestLevelsOp
//------------------------------------------------------------------------------

class djvGlslTestLevelsOp : public djvGlslTestOp
{
    Q_OBJECT
    
public:

    struct Value
    {
        Value() :
            inputMin (0.0),
            inputMax (1.0),
            gamma    (1.0),
            outputMin(0.0),
            outputMax(1.0),
            softClip (0.0)
        {}

        double inputMin;
        double inputMax;
        double gamma;
        double outputMin;
        double outputMax;
        double softClip;
    };

    struct Render
    {
        djvGlslTestShader shader;
    };

    class State
    {
    public:

        State();
        
        State(const Value &);

        bool operator != (const State &) const;

    private:

        double _softClip;
        bool   _init;
    };

    djvGlslTestLevelsOp();

    QString name() const { return "Levels"; }

    void render(const djvImage &) throw (djvError);

private Q_SLOTS:

    void inputMinCallback(double);
    void inputMaxCallback(double);
    void gammaCallback(double);
    void outputMinCallback(double);
    void outputMaxCallback(double);
    void softClipCallback(double);

private:

    Value  _value;
    Render _render;
    State  _state;
};

#endif // DJV_GLSL_TEST_LEVELS_H

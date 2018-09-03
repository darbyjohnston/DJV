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

#include <djvGlslTestAbstractOp.h>

#include <djvOpenGlShader.h>

//------------------------------------------------------------------------------
// djvGlslTestSharpenOp
//------------------------------------------------------------------------------

class djvGlslTestSharpenOp : public djvGlslTestAbstractOp
{
    Q_OBJECT
    
public:

    struct Values
    {
        Values() :
            value(1.0)
        {}

        double value;
        
        bool operator == (const Values & other) const
        {
            return other.value == value;
        }
    };

    explicit djvGlslTestSharpenOp(djvGlslTestContext *);

    const Values & values() const;

    virtual void render(const djvImage &) throw (djvError);

public Q_SLOTS:

    void setValues(const Values &);

private:

    Values          _values;
    djvOpenGlShader _shader;
    bool            _init;
};

//------------------------------------------------------------------------------
// djvGlslTestSharpenOpWidget
//------------------------------------------------------------------------------

class djvGlslTestSharpenOpWidget : public djvGlslTestAbstractOpWidget
{
    Q_OBJECT
    
public:

    djvGlslTestSharpenOpWidget(djvGlslTestSharpenOp *, djvGlslTestContext *);

private Q_SLOTS:

    void valueCallback(double);

private:

    djvGlslTestSharpenOp * _op;
};

//------------------------------------------------------------------------------
// djvGlslTestSharpenOpFactory
//------------------------------------------------------------------------------

class djvGlslTestSharpenOpFactory : public djvGlslTestAbstractOpFactory
{
    Q_OBJECT
    
public:

    explicit djvGlslTestSharpenOpFactory(djvGlslTestContext *);
    
    virtual QString name() const { return "Sharpen"; }

    virtual djvGlslTestAbstractOp * createOp() const;
    
    virtual djvGlslTestAbstractOpWidget * createWidget(
        djvGlslTestAbstractOp *) const;
};


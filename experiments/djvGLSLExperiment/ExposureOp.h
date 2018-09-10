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

#include <AbstractOp.h>

#include <djvGraphics/OpenGLShader.h>

//------------------------------------------------------------------------------
// ExposureOp
//------------------------------------------------------------------------------

class ExposureOp : public AbstractOp
{
    Q_OBJECT
    
public:
    struct Values
    {
        Values() :
            exposure(0.f),
            defog   (0.f),
            kneeLow (0.f),
            kneeHigh(5.f),
            gamma   (2.2f)
        {}

        float exposure;
        float defog;
        float kneeLow;
        float kneeHigh;
        float gamma;
        
        bool operator == (const Values & other) const
        {
            return
                other.exposure == exposure &&
                other.defog    == defog    &&
                other.kneeLow  == kneeLow  &&
                other.kneeHigh == kneeHigh &&
                other.gamma    == gamma;
        }
    };

    explicit ExposureOp(Context *);

    const Values & values() const;

    virtual void render(const djvImage &) throw (djvError);

public Q_SLOTS:
    void setValues(const Values &);

private:
    Values          _values;
    djvOpenGLShader _shader;
    bool            _init;
};

//------------------------------------------------------------------------------
// ExposureOpWidget
//------------------------------------------------------------------------------

class ExposureOpWidget : public AbstractOpWidget
{
    Q_OBJECT
    
public:
    ExposureOpWidget(ExposureOp *, Context *);

private Q_SLOTS:
    void exposureCallback(float);
    void defogCallback(float);
    void kneeLowCallback(float);
    void kneeHighCallback(float);
    void gammaCallback(float);

private:
    ExposureOp * _op;
};

//------------------------------------------------------------------------------
// ExposureOpFactory
//------------------------------------------------------------------------------

class ExposureOpFactory : public AbstractOpFactory
{
    Q_OBJECT
    
public:
    explicit ExposureOpFactory(Context *);
    
    virtual QString name() const { return "Exposure"; }
    virtual AbstractOp * createOp() const;
    virtual AbstractOpWidget * createWidget(AbstractOp *) const;
};


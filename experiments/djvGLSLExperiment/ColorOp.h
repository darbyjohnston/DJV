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
// ColorOp
//------------------------------------------------------------------------------

class ColorOp : public AbstractOp
{
    Q_OBJECT
    
public:
    struct Values
    {
        Values() :
            brightness(2.f),
            contrast  (1.f),
            saturation(1.f)
        {}

        float brightness;
        float contrast;
        float saturation;
        
        bool operator == (const Values & other) const
        {
            return
                other.brightness == brightness  &&
                other.contrast   == contrast    &&
                other.saturation == saturation;
        }
    };

    explicit ColorOp(Context *);

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
// ColorOpWidget
//------------------------------------------------------------------------------

class ColorOpWidget : public AbstractOpWidget
{
    Q_OBJECT
    
public:
    ColorOpWidget(ColorOp *, Context *);

private Q_SLOTS:
    void brightnessCallback(float);
    void contrastCallback(float);
    void saturationCallback(float);

private:
    ColorOp * _op;
};

//------------------------------------------------------------------------------
// ColorOpFactory
//------------------------------------------------------------------------------

class ColorOpFactory : public AbstractOpFactory
{
    Q_OBJECT
    
public:
    explicit ColorOpFactory(Context *);
    
    virtual QString name() const { return "Color"; }
    virtual AbstractOp * createOp() const;
    virtual AbstractOpWidget * createWidget(AbstractOp *) const;
};


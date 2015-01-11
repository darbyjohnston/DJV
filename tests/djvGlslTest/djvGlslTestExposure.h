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

//! \file djvGlslTestExposure.h

#ifndef DJV_GLSL_TEST_EXPOSURE_H
#define DJV_GLSL_TEST_EXPOSURE_H

#include <djvGlslTestOp.h>

//------------------------------------------------------------------------------
// djvGlslTestExposureOp
//------------------------------------------------------------------------------

class djvGlslTestExposureOp : public djvGlslTestOp
{
    Q_OBJECT
    
public:

    struct Value
    {
        Value() :
            exposure(0.0),
            defog   (0.0),
            kneeLow (0.0),
            kneeHigh(5.0),
            gamma   (2.2)
        {}

        double exposure;
        double defog;
        double kneeLow;
        double kneeHigh;
        double gamma;
    };

    djvGlslTestExposureOp();

    QString name() const { return "Exposure"; }

    void render(const djvImage &) throw (djvError);

private Q_SLOTS:

    void exposureCallback(double);
    void defogCallback(double);
    void kneeLowCallback(double);
    void kneeHighCallback(double);
    void gammaCallback(double);

private:

    Value             _value;
    djvGlslTestShader _shader;
    bool              _init;
};

#endif // DJV_GLSL_TEST_EXPOSURE_H

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

#include <djvGLSLTestOpsManager.h>

#include <djvGLSLTestBlurOp.h>
#include <djvGLSLTestColorOp.h>
#include <djvGLSLTestContext.h>
#include <djvGLSLTestEdgeOp.h>
#include <djvGLSLTestExposureOp.h>
#include <djvGLSLTestLevelsOp.h>
#include <djvGLSLTestNullOp.h>
#include <djvGLSLTestScaleOp.h>
#include <djvGLSLTestSharpenOp.h>

//------------------------------------------------------------------------------
// djvGLSLTestOpsManager
//------------------------------------------------------------------------------

djvGLSLTestOpsManager::djvGLSLTestOpsManager(
    djvGLSLTestContext * context,
    QObject *            parent) :
    QObject(parent),
    _currentIndex(0)
{
    _factories += new djvGLSLTestNullOpFactory(context);
    _factories += new djvGLSLTestColorOpFactory(context);
    _factories += new djvGLSLTestLevelsOpFactory(context);
    _factories += new djvGLSLTestExposureOpFactory(context);
    _factories += new djvGLSLTestBlurOpFactory(context);
    _factories += new djvGLSLTestSharpenOpFactory(context);
    _factories += new djvGLSLTestEdgeOpFactory(context);
    _factories += new djvGLSLTestScaleOpFactory(context);    
    Q_FOREACH(djvGLSLTestAbstractOpFactory * factory, _factories)
    {
        _ops += factory->createOp();
    }
}

djvGLSLTestOpsManager::~djvGLSLTestOpsManager()
{
    Q_FOREACH(djvGLSLTestAbstractOp * op, _ops)
    {
        delete op;
    }
    Q_FOREACH(djvGLSLTestAbstractOpFactory * factory, _factories)
    {
        delete factory;
    }
}

const QList<djvGLSLTestAbstractOpFactory *> djvGLSLTestOpsManager::factories() const
{
    return _factories;
}

const QList<djvGLSLTestAbstractOp *> djvGLSLTestOpsManager::ops() const
{
    return _ops;
}

int djvGLSLTestOpsManager::currentIndex() const
{
    return _currentIndex;
}
    
djvGLSLTestAbstractOpFactory * djvGLSLTestOpsManager::currentFactory() const
{
    return _factories[_currentIndex];
}
    
djvGLSLTestAbstractOp * djvGLSLTestOpsManager::currentOp() const
{
    return _ops[_currentIndex];
}

void djvGLSLTestOpsManager::setCurrentIndex(int index)
{
    if (_currentIndex == index)
        return;
    disconnect(currentOp(), SIGNAL(opChanged()), this, SIGNAL(opChanged()));
    _currentIndex = index;
    connect(currentOp(), SIGNAL(opChanged()), SIGNAL(opChanged()));
    Q_EMIT currentIndexChanged(_currentIndex);
}

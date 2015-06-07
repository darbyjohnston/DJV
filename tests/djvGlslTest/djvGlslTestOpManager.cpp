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

//! \file djvGlslTestOpManager.cpp

#include <djvGlslTestOpManager.h>

#include <djvGlslTestContext.h>
#include <djvGlslTestOp.h>

#include <djvGlslTestColor.h>
#include <djvGlslTestLevels.h>
#include <djvGlslTestExposure.h>
#include <djvGlslTestBlur.h>
#include <djvGlslTestSharpen.h>
#include <djvGlslTestEdge.h>
#include <djvGlslTestScale.h>

//------------------------------------------------------------------------------
// djvGlslTestOpManager
//------------------------------------------------------------------------------

djvGlslTestOpManager::djvGlslTestOpManager(
    djvGlslTestContext * context,
    QObject *            parent) :
    QObject(parent),
    _currentIndex(-1)
{
    _list += new djvGlslTestNullOp(context);
    _list += new djvGlslTestColorOp(context);
    _list += new djvGlslTestLevelsOp(context);
    _list += new djvGlslTestExposureOp(context);
    _list += new djvGlslTestBlurOp(context);
    _list += new djvGlslTestSharpenOp(context);
    _list += new djvGlslTestEdgeOp(context);
    _list += new djvGlslTestScaleOp(context);
}

djvGlslTestOpManager::~djvGlslTestOpManager()
{
    Q_FOREACH(djvGlslTestOp * op, _list)
    {
        delete op;
    }
}

const QList<djvGlslTestOp *> djvGlslTestOpManager::list() const
{
    return _list;
}

int djvGlslTestOpManager::currentIndex() const
{
    return _currentIndex;
}

djvGlslTestOp * djvGlslTestOpManager::currentOp() const
{
    return (_currentIndex >= 0 && _currentIndex < _list.count()) ?
        _list[_currentIndex] :
        0;
}

void djvGlslTestOpManager::setCurrentIndex(int index)
{
    if (_currentIndex == index)
        return;
    
    _currentIndex = index;
    
    Q_EMIT currentIndexChanged(_currentIndex);
}

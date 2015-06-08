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

//! \file djvGlslTestViewWidget

#include <djvGlslTestViewWidget.h>

#include <djvGlslTestAbstractOp.h>
#include <djvGlslTestContext.h>
#include <djvGlslTestImageLoad.h>
#include <djvGlslTestOpsManager.h>
#include <djvGlslTestPlayback.h>

#include <djvDebugLog.h>
#include <djvErrorUtil.h>

//------------------------------------------------------------------------------
// djvGlslTestViewWidget
//------------------------------------------------------------------------------

djvGlslTestViewWidget::djvGlslTestViewWidget(
    djvGlslTestImageLoad *  imageLoad,
    djvGlslTestOpsManager * opsManager,
    djvGlslTestPlayback *   playback,
    djvGlslTestContext *    context) :
    _imageLoad (imageLoad),
    _opsManager(opsManager),
    _playback  (playback),
    _context   (context)
{
    connect(
        opsManager,
        SIGNAL(currentIndexChanged(int)),
        SLOT(updateGL()));
    
    connect(
        opsManager,
        SIGNAL(opChanged()),
        SLOT(updateGL()));

    connect(
        playback,
        SIGNAL(frameChanged(qint64)),
        SLOT(updateGL()));
}

void djvGlslTestViewWidget::showEvent(QShowEvent *)
{
    //viewFit();
}

void djvGlslTestViewWidget::paintGL()
{
    djvOpenGlWidget::paintGL();

    if (djvGlslTestAbstractOp * op = _opsManager->currentOp())
    {
        if (const djvImage * image = _imageLoad->image(_playback->frame()))
        {
            try
            {
                op->render(*image);
            }
            catch (const djvError & error)
            {
                DJV_LOG(_context->debugLog(), "djvGlslTestWidget",
                    djvErrorUtil::format(error).join("\n"));
            }
        }
    }
}

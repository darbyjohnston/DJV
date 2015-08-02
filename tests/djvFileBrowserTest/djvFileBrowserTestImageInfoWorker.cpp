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

//! \file djvFileBrowserTestImageInfoWorker.cpp

#include <djvFileBrowserTestImageInfoWorker.h>

#include <djvDebug.h>
#include <djvImageContext.h>

#include <QScopedPointer>

djvFileBrowserTestImageInfoWorker::djvFileBrowserTestImageInfoWorker(
    djvImageContext * context,
    QObject *         parent) :
    QObject(parent),
    _context(context)
{}
    
djvFileBrowserTestImageInfoWorker::~djvFileBrowserTestImageInfoWorker()
{}

void djvFileBrowserTestImageInfoWorker::info(
    const djvFileInfo & fileInfo,
    int                 row,
    quint64             id)
{
    //DJV_DEBUG("djvFileBrowserTestImageInfoWorker::info");
    //DJV_DEBUG_PRINT("fileInfo = " << fileInfo);
    //DJV_DEBUG_PRINT("row = " << row);
    //DJV_DEBUG_PRINT("id = " << id);

    djvImageIoInfo info;

    QScopedPointer<djvImageLoad> load;
    
    try
    {
        load.reset(_context->imageIoFactory()->load(fileInfo, info));
    }
    catch (djvError error)
    {}

    Q_EMIT infoFinished(info, row, id);
}

void djvFileBrowserTestImageInfoWorker::finish()
{
    //DJV_DEBUG("djvFileBrowserTestImageInfoWorker::finish");
}


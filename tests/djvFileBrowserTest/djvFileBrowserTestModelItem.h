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

//! \file djvFileBrowserTestModelItem.h

#ifndef DJV_FILE_BROWSER_TEST_MODEL_ITEM_H
#define DJV_FILE_BROWSER_TEST_MODEL_ITEM_H

#include <djvFileInfo.h>

#include <QSize>
#include <QVariant>

//------------------------------------------------------------------------------
//! \struct djvFileBrowserTestModelItem
//!
//! This struct provides a file browser data model item.
//------------------------------------------------------------------------------

struct djvFileBrowserTestModelItem
{
    //! Constructor.
    
    djvFileBrowserTestModelItem() :
        infoInit         (false),
        infoComplete     (false),
        thumbnailInit    (false),
        thumbnailComplete(false)
    {}
    
    djvFileInfo fileInfo;          //!< File information.
    QVariant    text;              //!< Display role text.
    bool        infoInit;          //!< Whether the image info is initialized.
    bool        infoComplete;      //!< Whether the image info is complete.
    bool        thumbnailInit;     //!< Whether the image thumbnail is initialized.
    bool        thumbnailComplete; //!< Whether the image thumbnail is complete.
    QVariant    thumbnail;         //!< Image thumbnail.
    QSize       thumbnailSize;     //!< Image thumbnail size.
};

#endif // DJV_FILE_BROWSER_TEST_MODEL_ITEM_H


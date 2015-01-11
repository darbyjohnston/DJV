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

//! \file djvPicPlugin.h

#ifndef DJV_PIC_PLUGIN_H
#define DJV_PIC_PLUGIN_H

#include <djvImageIo.h>

//! \addtogroup plugins
//@{

//!\defgroup djvPicPlugin djvPicPlugin
//!
//! This plugin provides support for the Softimage image file format.
//!
//! File extensions: .pic
//!
//! Supported features:
//!
//! - 8-bit, RGB, RGBA, RGB plus Alpha
//! - File compression
//! - Read only
//!
//! References:
//!
//! - Softimage, "INFO: PIC file format"
//!   http://xsi.wiki.avid.com/index.php/INFO:_PIC_file_format

//@} // plugins

//! \addtogroup djvPicPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvPicPlugin
//!
//! This class provides a PIC plugin.
//------------------------------------------------------------------------------

class djvPicPlugin : public djvImageIo
{
public:

    //! The plugin name.

    static const QString staticName;

    //! This enumeration provides the file types.

    enum TYPE
    {
        TYPE_RGB,
        TYPE_RGBA,
        TYPE_RGB_A,

        TYPE_COUNT
    };

    //! This enumeration provides the file compression.

    enum COMPRESSION
    {
        COMPRESSION_NONE,
        COMPRESSION_RLE,

        COMPRESSION_COUNT
    };

    //! Get the compression labels.

    static const QStringList & compressionLabels();

    //! Load RLE data.

    static const quint8 * readRle(
        const quint8 * in,
        const quint8 * end,
        quint8 *       out,
        int            size,
        int            channels,
        int            stride,
        bool           endian);
    
    virtual djvPlugin * copyPlugin() const;

    virtual QString pluginName() const;

    virtual QStringList extensions() const;
    
    virtual djvImageLoad * createLoad() const;
};

//@} // djvPicPlugin

#endif // DJV_PIC_PLUGIN_H


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

//! \file djvIflPlugin.h

#ifndef DJV_IFL_PLUGIN_H
#define DJV_IFL_PLUGIN_H

#include <djvImageIo.h>

//! \addtogroup plugins
//@{

//! \defgroup djvIflPlugin djvIflPlugin
//!
//! This plugin provides support for the Autodesk Image File List (IFL) format.
//! IFL is a file format for creating sequences or playlists of other image
//! files. An IFL file simply consists of a list of image file names, one per
//! line.
//!
//! File extensions: .ifl
//!
//! Supported features:
//!
//! - Read only

//@} // plugins

//! \addtogroup djvIflPlugin
//@{

//------------------------------------------------------------------------------
//! \class djvIflPlugin
//!
//! This class provides an IFL plugin.
//------------------------------------------------------------------------------

class djvIflPlugin : public djvImageIo
{
public:

    //! Plugin name.
    
    static const QString staticName;
    
    virtual djvPlugin * copyPlugin() const;

    virtual QString pluginName() const;

    virtual QStringList extensions() const;
    
    virtual djvImageLoad * createLoad() const;
};

//@} // djvIflPlugin

#endif // DJV_IFL_PLUGIN_H


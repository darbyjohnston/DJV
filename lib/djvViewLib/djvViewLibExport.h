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

//! \file djvViewLibExport.h

#ifndef DJV_VIEW_LIB_EXPORT_H
#define DJV_VIEW_LIB_EXPORT_H

//! \addtogroup lib
//@{

//! \defgroup djvViewLib djvViewLib
//!
//! This library provides functionality for the djv_view application.

//@} // lib

//! \addtogroup djvViewLib
//@{

//! \defgroup djvViewFile djvViewFile
//!
//! This module provides file functionality.

//! \defgroup djvViewHelp djvViewHelp
//!
//! This module provides help functionality.

//! \defgroup djvViewImage djvViewImage
//!
//! This module provides image functionality.

//! \defgroup djvViewMisc djvViewMisc
//!
//! This module provides miscellaneous functionality.

//! \defgroup djvViewPlayback djvViewPlayback
//!
//! This module provides playback functionality.

//! \defgroup djvViewTool djvViewTool
//!
//! This module provides tool functionality.

//! \defgroup djvViewView djvViewView
//!
//! This module provides view functionality.

//! \defgroup djvViewWindow djvViewWindow
//!
//! This module provides window functionality.

//@} // djvCore

//! \addtogroup djvViewLib
//@{

#if defined(DJV_WINDOWS)

#if defined(djvViewLib_EXPORTS)

//! This define provides the Windows template export.

#define DJV_VIEW_LIB_EXPORT __declspec(dllexport)

#else // djvViewLib_EXPORTS

//! This define provides the Windows template export.

#define DJV_VIEW_LIB_EXPORT __declspec(dllimport)

#endif // djvViewLib_EXPORTS

#else // DJV_WINDOWS

#define DJV_VIEW_LIB_EXPORT

#endif // DJV_WINDOWS

//@} // djvViewLibMisc

#endif // DJV_VIEW_LIB_EXPORT_H


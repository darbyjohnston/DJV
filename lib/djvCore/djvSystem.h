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

//! \file djvSystem.h

#ifndef DJV_SYSTEM_H
#define DJV_SYSTEM_H

#include <djvConfig.h>
#include <djvCoreExport.h>

class QString;
class QStringList;

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvSystem
//!
//! This class provides system utilities.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvSystem
{
public:

    //! Destructor.
    
    virtual ~djvSystem() = 0;
    
    //! Get system information.

    static QString info();
    
    //! Get the DJV_PATH environment variable name.
    
    static const QString & djvPathEnv();
    
    //! Get the LD_LIBRARY_PATH environment variable name.
    
    static const QString & ldLibPathEnv();

    //! Get the search path. The default search path consists of:
    //!
    //! - The DJV_PATH environment variable
    //! - The LD_LIBRARY_PATH environment variable
    //! - The application path
    //! - The current directory

    static QStringList searchPath();
    
    //! Find a file in the search path. If the file is found the path to the
    //! file is returned, otherwise an empty string is returned.
    
    static QString findFile(const QString & fileName);

    //! Get the width of the terminal.

    static int terminalWidth();

    //! Print a message to the terminal.

    static void print(const QString &, bool newline = true, int indent = 0);

    //! Execute a command.

    static int exec(const QString &);

    //! Get an environment variable.

    static QString env(const QString &);

    //! Set an environment variable.

    static bool setEnv(const QString & var, const QString & value);
    
    //! Get the list of drives.
    
    static QStringList drives();
};

//@} // djvCoreMisc

#endif // DJV_SYSTEM_H


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

//! \file djv_ls.h

#ifndef DJV_LS_H
#define DJV_LS_H

#include <djvCoreApplication.h>
#include <djvFileInfo.h>
#include <djvFileInfoUtil.h>

//! \addtogroup bin
//@{

//! \defgroup djv_ls djv_ls
//!
//! This program provides a command line tool for listing directories with file
//! sequences.

//@} // bin

//! \addtogroup djv_ls
//@{

//------------------------------------------------------------------------------
//! \class djvLsApplication
//!
//! This class provides the application.
//------------------------------------------------------------------------------

class djvLsApplication : public djvCoreApplication
{
    Q_OBJECT
    
public:

    //! Constructor.

    djvLsApplication(int, char **) throw (djvError);
    
    //! Parse the command line.

    void commandLine(QStringList &) throw (QString);
    
    //! This enumeration provides error codes.
    
    enum ERROR
    {
        ERROR_OPEN,
        
        ERROR_COUNT
    };
    
    //! Get the error code labels.
    
    static const QStringList & errorLabels();

    virtual QString commandLineHelp() const;

private Q_SLOTS:

    void debugLogCallback(const QString &);

private:

    void process(djvFileInfoList &);

    void printItem(const djvFileInfo &, bool path = false, bool info = true);

    bool printDirectory(const djvFileInfo &, bool label);

    QStringList           _input;
    bool                  _info;
    bool                  _filePath;
    djvSequence::COMPRESS _sequence;
    bool                  _recurse;
    bool                  _hidden;
    QStringList           _glob;
    int                   _columns;
    djvFileInfoUtil::SORT _sort;
    bool                  _reverseSort;
    bool                  _doNotSortDirs;
};

//@} // djv_ls

#endif // DJV_LS_H


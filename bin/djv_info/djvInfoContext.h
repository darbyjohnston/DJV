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

#pragma once

#include <djvGraphics/GraphicsContext.h>

#include <djvCore/Sequence.h>

//! \addtogroup djv_info
//@{

//------------------------------------------------------------------------------
//! \class djvInfoContext
//!
//! This class provides global functionality for the application.
//------------------------------------------------------------------------------

class djvInfoContext : public djvGraphicsContext
{
    Q_OBJECT

public:
    explicit djvInfoContext(QObject * parent = nullptr);

    virtual ~djvInfoContext();

    //! Get the list of inputs.    
    const QStringList & input() const;
    
    //! Get whether to show image information.
    bool hasInfo() const;
    
    //! Get whether to show verbose information.
    bool hasVerbose() const;
    
    //! Get whether to show file paths.
    bool hasFilePath() const;
    
    //! Get the file sequencing.
    djvSequence::COMPRESS sequence() const;
    
    // Get whether to descend into sub-directories.
    bool hasRecurse() const;

    //! Get the number of columns for formatting the output.
    int columns() const;

protected:
    virtual bool commandLineParse(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;

private:
    QStringList           _input;
    bool                  _info;
    bool                  _verbose;
    bool                  _filePath;
    djvSequence::COMPRESS _sequence;
    bool                  _recurse;
    int                   _columns;
};

//@} // djv_info


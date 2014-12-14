//------------------------------------------------------------------------------
// Copyright (c) 2004-2014 Darby Johnston
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

//! \file djvCoreApplication.h

#ifndef DJV_CORE_APPLICATION_H
#define DJV_CORE_APPLICATION_H

#include <djvError.h>
#include <djvStringUtil.h>

#include <QCoreApplication>
#include <QStringList>

//! \addtogroup djvCoreMisc
//@{

//------------------------------------------------------------------------------
//! \class djvAbstractCoreApplication
//!
//! This class provides the base functionality for applications.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvAbstractCoreApplication
{
public:

    //! Constructor.

    djvAbstractCoreApplication(const QString & name, int & argc, char ** argv)
        throw (djvError);

    //! Destructor.

    virtual ~djvAbstractCoreApplication() = 0;

    //! This enumeration provides the exit value.

    enum EXIT_VALUE
    {
        EXIT_VALUE_DEFAULT,
        EXIT_VALUE_ERROR,
        EXIT_VALUE_HELP,
        EXIT_VALUE_INFO,
        EXIT_VALUE_ABOUT,
        
        EXIT_VALUE_COUNT
    };
    
    //! Get the exit value labels.

    static const QStringList & exitValueLabels();

    //! Start the application.

    virtual int run();

    //! Get the exit value.

    EXIT_VALUE exitValue() const;

    //! Set the exit value.

    virtual void setExitValue(EXIT_VALUE);

    //! Get the application name.

    static const QString & name();

    //! Get the documentation path.

    QString docPath() const;

    //! Get application information.

    virtual QString info() const;

    //! Get application about information.

    virtual QString about() const;
    
    //! Print a message.

    virtual void printMessage(const QString &, int indent = 0) const;

    //! Print an error.

    virtual void printError(const djvError &) const;

    //! Low-level printing function used internally by printMessage() and
    //! printError().

    void print(const QString &, bool newline = true, int indent = 0) const;

    //! Print a separator.

    void printSeparator() const;

    //! Parse the comamand line.

    virtual void commandLine(QStringList &) throw (djvError);

    //! Get the command line name.

    const QString & commandLineName() const;

    //! Get the command line help.

    virtual QString commandLineHelp() const;
    
    //! Get the global application instance.
    
    static djvAbstractCoreApplication * global();

protected:

    QStringList _commandLineArgs;

    static const QString errorCommandLine;

private:
    
    DJV_PRIVATE_IMPLEMENTATION();
    
    static QString _name;
};

//------------------------------------------------------------------------------
//! \class djvCoreApplication
//!
//! This class provides the base functionality for applications.
//------------------------------------------------------------------------------

class DJV_CORE_EXPORT djvCoreApplication :
    public QCoreApplication,
    public djvAbstractCoreApplication
{
public:

    //! Constructor.

    djvCoreApplication(const QString & name, int & argc, char ** argv)
        throw (djvError);
};

//------------------------------------------------------------------------------

//! Get the global application instance.

#define DJV_CORE_APP djvAbstractCoreApplication::global()

DJV_STRING_OPERATOR(DJV_CORE_EXPORT, djvAbstractCoreApplication::EXIT_VALUE);

//@} // djvCoreMisc

#endif // DJV_CORE_APPLICATION_H


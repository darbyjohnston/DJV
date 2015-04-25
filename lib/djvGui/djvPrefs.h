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

//! \file djvPrefs.h

#ifndef DJV_PREFS_H
#define DJV_PREFS_H

#include <djvGuiExport.h>

#include <djvUtil.h>

#include <QMetaType>

struct djvPrefsPrivate;

class QString;
class QStringList;

//! \addtogroup djvGuiMisc
//@{

//------------------------------------------------------------------------------
//! \class djvPrefs
//!
//! This class provides preferences.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvPrefs
{
    Q_GADGET
    Q_ENUMS(SCOPE)
    
public:

    //! This enumeration provides the preferences scope.
    
    enum SCOPE
    {
        USER,
        SYSTEM
    };

    //! Constructor.

    djvPrefs(const QString & group, SCOPE = USER);

    //! Destructor.

    ~djvPrefs();

    //! Get the preferences scope.

    SCOPE scope() const;

    //! Get a preference.

    template<typename T>
    inline bool get(const QString &, T &) const;

    //! Set a preference.

    template<typename T>
    inline void set(const QString &, const T &);

    //! Remove a preference.

    void remove(const QString &);

    //! Check whether a preference exists.

    bool contains(const QString &) const;

    //! Get the list of preferences.

    QStringList list() const;

    //! Get the preferences file name.

    QString fileName() const;

    //! Get whether preferences are in reset mode. Reset mode disables the
    //! reading of preferences.

    static bool hasReset();

    //! Set whether preferences are in reset mode. Reset mode disables the
    //! reading of preferences.

    static void setReset(bool);

private:

    bool _get(const QString &, QStringList &) const;

    void _set(const QString &, const QStringList &);
    
    DJV_PRIVATE_COPY(djvPrefs);
    
    djvPrefsPrivate * _p;
};

//@} // djvGuiMisc

#include <djvPrefsInline.h>

#endif // DJV_PREFS_H


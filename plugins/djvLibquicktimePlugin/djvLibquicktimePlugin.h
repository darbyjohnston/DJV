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

//! \file djvLibquicktimePlugin.h

#ifndef DJV_LIBQUICKTIME_PLUGIN_H
#define DJV_LIBQUICKTIME_PLUGIN_H

#include <djvImageIo.h>

#include <lqt/colormodels.h>
#include <lqt/lqt.h>

//! \addtogroup plugins
//@{

//!\defgroup djvLibquicktimePlugin djvLibquicktimePlugin
//!
//! This plugin provides support for libquicktime, an open source library for
//! reading and writing movies.
//!
//! Requires:
//!
//! - libquicktime - http://libquicktime.sourceforge.net
//!
//! File extensions: .qt, .mov, .mp4
//!
//! Supported features:
//!
//! - 8-bit RGBA
//! - File compression

//@} // plugins

//! \addtogroup djvLibquicktimePlugin
//@{

//------------------------------------------------------------------------------
//! \class djvLibquicktimePlugin
//!
//! This class provides a Libquicktime plugin.
//------------------------------------------------------------------------------

class djvLibquicktimePlugin : public djvImageIo
{
public:

    //! Plugin name.
    
    static const QString staticName;

    //! Get the codec labels.

    static QStringList codecLabels();

    //! Get the codec text labels.

    static QStringList codecTextLabels();

    //! Options.

    enum OPTIONS
    {
        CODEC,

        OPTIONS_COUNT
    };

    //! Get the option labels.

    static const QStringList & optionsLabels();

    //! This struct provides options.

    struct Options
    {
        //! Constructor.

        Options();

        QString codec;
    };
    
    virtual void initPlugin() throw (djvError);

    virtual djvPlugin * copyPlugin() const;

    virtual QString pluginName() const;

    virtual QStringList extensions() const;

    virtual bool isSeq() const;

    virtual QStringList option(const QString &) const;

    virtual bool setOption(const QString &, QStringList &);

    virtual QStringList options() const;

    virtual void commandLine(QStringList &) throw (QString);

    virtual QString commandLineHelp() const;
    
    virtual djvImageLoad * createLoad() const;
    
    virtual djvImageSave * createSave() const;

    virtual djvAbstractPrefsWidget * createWidget();

private:

    Options _options;
};

//@} // djvLibquicktimePlugin

#endif // DJV_LIBQUICKTIME_PLUGIN_H


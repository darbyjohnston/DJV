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

//! \file djvImageIoWidget.h

#ifndef DJV_IMAGE_IO_WIDGET_H
#define DJV_IMAGE_IO_WIDGET_H

#include <djvAbstractPrefsWidget.h>

#include <djvPlugin.h>
#include <djvSystem.h>

#include <QWidget>

class djvGuiContext;

class djvImageIo;

//! \addtogroup djvGuiWidget
//@{

//------------------------------------------------------------------------------
//! \class djvImageIoWidget
//!
//! This class provides the base functionality for image I/O widgets.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvImageIoWidget : public djvAbstractPrefsWidget
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvImageIoWidget(
        djvImageIo *    plugin,
        djvGuiContext * context,
        QWidget *       parent  = 0);

    //! Destructor.

    virtual ~djvImageIoWidget() = 0;

    //! Get the plugin.
    
    djvImageIo * plugin() const;

    //! Get the context.
    
    djvGuiContext * context() const;
    
private:

    djvImageIo *    _plugin;
    djvGuiContext * _context;
};

//------------------------------------------------------------------------------
//! \class djvImageIoWidgetPlugin
//!
//! This class provides an image I/O widget plugin.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvImageIoWidgetPlugin : public djvPlugin
{
public:

    //! Constructor.

    djvImageIoWidgetPlugin(djvCoreContext *);

    //! Destructor.

    virtual ~djvImageIoWidgetPlugin() = 0;

    //! Create a widget.
    
    virtual djvImageIoWidget * createWidget(djvImageIo * plugin) const = 0;
    
    //! Get the context.
    
    djvGuiContext * guiContext() const;

    virtual djvPlugin * copyPlugin() const;
};

//------------------------------------------------------------------------------
//! \class djvImageIoWidgetFactory
//!
//! This class provides a factory for image I/O widget plugins.
//------------------------------------------------------------------------------

class DJV_GUI_EXPORT djvImageIoWidgetFactory : public djvPluginFactory
{
    Q_OBJECT
    
public:

    //! Constructor.

    explicit djvImageIoWidgetFactory(
        djvGuiContext *     context,
        const QStringList & searchPath = djvSystem::searchPath(),
        QObject *           parent     = 0);

    //! Destructor.

    virtual ~djvImageIoWidgetFactory();

    //! Create a widget.
    
    djvImageIoWidget * createWidget(djvImageIo *) const;
    
private:

    DJV_PRIVATE_COPY(djvImageIoWidgetFactory);
    
    djvGuiContext * _context;
};

//@} // djvGuiWidget

#endif // DJV_IMAGE_IO_WIDGET_H


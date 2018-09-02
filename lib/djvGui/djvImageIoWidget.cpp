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

#include <djvImageIoWidget.h>

#include <djvGuiContext.h>

#include <djvImageIo.h>

//------------------------------------------------------------------------------
// djvImageIoWidget
//------------------------------------------------------------------------------

djvImageIoWidget::djvImageIoWidget(
    djvImageIo *    plugin,
    djvGuiContext * context,
    QWidget *       parent) :
    djvAbstractPrefsWidget(plugin->pluginName(), context, parent),
    _plugin (plugin),
    _context(context)
{}

djvImageIoWidget::~djvImageIoWidget()
{}

djvImageIo * djvImageIoWidget::plugin() const
{
    return _plugin;
}

djvGuiContext * djvImageIoWidget::context() const
{
    return _context;
}

//------------------------------------------------------------------------------
// djvImageIoWidgetPlugin
//------------------------------------------------------------------------------

djvImageIoWidgetPlugin::djvImageIoWidgetPlugin(djvCoreContext * context) :
    djvPlugin(context)
{}

djvImageIoWidgetPlugin::~djvImageIoWidgetPlugin()
{}

djvGuiContext * djvImageIoWidgetPlugin::guiContext() const
{
    return dynamic_cast<djvGuiContext *>(context());
}

djvPlugin * djvImageIoWidgetPlugin::copyPlugin() const
{
    return 0;
}

//------------------------------------------------------------------------------
// djvImageIoWidgetFactory
//------------------------------------------------------------------------------

djvImageIoWidgetFactory::djvImageIoWidgetFactory(
    djvGuiContext *     context,
    const QStringList & searchPath,
    QObject *           parent) :
    djvPluginFactory(context, searchPath, "djvImageIoWidgetEntry", "djv", "Plugin", parent),
    _context(context)
{
    //DJV_DEBUG("djvImageIoWidgetFactory::djvImageIoWidgetFactory");
}

djvImageIoWidgetFactory::~djvImageIoWidgetFactory()
{
    //DJV_DEBUG("djvImageIoWidgetFactory::~djvImageIoWidgetFactory");
}

djvImageIoWidget * djvImageIoWidgetFactory::createWidget(djvImageIo * imageIoPlugin) const
{
    Q_FOREACH(djvPlugin * plugin, plugins())
    {
        if (djvImageIoWidgetPlugin * imageIoWidgetPlugin =
            dynamic_cast<djvImageIoWidgetPlugin *>(plugin))
        {
            if (imageIoWidgetPlugin->pluginName() == imageIoPlugin->pluginName())
            {
                return imageIoWidgetPlugin->createWidget(imageIoPlugin);
            }
        }
    }    
    return 0;
}

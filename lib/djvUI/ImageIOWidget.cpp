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

#include <djvUI/ImageIOWidget.h>

#include <djvUI/UIContext.h>

#include <djvGraphics/ImageIO.h>

namespace djv
{
    namespace UI
    {
        ImageIOWidget::ImageIOWidget(
            Graphics::ImageIO * plugin,
            UIContext * context,
            QWidget * parent) :
            AbstractPrefsWidget(plugin->pluginName(), context, parent),
            _plugin(plugin),
            _context(context)
        {}

        ImageIOWidget::~ImageIOWidget()
        {}

        Graphics::ImageIO * ImageIOWidget::plugin() const
        {
            return _plugin;
        }

        UIContext * ImageIOWidget::context() const
        {
            return _context;
        }

        ImageIOWidgetPlugin::ImageIOWidgetPlugin(djvCoreContext * context) :
            djvPlugin(context)
        {}

        ImageIOWidgetPlugin::~ImageIOWidgetPlugin()
        {}

        UIContext * ImageIOWidgetPlugin::uiContext() const
        {
            return dynamic_cast<UIContext *>(context());
        }

        djvPlugin * ImageIOWidgetPlugin::copyPlugin() const
        {
            return 0;
        }

        ImageIOWidgetFactory::ImageIOWidgetFactory(
            UIContext * context,
            const QStringList & searchPath,
            QObject * parent) :
            djvPluginFactory(context, searchPath, "djv::UI::ImageIOWidgetEntry", "djv", "Plugin", parent),
            _context(context)
        {
            //DJV_DEBUG("ImageIOWidgetFactory::ImageIOWidgetFactory");
        }

        ImageIOWidgetFactory::~ImageIOWidgetFactory()
        {
            //DJV_DEBUG("ImageIOWidgetFactory::~ImageIOWidgetFactory");
        }

        ImageIOWidget * ImageIOWidgetFactory::createWidget(Graphics::ImageIO * imageIOPlugin) const
        {
            Q_FOREACH(djvPlugin * plugin, plugins())
            {
                if (ImageIOWidgetPlugin * imageIOWidgetPlugin = dynamic_cast<ImageIOWidgetPlugin *>(plugin))
                {
                    if (imageIOWidgetPlugin->pluginName() == imageIOPlugin->pluginName())
                    {
                        return imageIOWidgetPlugin->createWidget(imageIOPlugin);
                    }
                }
            }
            return 0;
        }

    } // namespace UI
} // namespace djv

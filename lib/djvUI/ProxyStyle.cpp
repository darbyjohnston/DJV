//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
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

#include <djvUI/ProxyStyle.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/UIContext.h>

#include <QPainter>
#include <QPointer>
#include <QStyleFactory>
#include <QStyleOption>

#include <iostream>

namespace djv
{
    namespace UI
    {
        struct ProxyStyle::Private
        {
            QPointer<UIContext> context;
            int fontSize = 12;
        };

        ProxyStyle::ProxyStyle(const QPointer<UIContext> & context) :
            QProxyStyle(QStyleFactory::create("fusion")),
            _p(new Private)
        {
            _p->context = context;
        }

        ProxyStyle::~ProxyStyle()
        {}
        
        int ProxyStyle::pixelMetric(
            PixelMetric metric,
            const QStyleOption * option,
            const QWidget * widget) const
        {
            //! \todo Add scaling for different DPI values.
            float scale = _p->fontSize / 12.f;
            switch (metric)
            {
            case PM_SmallIconSize: return static_cast<int>(20 * scale);
            case PM_ToolBarIconSize: return static_cast<int>(25 * scale);
            default: break;
            }
            return static_cast<int>(QProxyStyle::pixelMetric(metric, option, widget) * scale);
        }

        QPixmap ProxyStyle::standardPixmap(
            StandardPixmap standardPixmap,
            const QStyleOption * option,
            const QWidget * widget) const
        {
            switch (standardPixmap)
            {
            case SP_TitleBarNormalButton:
            case SP_TitleBarMinButton:
                //! \bug Hard-coded pixmap size.
                return _p->context->iconLibrary()->pixmap("djv/UI/DockFloatIcon96DPI.png");
            case SP_TitleBarCloseButton:
            case SP_DockWidgetCloseButton:
                //! \bug Hard-coded pixmap size.
                return _p->context->iconLibrary()->pixmap("djv/UI/DockCloseIcon96DPI.png");
            default: break;
            }
            return QProxyStyle::standardPixmap(standardPixmap, option, widget);
        }
        
        int ProxyStyle::styleHint(
            StyleHint hint,
            const QStyleOption * option,
            const QWidget * widget,
            QStyleHintReturn * out) const
        {
            switch (hint)
            {
            case SH_EtchDisabledText:
                return 0;
            default: break;
            }
            return QProxyStyle::styleHint(hint, option, widget, out);
        }

        void ProxyStyle::drawControl(
            ControlElement element,
            const QStyleOption * option,
            QPainter * painter,
            const QWidget * widget) const   
        {
            switch (element)
            {
                case CE_ToolBar:
                    return;
                default: break;
            }
            QProxyStyle::drawControl(element, option, painter, widget);
        }
        
        void ProxyStyle::drawPrimitive(
            PrimitiveElement element,
            const QStyleOption * option,
            QPainter * painter,
            const QWidget * widget) const
        {
            switch (element)
            {
                case QStyle::PE_FrameDockWidget:
                    return;
                default: break;
            }
            QProxyStyle::drawPrimitive(element, option, painter, widget);
        }
        
        void ProxyStyle::setFontSize(int value)
        {
            if (value == _p->fontSize)
                return;
            _p->fontSize = value;
        }

    } // namespace UI
} // namespace djv

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

#include <djvUI/ToolButton.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/UIContext.h>

#include <djvAV/Color.h>
#include <djvAV/ColorUtil.h>

#include <djvCore/Math.h>
#include <djvCore/SignalBlocker.h>

#include <QAction>
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QPointer>
#include <QStyle>
#include <QStyleOption>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ToolButton::Private
        {
            QPointer<QAction> defaultAction;
        };

        ToolButton::ToolButton(const QPointer<UIContext> & context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {
            styleUpdate();
            widgetUpdate();
        }

        ToolButton::ToolButton(const QIcon & icon, const QPointer<UIContext> & context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {
            setIcon(icon);
            styleUpdate();
            widgetUpdate();
        }

        void ToolButton::setDefaultAction(QAction * action)
        {
            if (action == _p->defaultAction)
                return;
            if (_p->defaultAction)
            {
                disconnect(
                    _p->defaultAction,
                    SIGNAL(toggled(bool)),
                    this,
                    SLOT(setChecked(bool)));
                disconnect(
                    _p->defaultAction,
                    SIGNAL(changed()),
                    this,
                    SLOT(widgetUpdate()));
                disconnect(
                    this,
                    SIGNAL(toggled(bool)),
                    _p->defaultAction,
                    SLOT(setChecked(bool)));
            }
            _p->defaultAction = action;
            widgetUpdate();
            if (_p->defaultAction)
            {
                connect(
                    _p->defaultAction,
                    SIGNAL(toggled(bool)),
                    SLOT(setChecked(bool)));
                connect(
                    _p->defaultAction,
                    SIGNAL(changed()),
                    SLOT(widgetUpdate()));
                connect(
                    this,
                    SIGNAL(toggled(bool)),
                    _p->defaultAction,
                    SLOT(setChecked(bool)));
            }
        }

        ToolButton::~ToolButton()
        {}
        
        QSize ToolButton::sizeHint() const
        {
            const int iconSize = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
            const int margin = style()->pixelMetric(QStyle::PM_ButtonMargin);
            const QString & text = this->text();
            const int textWidth = fontMetrics().width(text);
            const int textHeight = fontMetrics().height();
            QSize sizeHint;
            if (!icon().isNull())
            {
                sizeHint.setWidth(iconSize);
            }
            sizeHint.setWidth(sizeHint.width() + textWidth);
            sizeHint.setHeight(Math::max(iconSize, textHeight));
            sizeHint += QSize(margin * 2, margin * 2);
            return sizeHint.expandedTo(QApplication::globalStrut());
        }

        void ToolButton::nextCheckState()
        {
            if (!_p->defaultAction)
            {
                AbstractToolButton::nextCheckState();
            }
            else
            {
                _p->defaultAction->trigger();
            }
        }

        void ToolButton::paintEvent(QPaintEvent * event)
        {
            AbstractToolButton::paintEvent(event);

            QPainter painter(this);

            QIcon::Mode  mode  = QIcon::Normal;
            QIcon::State state = QIcon::Off;
            if (!isEnabled())
                mode = QIcon::Disabled;
            if (isChecked())
                state = QIcon::On;
            const int iconSize = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
            const int margin = style()->pixelMetric(QStyle::PM_ButtonMargin);
            const QPixmap & pixmap = icon().pixmap(iconSize, iconSize, mode, state);
            if (!pixmap.isNull())
            {
                painter.drawPixmap(margin + iconSize / 2 - pixmap.width() / 2, margin + iconSize / 2 - pixmap.height() / 2, pixmap);
            }

            const QString & text = this->text();
            if (!text.isEmpty())
            {
                painter.setPen(palette().color(QPalette::Text));
                painter.drawText(
                    QRect(pixmap.width(), 0, width() - pixmap.width(), height()),
                    Qt::AlignCenter,
                    text);
            }
        }

        bool ToolButton::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractToolButton::event(event);
        }

        void ToolButton::styleUpdate()
        {
            updateGeometry();
        }
        
        void ToolButton::widgetUpdate()
        {
            if (_p->defaultAction)
            {
                setIcon(_p->defaultAction->icon());
                setToolTip(_p->defaultAction->toolTip());
                setWhatsThis(_p->defaultAction->whatsThis());
                setCheckable(_p->defaultAction->isCheckable());
                setChecked(_p->defaultAction->isChecked());
                setAutoRepeat(_p->defaultAction->autoRepeat());
                setEnabled(_p->defaultAction->isEnabled());
            }
        }

    } // namespace UI
} // namespace djv

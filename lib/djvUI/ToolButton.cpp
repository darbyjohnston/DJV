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

#include <djvUI/ToolButton.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Style.h>
#include <djvUI/UIContext.h>

#include <djvGraphics/Color.h>
#include <djvGraphics/ColorUtil.h>

#include <djvCore/SignalBlocker.h>

#include <QAction>
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>

namespace djv
{
    namespace UI
    {
        struct ToolButton::Private
        {
            QAction * defaultAction = nullptr;
        };

        ToolButton::ToolButton(UIContext * context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {
            sizeUpdate();
            widgetUpdate();
            connect(
                context->style(),
                SIGNAL(sizeMetricsChanged()),
                SLOT(sizeUpdate()));
        }

        ToolButton::ToolButton(const QIcon & icon, UIContext * context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {
            setIcon(icon);
            sizeUpdate();
            widgetUpdate();
            connect(
                context->style(),
                SIGNAL(sizeMetricsChanged()),
                SLOT(sizeUpdate()));
        }

        ToolButton::~ToolButton()
        {}

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

        QSize ToolButton::sizeHint() const
        {
            const int iconSize = context()->style()->sizeMetric().iconSize;
            QSize sizeHint(iconSize, iconSize);
            const int margin = context()->style()->sizeMetric().margin;
            QStyleOptionToolButton opt;
            opt.iconSize = icon().actualSize(sizeHint);
            opt.iconSize += QSize(margin * 2, margin * 2);
            sizeHint = opt.iconSize.expandedTo(QApplication::globalStrut());
            return sizeHint;
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

            QIcon::Mode  mode = QIcon::Normal;
            QIcon::State state = QIcon::Off;
            if (!isEnabled())
                mode = QIcon::Disabled;
            if (isChecked())
                state = QIcon::On;
            const QPixmap & pixmap = icon().pixmap(width(), height(), mode, state);
            if (!pixmap.isNull())
            {
                QImage image(pixmap.width(), pixmap.height(), QImage::Format_ARGB32_Premultiplied);
                image.fill(Qt::transparent);
                QPainter imagePainter(&image);
                imagePainter.drawPixmap(0, 0, pixmap);
                imagePainter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceIn);
                imagePainter.fillRect(0, 0, pixmap.width(), pixmap.height(), QPalette().foreground());

                QPainter painter(this);
                painter.drawImage(
                    width() / 2 - image.width() / 2,
                    height() / 2 - image.height() / 2,
                    image);
            }
        }

        void ToolButton::sizeUpdate()
        {
            updateGeometry();
        }
        
        void ToolButton::widgetUpdate()
        {
            //djvSignalBlocker signalBlocker(this);
            if (_p->defaultAction)
            {
                setText(_p->defaultAction->text());
                setIcon(_p->defaultAction->icon());
                setToolTip(_p->defaultAction->toolTip());
                setCheckable(_p->defaultAction->isCheckable());
                setChecked(_p->defaultAction->isChecked());
                setAutoRepeat(_p->defaultAction->autoRepeat());
            }
        }

    } // namespace UI
} // namespace djv

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

#include <djvUI/ShuttleButton.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/Style.h>
#include <djvUI/UIContext.h>

#include <djvCore/Math.h>
#include <djvCore/Vector.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

namespace djv
{
    namespace UI
    {
        struct ShuttleButton::Private
        {
            float          value = 0.f;
            QVector<QIcon> icons;
            bool           mousePress = false;
            glm::ivec2     mouseStartPos = glm::ivec2(0, 0);
        };

        ShuttleButton::ShuttleButton(UIContext * context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {}

        ShuttleButton::ShuttleButton(const QVector<QIcon> & icons, UIContext * context, QWidget * parent) :
            AbstractToolButton(context, parent),
            _p(new Private)
        {
            setIcons(icons);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        }

        ShuttleButton::~ShuttleButton()
        {}

        const QVector<QIcon> & ShuttleButton::icons() const
        {
            return _p->icons;
        }

        void ShuttleButton::setIcons(const QVector<QIcon> & icons)
        {
            _p->icons = icons;

            updateGeometry();
            update();
        }

        const QVector<QIcon> & ShuttleButton::iconsDefault(UIContext * context)
        {
            static const QVector<QIcon> data = QVector<QIcon>() <<
                context->iconLibrary()->icon("djvShuttle0Icon.png") <<
                context->iconLibrary()->icon("djvShuttle1Icon.png") <<
                context->iconLibrary()->icon("djvShuttle2Icon.png") <<
                context->iconLibrary()->icon("djvShuttle3Icon.png") <<
                context->iconLibrary()->icon("djvShuttle4Icon.png") <<
                context->iconLibrary()->icon("djvShuttle5Icon.png") <<
                context->iconLibrary()->icon("djvShuttle6Icon.png") <<
                context->iconLibrary()->icon("djvShuttle7Icon.png");
            return data;
        }

        QSize ShuttleButton::sizeHint() const
        {
            const int iconSize = context()->style()->sizeMetric().iconSize;
            QSize sizeHint(iconSize, iconSize);
            const int margin = context()->style()->sizeMetric().margin;
            QStyleOptionToolButton opt;
            opt.iconSize = _p->icons.count() > 0 ?
                _p->icons[0].actualSize(sizeHint) :
                sizeHint;
            opt.iconSize += QSize(margin * 2, margin * 2);
            sizeHint = opt.iconSize.expandedTo(QApplication::globalStrut());
            return sizeHint;
        }

        void ShuttleButton::mousePressEvent(QMouseEvent * event)
        {
            _p->value = 0.f;
            _p->mousePress = true;
            _p->mouseStartPos = glm::ivec2(event->pos().x(), event->pos().y());
            setCursor(Qt::SizeHorCursor);
            update();
            Q_EMIT mousePressed(_p->mousePress);
        }

        void ShuttleButton::mouseReleaseEvent(QMouseEvent * event)
        {
            _p->value = 0.f;
            _p->mousePress = false;
            setCursor(QCursor());
            update();
            Q_EMIT mousePressed(_p->mousePress);
        }

        void ShuttleButton::mouseMoveEvent(QMouseEvent * event)
        {
            const int tmp = event->pos().x() - _p->mouseStartPos.x;
            if (tmp != _p->value)
            {
                _p->value = static_cast<float>(tmp);
                update();
                Q_EMIT valueChanged(Core::Math::round(_p->value / 5.f));
            }
        }

        void ShuttleButton::paintEvent(QPaintEvent * event)
        {
            AbstractToolButton::paintEvent(event);

            const int i = Core::Math::mod(Core::Math::round(_p->value / 5.f), 8);
            QIcon::Mode  mode = QIcon::Normal;
            QIcon::State state = QIcon::Off;
            if (!isEnabled())
                mode = QIcon::Disabled;
            const QPixmap & pixmap = _p->icons[i].pixmap(width(), height(), mode, state);

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

    } // namespace UI
} // namespace djv

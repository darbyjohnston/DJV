//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvUI/ColorSwatch.h>

#include <djvUI/ColorDialog.h>
#include <djvUI/StylePrefs.h>
#include <djvUI/UIContext.h>

#include <djvGraphics/ColorUtil.h>

#include <QEvent>
#include <QPainter>
#include <QStyle>

namespace djv
{
    namespace UI
    {
        struct ColorSwatch::Private
        {
            Graphics::Color color;
            SWATCH_SIZE swatchSize = SWATCH_MEDIUM;
            bool colorDialogEnabled = false;
            QPointer<UIContext> context;
        };

        ColorSwatch::ColorSwatch(const QPointer<UIContext> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            setAttribute(Qt::WA_OpaquePaintEvent);
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            styleUpdate();
        }

        ColorSwatch::~ColorSwatch()
        {}

        const Graphics::Color & ColorSwatch::color() const
        {
            return _p->color;
        }

        ColorSwatch::SWATCH_SIZE ColorSwatch::swatchSize() const
        {
            return _p->swatchSize;
        }

        void ColorSwatch::setSwatchSize(SWATCH_SIZE swatchSize)
        {
            if (swatchSize == _p->swatchSize)
                return;
            _p->swatchSize = swatchSize;
            updateGeometry();
        }

        bool ColorSwatch::isColorDialogEnabled() const
        {
            return _p->colorDialogEnabled;
        }

        void ColorSwatch::setColorDialogEnabled(bool value)
        {
            _p->colorDialogEnabled = value;
        }

        QSize ColorSwatch::sizeHint() const
        {
            int size = style()->pixelMetric(QStyle::PM_LargeIconSize) * 2;
            switch (_p->swatchSize)
            {
            case SWATCH_SMALL: size /= 2; break;
            case SWATCH_LARGE: size *= 2; break;
            default: break;
            }
            size = (size / 10) * 10;
            return QSize(size, size);
        }

        void ColorSwatch::setColor(const Graphics::Color & color)
        {
            if (color == _p->color)
                return;
            _p->color = color;
            update();
            Q_EMIT colorChanged(_p->color);
        }

        void ColorSwatch::mousePressEvent(QMouseEvent *)
        {
            if (_p->colorDialogEnabled)
            {
                ColorDialog dialog(color(), _p->context);
                connect(
                    &dialog,
                    SIGNAL(colorChanged(const djv::Graphics::Color &)),
                    SLOT(setColor(const djv::Graphics::Color &)));
                dialog.exec();
            }
            Q_EMIT clicked();
        }

        void ColorSwatch::paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            QRect rect(this->rect());
            if (_p->context->stylePrefs()->hasColorSwatchTransparency())
            {
                Graphics::Color tmp(Graphics::Pixel::RGBA_U8);
                Graphics::ColorUtil::convert(_p->color, tmp);
                int vCount = 0;
                for (int y = rect.top(); y < rect.bottom(); y = y + 10, ++vCount)
                {
                    int hCount = vCount % 2;
                    for (int x = rect.left(); x < rect.right(); x = x + 10, ++hCount)
                    {
                        painter.fillRect(
                            QRect(x, y, 10, 10),
                            hCount % 2 == 0 ? Qt::white : Qt::black);
                    }
                }
                rect = rect.adjusted(0, 0, -1, -1);
                painter.setPen(palette().color(QPalette::Shadow));
                painter.drawRect(rect);
                rect = rect.adjusted(1, 1, 0, 0);
                painter.fillRect(rect, QColor(tmp.u8(0), tmp.u8(1), tmp.u8(2), tmp.u8(3)));
            }
            else
            {
                Graphics::Color tmp(Graphics::Pixel::RGB_U8);
                Graphics::ColorUtil::convert(_p->color, tmp);
                rect = rect.adjusted(0, 0, -1, -1);
                painter.setPen(palette().color(QPalette::Shadow));
                painter.drawRect(rect);
                rect = rect.adjusted(1, 1, 0, 0);
                painter.fillRect(rect, QColor(tmp.u8(0), tmp.u8(1), tmp.u8(2)));
            }
        }

        bool ColorSwatch::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void ColorSwatch::styleUpdate()
        {
            updateGeometry();
            update();
        }

    } // namespace UI
} // namespace djv

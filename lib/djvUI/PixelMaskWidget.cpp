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

#include <djvUI/PixelMaskWidget.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/ToolButton.h>
#include <djvUI/UIContext.h>

#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QMenu>

namespace djv
{
    namespace UI
    {
        struct PixelMaskWidget::Private
        {
            UIContext * context = nullptr;
            Graphics::Pixel::Mask mask;
            ToolButton * button = nullptr;
        };

        PixelMaskWidget::PixelMaskWidget(UIContext * context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->context = context;
            
            _p->button = new ToolButton(context);
            _p->button->setCheckable(true);
            _p->button->setToolTip(
                qApp->translate("djv::UI::PixelMaskWidget", "Set the pixel mask"));

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->button);

            styleUpdate();
            valueUpdate();

            connect(
                _p->button,
                SIGNAL(pressed()),
                SLOT(buttonCallback()));
        }

        const Graphics::Pixel::Mask & PixelMaskWidget::mask() const
        {
            return _p->mask;
        }

        void PixelMaskWidget::setMask(const Graphics::Pixel::Mask & mask)
        {
            if (mask == _p->mask)
                return;
            _p->mask = mask;
            valueUpdate();
            Q_EMIT maskChanged(_p->mask);
        }

        bool PixelMaskWidget::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }

        void PixelMaskWidget::buttonCallback()
        {
            QMenu menu;
            static const QString text[] =
            {
                qApp->translate("djv::UI::PixelMaskWidget", "Solo red"),
                qApp->translate("djv::UI::PixelMaskWidget", "Solo green"),
                qApp->translate("djv::UI::PixelMaskWidget", "Solo blue"),
                qApp->translate("djv::UI::PixelMaskWidget", "Solo alpha")
            };
            int count = sizeof(text) / sizeof(text[0]);
            int data = 0;
            for (int i = 0; i < count; ++i)
            {
                QAction * action = menu.addAction(text[i]);
                action->setData(data++);
                connect(action, SIGNAL(triggered()), SLOT(soloCallback()));
            }

            menu.addSeparator();

            static const QString text2[] =
            {
                qApp->translate("djv::UI::PixelMaskWidget", "Red"),
                qApp->translate("djv::UI::PixelMaskWidget", "Green"),
                qApp->translate("djv::UI::PixelMaskWidget", "Blue"),
                qApp->translate("djv::UI::PixelMaskWidget", "Alpha")
            };
            count = sizeof(text) / sizeof(text[0]);
            data = 0;
            for (int i = 0; i < count; ++i)
            {
                QAction * action = menu.addAction(text2[i]);
                action->setCheckable(true);
                action->setChecked(_p->mask[i]);
                action->setData(data++);
                connect(action, SIGNAL(toggled(bool)), SLOT(toggleCallback(bool)));
            }

            menu.addSeparator();

            QAction * action = menu.addAction(
                qApp->translate("djv::UI::PixelMaskWidget", "Reset"));
            connect(action, SIGNAL(triggered()), SLOT(resetCallback()));

            menu.exec(mapToGlobal(
                QPoint(_p->button->x(), _p->button->y() + _p->button->height())));

            _p->button->setDown(false);
        }

        void PixelMaskWidget::soloCallback()
        {
            QAction * action = qobject_cast<QAction *>(sender());
            Graphics::Pixel::Mask mask(false);
            mask[action->data().toInt()] = true;
            setMask(mask);
        }

        void PixelMaskWidget::toggleCallback(bool checked)
        {
            QAction * action = qobject_cast<QAction *>(sender());
            Graphics::Pixel::Mask mask = _p->mask;
            mask[action->data().toInt()] = checked;
            setMask(mask);
        }

        void PixelMaskWidget::resetCallback()
        {
            setMask(Graphics::Pixel::Mask());
        }

        void PixelMaskWidget::styleUpdate()
        {
            _p->button->setIcon(_p->context->iconLibrary()->icon("djv/UI/SubMenuIcon"));
        }

        void PixelMaskWidget::valueUpdate()
        {
            Core::SignalBlocker signalBlocker(_p->button);
            bool checked = false;
            for (int i = 0; i < Graphics::Pixel::channelsMax; ++i)
            {
                if (!_p->mask[i])
                {
                    checked = true;
                    break;
                }
            }
            _p->button->setChecked(checked);
        }

    } // namespace UI
} // namespace djv

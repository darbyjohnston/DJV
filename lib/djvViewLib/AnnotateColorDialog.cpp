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

#include <djvViewLib/AnnotateColorDialog.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/ColorWidget.h>
#include <djvUI/ToolButton.h>

#include <djvAV/Color.h>

#include <QApplication>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPointer>
#include <QStyle>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        namespace
        {
            const std::vector<AV::Color> colors =
            {
                AV::Color(1.f, 0.f, 0.f),
                AV::Color(1.f, 1.f, 0.f),
                AV::Color(0.f, 1.f, 0.f),
                AV::Color(0.f, 1.f, 1.f),
                AV::Color(0.f, 0.f, 1.f),
                AV::Color(1.f, 0.f, 1.f)
            };

            const int columns = 6;
        
        } // namespace

        struct AnnotateColorDialog::Private
        {
            AV::Color color;
            QPointer<UI::ColorSwatch> colorSwatch;
            QPointer<UI::ColorWidget> colorWidget;
            std::vector<QPointer<UI::ToolButton> > colorButtons;
            QPointer<QButtonGroup> colorButtonGroup;
            QPointer<QDialogButtonBox> buttonBox;
            QPointer<QGridLayout> buttonLayout;
        };

        AnnotateColorDialog::AnnotateColorDialog(const QPointer<ViewContext> & context) :
            _p(new Private)
        {
            _p->colorSwatch = new UI::ColorSwatch(context.data());
            _p->colorSwatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            _p->colorWidget = new UI::ColorWidget(context.data());

            _p->colorButtonGroup = new QButtonGroup(this);
            int j = 0;
            for (const auto & i : colors)
            {
                auto button = new UI::ToolButton(context.data());
                _p->colorButtons.push_back(button);
                _p->colorButtonGroup->addButton(button, j);
                ++j;
            }

            _p->buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

            auto layout = new QVBoxLayout(this);
            auto hLayout = new QHBoxLayout;
            hLayout->addWidget(_p->colorSwatch);
            hLayout->addWidget(_p->colorWidget, 1000);
            layout->addLayout(hLayout, 1000);
            hLayout = new QHBoxLayout;
            _p->buttonLayout = new QGridLayout;
            int x = 0;
            int y = 0;
            for (const auto & i : _p->colorButtons)
            {
                _p->buttonLayout->addWidget(i, y, x);
                ++x;
                if (x >= columns)
                {
                    x = 0;
                    ++y;
                }
            }
            hLayout->addLayout(_p->buttonLayout);
            hLayout->addStretch();
            layout->addLayout(hLayout);
            layout->addWidget(_p->buttonBox);

            setWindowTitle(qApp->translate("djv::ViewLib::AnnotateColorDialog", "Color"));
            setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
            styleUpdate();
            widgetUpdate();

            connect(
                _p->colorWidget,
                &UI::ColorWidget::colorChanged,
                [this](const djv::AV::Color & value)
            {
                setColor(value);
                Q_EMIT colorChanged(value);
            });

            connect(
                _p->colorButtonGroup,
                SIGNAL(buttonClicked(int)),
                SLOT(buttonCallback(int)));

            connect(_p->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        }

        void AnnotateColorDialog::setColor(const AV::Color & value)
        {
            if (value == _p->color)
                return;
            _p->color = value;
            widgetUpdate();
        }

        AnnotateColorDialog::~AnnotateColorDialog()
        {}

        bool AnnotateColorDialog::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QDialog::event(event);
        }

        void AnnotateColorDialog::buttonCallback(int value)
        {
            Q_EMIT colorChanged(colors[value]);
            accept();
        }

        void AnnotateColorDialog::styleUpdate()
        {
            _p->buttonLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void AnnotateColorDialog::widgetUpdate()
        {
            _p->colorSwatch->setColor(_p->color);
            _p->colorWidget->setColor(_p->color);
            size_t j = 0;
            for (const auto & i : _p->colorButtons)
            {
                const int size = style()->pixelMetric(QStyle::PM_ButtonIconSize);
                QImage image(size, size, QImage::Format_ARGB32);
                image.fill(AV::ColorUtil::toQt(colors[j]));
                i->setIcon(QPixmap::fromImage(image));
                ++j;
            }
        }

    } // namespace ViewLib
} // namespace djv


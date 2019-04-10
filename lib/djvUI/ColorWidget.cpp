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

#include <djvUI/ColorWidget.h>

#include <djvUI/FloatEditSlider.h>
#include <djvUI/FloatObject.h>
#include <djvUI/IntEditSlider.h>

#include <djvAV/ColorUtil.h>
#include <djvCore/ListUtil.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPointer>
#include <QStyle>
#include <QVBoxLayout>

namespace djv
{
    namespace UI
    {
        struct ColorWidget::Private
        {
            AV::Color color = AV::Color(AV::Pixel::RGB_U8);
            bool editPixel = true;
            QVector<QPointer<IntEditSlider> > intWidgets;
            QVector<QPointer<FloatEditSlider> > floatWidgets;
            QPointer<QComboBox> formatWidget;
            QPointer<QComboBox> typeWidget;
            QPointer<QHBoxLayout> bottomLayout;
        };

        ColorWidget::ColorWidget(const QPointer<UIContext> & context, QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            //DJV_DEBUG("ColorWidget::ColorWidget");

            // Create the widgets.
            for (int i = 0; i < AV::Pixel::channelsMax; ++i)
            {
                auto intSlider = new IntEditSlider(context);
                intSlider->setResetToDefault(false);
                _p->intWidgets += intSlider;

                auto floatSlider = new FloatEditSlider(context);
                floatSlider->setResetToDefault(false);
                floatSlider->editObject()->setClamp(false);
                _p->floatWidgets += floatSlider;
            }

            _p->formatWidget = new QComboBox;
            _p->formatWidget->addItems(AV::Pixel::formatLabels());
            _p->formatWidget->setToolTip(qApp->translate("djv::UI::ColorWidget", "Pixel format"));

            _p->typeWidget = new QComboBox;
            _p->typeWidget->addItems(AV::Pixel::typeLabels());
            _p->typeWidget->setToolTip(qApp->translate("djv::UI::ColorWidget", "Pixel type"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->setMargin(0);
            for (int i = 0; i < AV::Pixel::channelsMax; ++i)
            {
                layout->addWidget(_p->intWidgets[i]);
                layout->addWidget(_p->floatWidgets[i]);
            }
            _p->bottomLayout = new QHBoxLayout;
            _p->bottomLayout->setMargin(0);
            _p->bottomLayout->addStretch();
            _p->bottomLayout->addWidget(_p->formatWidget);
            _p->bottomLayout->addWidget(_p->typeWidget);
            layout->addLayout(_p->bottomLayout);

            // Initialize.
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            styleUpdate();
            widgetUpdate();
            valueUpdate();

            // Setup the callbacks.
            for (int i = 0; i < AV::Pixel::channelsMax; ++i)
            {
                connect(
                    _p->intWidgets[i],
                    SIGNAL(valueChanged(int)),
                    SLOT(intCallback(int)));
                connect(
                    _p->floatWidgets[i],
                    SIGNAL(valueChanged(float)),
                    SLOT(floatCallback(float)));
            }
            connect(
                _p->formatWidget,
                SIGNAL(activated(int)),
                SLOT(formatCallback(int)));
            connect(
                _p->typeWidget,
                SIGNAL(activated(int)),
                SLOT(typeCallback(int)));
        }

        ColorWidget::~ColorWidget()
        {}
        
        const AV::Color & ColorWidget::color() const
        {
            return _p->color;
        }

        float ColorWidget::floatMin() const
        {
            return 0.f;
        }

        float ColorWidget::floatMax() const
        {
            return 1.f;
        }

        void ColorWidget::setColor(const AV::Color & color)
        {
            if (color == _p->color)
                return;
            //DJV_DEBUG("ColorWidget::setColor");
            //DJV_DEBUG_PRINT("color = " << color);
            const AV::Color prev = _p->color;
            _p->color = color;
            if (_p->color.pixel() != prev.pixel())
            {
                widgetUpdate();
            }
            valueUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void ColorWidget::setFloatRange(float, float)
        {
            //! \todo Implement custom floating-point ranges.
        }

        void ColorWidget::setEditPixel(bool value)
        {
            if (value == _p->editPixel)
                return;
            _p->editPixel = value;
            widgetUpdate();
        }

        QHBoxLayout * ColorWidget::bottomLayout()
        {
            return _p->bottomLayout;
        }

        bool ColorWidget::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return QWidget::event(event);
        }
        
        void ColorWidget::intCallback(int)
        {
            for (int i = 0; i < AV::Pixel::channels(_p->color.pixel()); ++i)
            {
                switch (AV::Pixel::type(_p->color.pixel()))
                {
                case AV::Pixel::U8:
                    _p->color.setU8(_p->intWidgets[i]->value(), i);
                    break;
                case AV::Pixel::U10:
                    _p->color.setU10(_p->intWidgets[i]->value(), i);
                    break;
                case AV::Pixel::U16:
                    _p->color.setU16(_p->intWidgets[i]->value(), i);
                    break;
                case AV::Pixel::F16:
                    _p->color.setF16(
                        static_cast<AV::Pixel::F16_T>(_p->floatWidgets[i]->value()),
                        i);
                    break;
                case AV::Pixel::F32:
                    _p->color.setF32(
                        static_cast<AV::Pixel::F32_T>(_p->floatWidgets[i]->value()),
                        i);
                    break;
                default: break;
                }
            }
            valueUpdate();
            Q_EMIT colorChanged(_p->color);
        }

        void ColorWidget::floatCallback(float)
        {
            intCallback(0);
        }

        void ColorWidget::formatCallback(int in)
        {
            AV::Color tmp(AV::Pixel::pixel(
                static_cast<AV::Pixel::FORMAT>(in),
                AV::Pixel::type(_p->color.pixel())));
            AV::ColorUtil::convert(_p->color, tmp);
            setColor(tmp);
            widgetUpdate();
        }

        void ColorWidget::typeCallback(int in)
        {
            AV::Color tmp(AV::Pixel::pixel(
                AV::Pixel::format(_p->color.pixel()),
                static_cast<AV::Pixel::TYPE>(in)));
            AV::ColorUtil::convert(_p->color, tmp);
            setColor(tmp);
            widgetUpdate();
        }

        void ColorWidget::styleUpdate()
        {
            _p->bottomLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }
        
        void ColorWidget::widgetUpdate()
        {
            //DJV_DEBUG("ColorWidget::widgetUpdate");
            //DJV_DEBUG_PRINT("color = " << _p->color);

            QVector<QObject *> widgets;
            Q_FOREACH(auto i, _p->intWidgets)
                widgets.append(i);
            Q_FOREACH(auto i, _p->floatWidgets)
                widgets.append(i);
            widgets.append(_p->formatWidget);
            widgets.append(_p->typeWidget);
            Core::SignalBlocker signalBlocker(widgets);

            const int channels = AV::Pixel::channels(_p->color.pixel());
            static const QString toolTip0[] =
            {
                qApp->translate("djv::UI::ColorWidget", "Luminance channel"),
                qApp->translate("djv::UI::ColorWidget", "Alpha channel")
            };
            static const QString toolTip1[] =
            {
                qApp->translate("djv::UI::ColorWidget", "Red channel"),
                qApp->translate("djv::UI::ColorWidget", "Green channel"),
                qApp->translate("djv::UI::ColorWidget", "Blue channel"),
                qApp->translate("djv::UI::ColorWidget", "Alpha channel")
            };

            const QString * toolTip = 0;
            switch (AV::Pixel::format(_p->color.pixel()))
            {
            case AV::Pixel::L:
            case AV::Pixel::LA:
                toolTip = toolTip0;
                break;
            case AV::Pixel::RGB:
            case AV::Pixel::RGBA:
                toolTip = toolTip1;
                break;
            default: break;
            }

            for (int i = 0; i < channels; ++i)
            {
                _p->intWidgets[i]->setRange(0, AV::Pixel::max(_p->color.pixel()));
                _p->intWidgets[i]->setToolTip(toolTip[i]);
                _p->floatWidgets[i]->setToolTip(toolTip[i]);
            }

            for (int i = 0; i < AV::Pixel::channelsMax; ++i)
            {
                switch (AV::Pixel::type(_p->color.pixel()))
                {
                case AV::Pixel::U8:
                case AV::Pixel::U10:
                case AV::Pixel::U16:
                    if (i < channels)
                    {
                        _p->intWidgets[i]->show();
                    }
                    else
                    {
                        _p->intWidgets[i]->hide();
                    }
                    _p->floatWidgets[i]->hide();
                    break;
                case AV::Pixel::F16:
                case AV::Pixel::F32:
                    _p->intWidgets[i]->hide();
                    if (i < channels)
                    {
                        _p->floatWidgets[i]->show();
                    }
                    else
                    {
                        _p->floatWidgets[i]->hide();
                    }
                    break;
                default: break;
                }
            }

            _p->formatWidget->setCurrentIndex(AV::Pixel::format(_p->color.pixel()));
            _p->formatWidget->setVisible(_p->editPixel);
            _p->typeWidget->setCurrentIndex(AV::Pixel::type(_p->color.pixel()));
            _p->typeWidget->setVisible(_p->editPixel);

            update();
        }

        void ColorWidget::valueUpdate()
        {
            //DJV_DEBUG("ColorWidget::valueUpdate");

            QVector<QObject *> widgets;
            Q_FOREACH(auto i, _p->intWidgets)
                widgets.append(i);
            Q_FOREACH(auto i, _p->floatWidgets)
                widgets.append(i);
            Core::SignalBlocker signalBlocker(widgets);

            for (int i = 0; i < AV::Pixel::channels(_p->color.pixel()); ++i)
            {
                switch (AV::Pixel::type(_p->color.pixel()))
                {
                case AV::Pixel::U8:
                    _p->intWidgets[i]->setValue(_p->color.u8(i));
                    break;
                case AV::Pixel::U10:
                    _p->intWidgets[i]->setValue(_p->color.u10(i));
                    break;
                case AV::Pixel::U16:
                    _p->intWidgets[i]->setValue(_p->color.u16(i));
                    break;
                case AV::Pixel::F16:
                    _p->floatWidgets[i]->setValue(_p->color.f16(i));
                    break;
                case AV::Pixel::F32:
                    _p->floatWidgets[i]->setValue(_p->color.f32(i));
                    break;
                default: break;
                }
            }
        }

    } // namespace UI
} // namespace djv

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

#include <djvViewLib/HistogramTool.h>

#include <djvViewLib/ImageView.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/PixelMaskWidget.h>
#include <djvUI/Prefs.h>
#include <djvUI/ToolButton.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPainter>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        class HistogramWidget : public QWidget
        {
        public:
            HistogramWidget();

            void set(
                const Graphics::PixelData * data,
                const Graphics::Color &     min,
                const Graphics::Color &     max);

            virtual QSize sizeHint() const;

        protected:
            virtual void resizeEvent(QResizeEvent *);
            virtual void paintEvent(QPaintEvent *);

        private:
            void updatePixmap();

            const Graphics::PixelData * _data;
            Graphics::Color             _min;
            Graphics::Color             _max;
            QPixmap                     _pixmap;
            bool                        _dirty;
        };

        HistogramWidget::HistogramWidget() :
            _data(0),
            _dirty(true)
        {
            setAttribute(Qt::WA_OpaquePaintEvent);
        }

        void HistogramWidget::set(
            const Graphics::PixelData * data,
            const Graphics::Color &     min,
            const Graphics::Color &     max)
        {
            _data = data;
            _min = min;
            _max = max;
            _dirty = true;
            update();
        }

        QSize HistogramWidget::sizeHint() const
        {
            return QSize(256, 256);
        }

        void HistogramWidget::resizeEvent(QResizeEvent *)
        {
            if (isVisible() && _data)
            {
                _dirty = true;
            }
        }

        void HistogramWidget::paintEvent(QPaintEvent *)
        {
            if (_dirty)
            {
                updatePixmap();
            }
            QPainter painter(this);
            painter.drawPixmap(0, 0, _pixmap);
        }

        namespace
        {
            void drawHistrogramBar(
                QPainter &     painter,
                int            width,
                int            height,
                int            dataWidth,
                int            index,
                int            y,
                const QColor & color)
            {
                const int x = Core::Math::floor(
                    index / static_cast<float>(dataWidth)* width);
                const int w = Core::Math::max(
                    Core::Math::floor(
                    (index + 1) / static_cast<float>(dataWidth)* width) - x,
                    1);
                painter.fillRect(x, height - 1 - y, w, y, color);
            }

        } // namespace

        void HistogramWidget::updatePixmap()
        {
            if (!_data)
                return;

            //DJV_DEBUG("HistogramWidget::updatePixmap");
            //DJV_DEBUG_PRINT("data = " << *_data);

            const int width = this->width();
            const int height = this->height();
            //DJV_DEBUG_PRINT("size = " << w << " " << h);
            _pixmap = QPixmap(width, height);

            QPainter painter(&_pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.fillRect(0, 0, width, height, palette().color(QPalette::Background));

            const Graphics::Pixel::U16_T * p = reinterpret_cast<const Graphics::Pixel::U16_T *>(_data->data());
            const int dataW = _data->w();
            const int dataC = _data->channels();
            //DJV_DEBUG_PRINT("dataW = " << dataW);
            //DJV_DEBUG_PRINT("dataC = " << dataC);

            int dataMax = 0;
            for (int i = 0; i < dataW; ++i, p += dataC)
            {
                for (int c = 0; c < dataC; ++c)
                {
                    dataMax = Core::Math::max(static_cast<int>(p[c]), dataMax);
                }
            }
            //DJV_DEBUG_PRINT("dataMax = " << dataMax);

            p = reinterpret_cast<const Graphics::Pixel::U16_T *>(_data->data());
            const QColor colors[] =
            {
                QColor(255,   0,   0),
                QColor(0, 255,   0),
                QColor(0,   0, 255)
            };
            for (int i = 0; i < dataW; ++i, p += dataC)
            {
                int yC[Graphics::Pixel::channelsMax] = { 0, 0, 0, 0 };
                int yMax = 0;
                for (int c = dataC - 1; c >= 0; --c)
                {
                    const int y = Core::Math::floor(
                        p[c] / static_cast<float>(dataMax) * (height - 1));
                    yC[c] = y;
                    yMax = Core::Math::max(y, yMax);
                }
                drawHistrogramBar(painter, width, height, dataW, i, yMax, Qt::black);
                painter.setCompositionMode(QPainter::CompositionMode_Plus);
                for (int c = dataC - 1; c >= 0; --c)
                {
                    drawHistrogramBar(painter, width, height, dataW, i, yC[c], colors[c]);
                }
            }
        }

        struct HistogramTool::Private
        {
            Util::HISTOGRAM size = static_cast<Util::HISTOGRAM>(0);
            bool colorProfile = true;
            bool displayProfile = true;
            Graphics::PixelData histogram;
            Graphics::Color min;
            Graphics::Color max;
            Graphics::Pixel::Mask mask;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;

            HistogramWidget * widget = nullptr;
            QLineEdit * minWidget = nullptr;
            QLineEdit * maxWidget = nullptr;
            QComboBox * sizeWidget = nullptr;
            UI::PixelMaskWidget * maskWidget = nullptr;
            UI::ToolButton * colorProfileButton = nullptr;
            UI::ToolButton * displayProfileButton = nullptr;
        };

        HistogramTool::HistogramTool(
            MainWindow * mainWindow,
            Context *    context,
            QWidget *    parent) :
            AbstractTool(mainWindow, context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->widget = new HistogramWidget;

            _p->minWidget = new QLineEdit;
            _p->minWidget->setReadOnly(true);

            _p->maxWidget = new QLineEdit;
            _p->maxWidget->setReadOnly(true);

            _p->sizeWidget = new QComboBox;
            _p->sizeWidget->addItems(Util::histogramLabels());
            _p->sizeWidget->setToolTip(
                qApp->translate("djv::ViewLib::HistogramTool", "The size of the histogram"));

            _p->maskWidget = new UI::PixelMaskWidget(context);

            _p->colorProfileButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvDisplayProfileIcon.png"));
            _p->colorProfileButton->setCheckable(true);
            _p->colorProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::HistogramTool", "Set whether the color profile is enabled"));

            _p->displayProfileButton = new UI::ToolButton(
                context->iconLibrary()->icon("djvDisplayProfileIcon.png"));
            _p->displayProfileButton->setCheckable(true);
            _p->displayProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::HistogramTool", "Set whether the display profile is enabled"));

            // Layout the widgets.
            QVBoxLayout * layout = new QVBoxLayout(this);

            layout->addWidget(_p->widget, 1);

            QFormLayout * formLayout = new QFormLayout;
            formLayout->addRow(
                qApp->translate("djv::ViewLib::HistogramTool", "Min:"),
                _p->minWidget);
            formLayout->addRow(
                qApp->translate("djv::ViewLib::HistogramTool", "Max:"),
                _p->maxWidget);
            layout->addLayout(formLayout);

            QHBoxLayout * hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->sizeWidget);
            hLayout->addStretch();
            hLayout->addWidget(_p->maskWidget);
            QHBoxLayout * hLayout2 = new QHBoxLayout;
            hLayout2->setMargin(0);
            hLayout2->setSpacing(0);
            hLayout2->addWidget(_p->colorProfileButton);
            hLayout2->addWidget(_p->displayProfileButton);
            hLayout->addLayout(hLayout2);
            layout->addLayout(hLayout);

            // Preferences.
            UI::Prefs prefs("djv::ViewLib::HistogramTool");
            prefs.get("colorProfile", _p->colorProfile);
            prefs.get("displayProfile", _p->displayProfile);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::HistogramTool", "Histogram"));
            widgetUpdate();

            // Setup the callbacks.
            connect(
                mainWindow,
                SIGNAL(imageChanged()),
                SLOT(widgetUpdate()));
            connect(
                _p->sizeWidget,
                SIGNAL(activated(int)),
                SLOT(sizeCallback(int)));
            connect(
                _p->maskWidget,
                SIGNAL(maskChanged(const djvPixel::Mask &)),
                SLOT(maskCallback(const djvPixel::Mask &)));
            connect(
                _p->colorProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->displayProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(displayProfileCallback(bool)));
        }

        HistogramTool::~HistogramTool()
        {
            UI::Prefs prefs("djv::ViewLib::HistogramTool");
            prefs.set("colorProfile", _p->colorProfile);
            prefs.set("displayProfile", _p->displayProfile);

            context()->makeGLContextCurrent();
            _p->openGLImage.reset();
        }

        void HistogramTool::sizeCallback(int in)
        {
            _p->size = static_cast<Util::HISTOGRAM>(in);
            widgetUpdate();
        }

        void HistogramTool::maskCallback(const Graphics::Pixel::Mask & mask)
        {
            _p->mask = mask;
            widgetUpdate();
        }

        void HistogramTool::colorProfileCallback(bool in)
        {
            _p->colorProfile = in;
            widgetUpdate();
        }

        void HistogramTool::displayProfileCallback(bool in)
        {
            _p->displayProfile = in;
            widgetUpdate();
        }

        void HistogramTool::showEvent(QShowEvent * event)
        {
            AbstractTool::showEvent(event);
            widgetUpdate();
        }

        void HistogramTool::resizeEvent(QResizeEvent * event)
        {
            AbstractTool::resizeEvent(event);
            widgetUpdate();
        }

        void HistogramTool::widgetUpdate()
        {
            //DJV_DEBUG("HistogramTool::widgetUpdate");
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->widget <<
                _p->minWidget <<
                _p->maxWidget <<
                _p->sizeWidget <<
                _p->maskWidget <<
                _p->colorProfileButton <<
                _p->displayProfileButton);
            if (isVisible())
            {
                if (const Graphics::PixelData * data = viewWidget()->data())
                {
                    try
                    {
                        context()->makeGLContextCurrent();
                        if (!_p->openGLImage)
                        {
                            _p->openGLImage.reset(new Graphics::OpenGLImage);
                        }
                        Graphics::OpenGLImageOptions options = viewWidget()->options();
                        //! \todo Why do we need to reverse the rotation here?
                        options.xform.rotate = options.xform.rotate;
                        const Core::Box2f bbox =
                            Graphics::OpenGLImageXform::xformMatrix(options.xform) *
                            Core::Box2f(data->size());
                        //DJV_DEBUG_PRINT("bbox = " << bbox);
                        options.xform.position = -bbox.position;
                        if (!_p->colorProfile)
                        {
                            options.colorProfile = Graphics::ColorProfile();
                        }
                        if (!_p->displayProfile)
                        {
                            options.displayProfile = DisplayProfile();
                        }
                        Graphics::PixelData tmp(Graphics::PixelDataInfo(bbox.size, data->pixel()));
                        _p->openGLImage->copy(*data, tmp, options);
                        _p->openGLImage->histogram(
                            tmp,
                            _p->histogram,
                            Util::histogramSize(_p->size),
                            _p->min,
                            _p->max,
                            _p->mask);
                    }
                    catch (Core::Error error)
                    {
                        error.add(Util::errorLabels()[Util::ERROR_HISTOGRAM]);
                        context()->printError(error);
                    }
                }
                else
                {
                    _p->histogram.zero();
                    _p->min.zero();
                    _p->max.zero();
                }
            }
            _p->widget->set(&_p->histogram, _p->min, _p->max);
            _p->minWidget->setText(Core::StringUtil::label(_p->min).join(", "));
            _p->maxWidget->setText(Core::StringUtil::label(_p->max).join(", "));
            _p->maskWidget->setMask(_p->mask);
            _p->colorProfileButton->setChecked(_p->colorProfile);
            _p->displayProfileButton->setChecked(_p->displayProfile);
        }

    } // namespace ViewLib
} // namespace djv

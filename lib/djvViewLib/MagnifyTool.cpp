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

#include <djvViewLib/MagnifyTool.h>

#include <djvViewLib/Context.h>
#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/MainWindow.h>
#include <djvViewLib/ToolActions.h>

#include <djvUI/IconLibrary.h>
#include <djvUI/IntEditSlider.h>
#include <djvUI/Prefs.h>
#include <djvUI/ToolButton.h>

#include <djvGraphics/OpenGLImage.h>
#include <djvGraphics/OpenGLOffscreenBuffer.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QPointer>
#include <QScopedPointer>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace
{
    class Widget : public QWidget
    {
    public:
        Widget();

        void setPixmap(const QPixmap &);
        
    protected:
        void paintEvent(QPaintEvent *) override;

    private:
        QPixmap _pixmap;
    };

    Widget::Widget()
    {
        setAttribute(Qt::WA_OpaquePaintEvent);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void Widget::setPixmap(const QPixmap & pixmap)
    {
        _pixmap = pixmap;
        update();
    }

    void Widget::paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::black);
        if (! _pixmap.isNull())
        {
            painter.drawPixmap(0, 0, _pixmap);
        }
    }

} // namespace

namespace djv
{
    namespace ViewLib
    {
        struct MagnifyTool::Private
        {
            glm::ivec2 pick = glm::ivec2(0, 0);
            int zoom = 2;
            bool colorProfile = true;
            bool displayProfile = true;
            bool pixelDataInit = false;
            std::unique_ptr<Graphics::OpenGLImage> openGLImage;
            QPointer<Widget> widget;
            QPointer<UI::IntEditSlider> slider;
            QPointer<UI::ToolButton> colorProfileButton;
            QPointer<UI::ToolButton> displayProfileButton;
            QPointer<QHBoxLayout> hLayout;
        };

        MagnifyTool::MagnifyTool(
            const QPointer<MainWindow> & mainWindow,
            const QPointer<Context> & context,
            QWidget * parent) :
            AbstractTool(mainWindow, context, parent),
            _p(new Private)
        {
            // Create the widgets.
            _p->widget = new Widget;

            _p->slider = new UI::IntEditSlider(context.data());
            _p->slider->setRange(1, 10);
            _p->slider->setResetToDefault(false);
            _p->slider->setToolTip(
                qApp->translate("djv::ViewLib::MagnifyTool", "Magnification level"));

            _p->colorProfileButton = new UI::ToolButton(context.data());
            _p->colorProfileButton->setCheckable(true);
            _p->colorProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::MagnifyTool", "Enable the color profile"));

            _p->displayProfileButton = new UI::ToolButton(context.data());
            _p->displayProfileButton->setCheckable(true);
            _p->displayProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::MagnifyTool", "Enable the display profile"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);
            layout->addWidget(_p->widget, 1);

            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->slider);

            _p->hLayout = new QHBoxLayout;
            _p->hLayout->setMargin(0);
            _p->hLayout->addWidget(_p->colorProfileButton);
            _p->hLayout->addWidget(_p->displayProfileButton);
            hLayout->addLayout(_p->hLayout);
            layout->addLayout(hLayout);

            // Preferences.
            UI::Prefs prefs("djv::ViewLib::MagnifyTool");
            prefs.get("zoom", _p->zoom);
            prefs.get("colorProfile", _p->colorProfile);
            prefs.get("displayProfile", _p->displayProfile);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::MagnifyTool", "Magnify"));
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                mainWindow,
                SIGNAL(imageChanged()),
                SLOT(widgetUpdate()));
            connect(
                mainWindow->viewWidget(),
                SIGNAL(pickChanged(const glm::ivec2 &)),
                SLOT(pickCallback(const glm::ivec2 &)));
            connect(
                _p->slider,
                SIGNAL(valueChanged(int)),
                SLOT(sliderCallback(int)));
            connect(
                _p->colorProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->displayProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(displayProfileCallback(bool)));
        }

        MagnifyTool::~MagnifyTool()
        {
            //DJV_DEBUG("MagnifyTool::~MagnifyTool");
            UI::Prefs prefs("djv::ViewLib::MagnifyTool");
            prefs.set("zoom", _p->zoom);
            prefs.set("colorProfile", _p->colorProfile);
            prefs.set("displayProfile", _p->displayProfile);

            context()->makeGLContextCurrent();
            _p->openGLImage.reset();
        }

        void MagnifyTool::showEvent(QShowEvent *)
        {
            widgetUpdate();
        }

        bool MagnifyTool::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractTool::event(event);
        }

        void MagnifyTool::pickCallback(const glm::ivec2 & in)
        {
            _p->pick = in;
            widgetUpdate();
        }

        void MagnifyTool::sliderCallback(int in)
        {
            _p->zoom = in - 1;
            widgetUpdate();
        }

        void MagnifyTool::colorProfileCallback(bool in)
        {
            _p->colorProfile = in;
            widgetUpdate();
        }

        void MagnifyTool::displayProfileCallback(bool in)
        {
            _p->displayProfile = in;
            widgetUpdate();
        }

        void MagnifyTool::styleUpdate()
        {
            _p->colorProfileButton->setIcon(context()->iconLibrary()->icon("djv/UI/DisplayProfileIcon"));
            _p->displayProfileButton->setIcon(context()->iconLibrary()->icon("djv/UI/DisplayProfileIcon"));
            _p->hLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }
        
        void MagnifyTool::widgetUpdate()
        {
            //DJV_DEBUG("MagnifyTool::widgetUpdate");
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->slider <<
                _p->colorProfileButton <<
                _p->displayProfileButton);
            _p->slider->setValue(_p->zoom + 1);
            _p->colorProfileButton->setChecked(_p->colorProfile);
            _p->displayProfileButton->setChecked(_p->displayProfile);
            if (!_p->pixelDataInit && isVisible())
            {
                _p->pixelDataInit = true;
                QTimer::singleShot(0, this, SLOT(pixelDataUpdate()));
            }
        }

        void MagnifyTool::pixelDataUpdate()
        {
            //DJV_DEBUG("MagnifyTool::pixelDataUpdate");
            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->widget);
            QPixmap pixmap;
            const glm::ivec2 size(_p->widget->width(), _p->widget->height());
            if (Core::VectorUtil::isSizeValid(size))
            {
                Graphics::PixelData tmp(Graphics::PixelDataInfo(
                    size,
                    Graphics::Pixel::RGB_U8));
                if (const Graphics::PixelData * data = viewWidget()->data())
                {
                    //DJV_DEBUG_PRINT("data = " << *data);
                    const float zoom = Core::Math::pow(2, _p->zoom);
                    glm::ivec2 pick = Core::VectorUtil::floor(
                        glm::vec2(_p->pick - viewWidget()->viewPos()) * zoom -
                        glm::vec2(tmp.info().size) / 2.f);
                    //DJV_DEBUG_PRINT("zoom = " << zoom);
                    //DJV_DEBUG_PRINT("pick = " << pick);
                    try
                    {
                        context()->makeGLContextCurrent();
                        if (!_p->openGLImage)
                        {
                            _p->openGLImage.reset(new Graphics::OpenGLImage);
                        }
                        Graphics::OpenGLImageOptions options = viewWidget()->options();
                        options.xform.position -= pick;
                        options.xform.scale *= zoom * viewWidget()->viewZoom();
                        if (!_p->colorProfile)
                        {
                            options.colorProfile = Graphics::ColorProfile();
                        }
                        if (!_p->displayProfile)
                        {
                            options.displayProfile = DisplayProfile();
                        }
                        _p->openGLImage->copy(*data, tmp, options);
                        pixmap = _p->openGLImage->toQt(tmp);
                    }
                    catch (Core::Error error)
                    {
                        error.add(Enum::errorLabels()[Enum::ERROR_MAGNIFY]);
                        context()->printError(error);
                    }
                }
            }
            _p->widget->setPixmap(pixmap);
            _p->pixelDataInit = false;
        }

    } // namespace ViewLib
} // namespace djv

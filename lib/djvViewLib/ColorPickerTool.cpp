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

#include <djvViewLib/ColorPickerTool.h>

#include <djvViewLib/DisplayProfile.h>
#include <djvViewLib/ImageView.h>
#include <djvViewLib/Session.h>
#include <djvViewLib/ViewContext.h>

#include <djvUI/ColorSwatch.h>
#include <djvUI/ColorWidget.h>
#include <djvUI/IconLibrary.h>
#include <djvUI/IntEditSlider.h>
#include <djvUI/Prefs.h>
#include <djvUI/ToolButton.h>

#include <djvAV/OpenGLImage.h>
#include <djvAV/OpenGLOffscreenBuffer.h>

#include <djvCore/Debug.h>
#include <djvCore/Error.h>
#include <djvCore/SignalBlocker.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QPointer>
#include <QScopedPointer>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace djv
{
    namespace ViewLib
    {
        struct ColorPickerTool::Private
        {
            std::shared_ptr<AV::Image> image;
            glm::ivec2 pick = glm::ivec2(0, 0);
            glm::ivec2 viewPos = glm::ivec2(0, 0);
            float viewZoom = 0.f;
            AV::Color sample = AV::Pixel::RGBA_F32;
            int size = 4;
            bool colorProfile = true;
            bool displayProfile = true;
            bool lock = false;
            std::unique_ptr<AV::OpenGLImage> openGLImage;
            AV::OpenGLImageOptions openGLImageOptions;

            QPointer<UI::ColorWidget> widget;
            QPointer<UI::ColorSwatch> swatch;
            QPointer<UI::IntEditSlider> sizeSlider;
            QPointer<UI::ToolButton> colorProfileButton;
            QPointer<UI::ToolButton> displayProfileButton;
            QPointer<UI::ToolButton> lockWidget;
            QPointer<QHBoxLayout> hLayout;
        };

        ColorPickerTool::ColorPickerTool(
            const QPointer<Session> & session,
            const QPointer<ViewContext> & context,
            QWidget * parent) :
            AbstractTool(session, context, parent),
            _p(new Private)
        {
            //DJV_DEBUG("ColorPickerTool::ColorPickerTool");

            // Create the widgets.
            _p->widget = new UI::ColorWidget(context.data());

            _p->swatch = new UI::ColorSwatch(context.data());
            _p->swatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            _p->sizeSlider = new UI::IntEditSlider(context.data());
            _p->sizeSlider->setRange(1, 100);
            _p->sizeSlider->setResetToDefault(false);
            _p->sizeSlider->setToolTip(
                qApp->translate("djv::ViewLib::ColorPickerTool", "Color picker sample size"));

            _p->colorProfileButton = new UI::ToolButton(context.data());
            _p->colorProfileButton->setCheckable(true);
            _p->colorProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::ColorPickerTool", "Enable the color profile"));

            _p->displayProfileButton = new UI::ToolButton(context.data());
            _p->displayProfileButton->setCheckable(true);
            _p->displayProfileButton->setToolTip(
                qApp->translate("djv::ViewLib::ColorPickerTool", "Enable the display profile"));

            _p->lockWidget = new UI::ToolButton(context.data());
            _p->lockWidget->setCheckable(true);
            _p->lockWidget->setToolTip(
                qApp->translate("djv::ViewLib::ColorPickerTool", "Lock the pixel format and type"));

            // Layout the widgets.
            auto layout = new QVBoxLayout(this);

            auto hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->swatch);
            hLayout->addWidget(_p->widget, 1);
            layout->addLayout(hLayout);

            hLayout = new QHBoxLayout;
            hLayout->setMargin(0);
            hLayout->addWidget(_p->sizeSlider);

            _p->hLayout = new QHBoxLayout;
            _p->hLayout->setMargin(0);
            _p->hLayout->addWidget(_p->colorProfileButton);
            _p->hLayout->addWidget(_p->displayProfileButton);
            hLayout->addLayout(_p->hLayout);

            layout->addLayout(hLayout);

            _p->widget->bottomLayout()->insertWidget(1, _p->lockWidget);

            // Preferences.
            UI::Prefs prefs("djv::ViewLib::ColorPickerTool");
            prefs.get("size", _p->size);
            AV::Pixel::PIXEL pixel = _p->sample.pixel();
            prefs.get("pixel", pixel);
            _p->sample.setPixel(pixel);
            prefs.get("colorProfile", _p->colorProfile);
            prefs.get("displayProfile", _p->displayProfile);
            prefs.get("lock", _p->lock);

            // Initialize.
            setWindowTitle(qApp->translate("djv::ViewLib::ColorPickerTool", "Color Picker"));
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            styleUpdate();
            widgetUpdate();

            // Setup the callbacks.
            connect(
                session,
                &Session::imageChanged,
                [this](const std::shared_ptr<AV::Image> & value)
            {
                _p->image = value;
                widgetUpdate();
            });
            connect(
                session,
                &Session::imageOptionsChanged,
                [this](const AV::OpenGLImageOptions & value)
            {
                _p->openGLImageOptions = value;
                widgetUpdate();
            });
            auto viewWidget = session->viewWidget();
            connect(
                viewWidget,
                &ImageView::viewPosChanged,
                [this](const glm::ivec2 & value)
            {
                _p->viewPos = value;
                widgetUpdate();
            });
            connect(
                viewWidget,
                &ImageView::viewZoomChanged,
                [this](float value)
            {
                _p->viewZoom = value;
                widgetUpdate();
            });
            connect(
                viewWidget,
                SIGNAL(pickChanged(const glm::ivec2 &)),
                SLOT(pickCallback(const glm::ivec2 &)));
            connect(
                _p->widget,
                SIGNAL(colorChanged(const djv::AV::Color &)),
                SLOT(widgetCallback(const djv::AV::Color &)));
            connect(
                _p->sizeSlider,
                SIGNAL(valueChanged(int)),
                SLOT(sizeCallback(int)));
            connect(
                _p->colorProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(colorProfileCallback(bool)));
            connect(
                _p->displayProfileButton,
                SIGNAL(toggled(bool)),
                SLOT(displayProfileCallback(bool)));
            connect(
                _p->lockWidget,
                SIGNAL(toggled(bool)),
                SLOT(lockCallback(bool)));
        }

        ColorPickerTool::~ColorPickerTool()
        {
            //DJV_DEBUG("ColorPickerTool::~ColorPickerTool");

            UI::Prefs prefs("djv::ViewLib::ColorPickerTool");
            prefs.set("size", _p->size);
            prefs.set("pixel", _p->sample.pixel());
            prefs.set("colorProfile", _p->colorProfile);
            prefs.set("displayProfile", _p->displayProfile);
            prefs.set("lock", _p->lock);

            if (_p->openGLImage)
            {
                context()->makeGLContextCurrent();
                _p->openGLImage.reset();
            }
        }

        void ColorPickerTool::showEvent(QShowEvent *)
        {
            widgetUpdate();
        }

        bool ColorPickerTool::event(QEvent * event)
        {
            if (QEvent::StyleChange == event->type())
            {
                styleUpdate();
            }
            return AbstractTool::event(event);
        }

        void ColorPickerTool::pickCallback(const glm::ivec2 & in)
        {
            _p->pick = in;
            widgetUpdate();
        }

        void ColorPickerTool::widgetCallback(const AV::Color & color)
        {
            _p->sample = color;
            _p->swatch->setColor(color);
        }

        void ColorPickerTool::sizeCallback(int value)
        {
            _p->size = value;
            widgetUpdate();
        }

        void ColorPickerTool::colorProfileCallback(bool in)
        {
            _p->colorProfile = in;
            widgetUpdate();
        }

        void ColorPickerTool::displayProfileCallback(bool in)
        {
            _p->displayProfile = in;
            widgetUpdate();
        }

        void ColorPickerTool::lockCallback(bool in)
        {
            _p->lock = in;
            widgetUpdate();
        }

        void ColorPickerTool::styleUpdate()
        {
            _p->colorProfileButton->setIcon(context()->iconLibrary()->icon("djv/UI/DisplayProfileIcon"));
            _p->displayProfileButton->setIcon(context()->iconLibrary()->icon("djv/UI/DisplayProfileIcon"));
            _p->lockWidget->setIcon(context()->iconLibrary()->icon("djv/UI/UnlockIcon", "djv/UI/LockIcon"));
            _p->hLayout->setSpacing(style()->pixelMetric(QStyle::PM_ToolBarItemSpacing));
        }

        void ColorPickerTool::widgetUpdate()
        {
            //DJV_DEBUG("ColorPickerTool::swatchUpdate");
            //DJV_DEBUG_PRINT("color profile = " << _p->colorProfile);
            //DJV_DEBUG_PRINT("display profile = " << _p->displayProfile);
            //DJV_DEBUG_PRINT("lock = " << _p->lock);

            Core::SignalBlocker signalBlocker(QObjectList() <<
                _p->widget <<
                _p->swatch <<
                _p->sizeSlider <<
                _p->colorProfileButton <<
                _p->displayProfileButton <<
                _p->lockWidget);
            _p->sizeSlider->setValue(_p->size);
            _p->colorProfileButton->setChecked(_p->colorProfile);
            _p->displayProfileButton->setChecked(_p->displayProfile);
            _p->lockWidget->setChecked(_p->lock);

            if (!_p->lock && _p->image)
            {
                _p->sample.setPixel(_p->image->pixel());
            }
            else
            {
                _p->sample.setPixel(_p->widget->color().pixel());
            }

            if (isVisible() && _p->image)
            {
                try
                {
                    //DJV_DEBUG_PRINT("image = " << *_p->image);

                    const glm::ivec2 pick = Core::VectorUtil::floor(glm::vec2(_p->pick - _p->viewPos) / _p->viewZoom);
                    //DJV_DEBUG_PRINT("pick = " << _p->pick);
                    //DJV_DEBUG_PRINT("pick size = " << _p->size);

                    context()->makeGLContextCurrent();
                    if (!_p->openGLImage)
                    {
                        _p->openGLImage.reset(new AV::OpenGLImage);
                    }

                    AV::OpenGLImageOptions options = _p->openGLImageOptions;
                    options.xform.position -= pick - glm::ivec2((_p->size - 1) / 2);
                    if (!_p->colorProfile)
                    {
                        options.colorProfile = AV::ColorProfile();
                    }
                    if (!_p->displayProfile)
                    {
                        options.displayProfile = DisplayProfile();
                    }
                    //DJV_DEBUG_PRINT("color profile = " << options.colorProfile);

                    AV::PixelData tmp(AV::PixelDataInfo(glm::ivec2(_p->size, _p->size), _p->sample.pixel()));
                    _p->openGLImage->copy(*_p->image, tmp, options);
                    _p->openGLImage->average(tmp, _p->sample);
                }
                catch (Core::Error error)
                {
                    error.add(Enum::errorLabels()[Enum::ERROR_PICK_COLOR]);
                    context()->printError(error);
                }
                //DJV_DEBUG_PRINT("sample = " << _p->sample);
            }

            _p->widget->setColor(_p->sample);
            _p->swatch->setColor(_p->sample);
        }

    } // namespace ViewLib
} // namespace djv

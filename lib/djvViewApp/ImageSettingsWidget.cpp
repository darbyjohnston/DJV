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

#include <djvViewApp/ImageSettingsWidget.h>

#include <djvViewApp/ImageSettings.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvAV/AVSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageAlphaBlendSettingsWidget::Private
        {
            AV::AlphaBlend alphaBlend = AV::AlphaBlend::First;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::shared_ptr<ValueObserver<AV::AlphaBlend> > alphaBlendObserver;
        };

        void ImageAlphaBlendSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageAlphaBlendSettingsWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            _widgetUpdate();

            auto contextWeak = std::weak_ptr<Context>(context);
            auto weak = std::weak_ptr<ImageAlphaBlendSettingsWidget>(std::dynamic_pointer_cast<ImageAlphaBlendSettingsWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto avSystem = context->getSystemT<AV::AVSystem>();
                            avSystem->setAlphaBlend(static_cast<AV::AlphaBlend>(value));
                        }
                    }
                });

            auto avSystem = context->getSystemT<AV::AVSystem>();
            p.alphaBlendObserver = ValueObserver<AV::AlphaBlend>::create(
                avSystem->observeAlphaBlend(),
                [weak](AV::AlphaBlend value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->alphaBlend = value;
                        widget->_p->comboBox->setCurrentItem(static_cast<int>(value));
                    }
                });
        }

        ImageAlphaBlendSettingsWidget::ImageAlphaBlendSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageAlphaBlendSettingsWidget> ImageAlphaBlendSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageAlphaBlendSettingsWidget>(new ImageAlphaBlendSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageAlphaBlendSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Alpha Blend");
        }

        std::string ImageAlphaBlendSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Image");
        }

        std::string ImageAlphaBlendSettingsWidget::getSettingsSortKey() const
        {
            return "F";
        }

        void ImageAlphaBlendSettingsWidget::_textUpdateEvent(Event::TextUpdate& event)
        {
            ISettingsWidget::_textUpdateEvent(event);
            _widgetUpdate();
        }

        void ImageAlphaBlendSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            for (auto i : AV::getAlphaBlendEnums())
            {
                std::stringstream ss;
                ss << i;
                p.comboBox->addItem(_getText(ss.str()));
            }
            p.comboBox->setCurrentItem(static_cast<int>(p.alphaBlend));
        }

        struct ImageAspectRatioSettingsWidget::Private
        {
            UI::ImageAspectRatio aspectRatio = UI::ImageAspectRatio::First;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::shared_ptr<ValueObserver<UI::ImageAspectRatio> > aspectRatioObserver;
        };

        void ImageAspectRatioSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageAspectRatioSettingsWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            _widgetUpdate();

            auto contextWeak = std::weak_ptr<Context>(context);
            auto weak = std::weak_ptr<ImageAspectRatioSettingsWidget>(std::dynamic_pointer_cast<ImageAspectRatioSettingsWidget>(shared_from_this()));
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                imageSettings->setAspectRatio(static_cast<UI::ImageAspectRatio>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.aspectRatioObserver = ValueObserver<UI::ImageAspectRatio>::create(
                    imageSettings->observeAspectRatio(),
                    [weak](UI::ImageAspectRatio value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->aspectRatio = value;
                            widget->_p->comboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        ImageAspectRatioSettingsWidget::ImageAspectRatioSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageAspectRatioSettingsWidget> ImageAspectRatioSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageAspectRatioSettingsWidget>(new ImageAspectRatioSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageAspectRatioSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Aspect Ratio");
        }

        std::string ImageAspectRatioSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Image");
        }

        std::string ImageAspectRatioSettingsWidget::getSettingsSortKey() const
        {
            return "F";
        }

        void ImageAspectRatioSettingsWidget::_textUpdateEvent(Event::TextUpdate& event)
        {
            ISettingsWidget::_textUpdateEvent(event);
            _widgetUpdate();
        }

        void ImageAspectRatioSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            for (auto i : UI::getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                p.comboBox->addItem(_getText(ss.str()));
            }
            p.comboBox->setCurrentItem(static_cast<int>(p.aspectRatio));
        }

        struct ImageRotateSettingsWidget::Private
        {
            ImageRotate rotate = ImageRotate::First;
            std::shared_ptr<UI::ComboBox> comboBox;
            std::shared_ptr<ValueObserver<ImageRotate> > rotateObserver;
        };

        void ImageRotateSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageRotateSettingsWidget");

            p.comboBox = UI::ComboBox::create(context);
            addChild(p.comboBox);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageRotateSettingsWidget>(std::dynamic_pointer_cast<ImageRotateSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.comboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<UI::Settings::System>();
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                imageSettings->setRotate(static_cast<ImageRotate>(value));
                            }
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.rotateObserver = ValueObserver<ImageRotate>::create(
                    imageSettings->observeRotate(),
                    [weak](ImageRotate value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->rotate = value;
                            widget->_p->comboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        ImageRotateSettingsWidget::ImageRotateSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageRotateSettingsWidget> ImageRotateSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ImageRotateSettingsWidget>(new ImageRotateSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageRotateSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Rotate");
        }

        std::string ImageRotateSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Image");
        }

        std::string ImageRotateSettingsWidget::getSettingsSortKey() const
        {
            return "F";
        }

        void ImageRotateSettingsWidget::_textUpdateEvent(Event::TextUpdate& event)
        {
            ISettingsWidget::_textUpdateEvent(event);
            _widgetUpdate();
        }

        void ImageRotateSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.comboBox->clearItems();
            for (auto i : getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                p.comboBox->addItem(_getText(ss.str()));
            }
            p.comboBox->setCurrentItem(static_cast<int>(p.rotate));
        }

    } // namespace ViewApp
} // namespace djv


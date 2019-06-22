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
#include <djvUI/ToggleButton.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageSettingsWidget::Private
        {
            std::vector<std::string> colorSpaces;
            std::vector<AV::OCIODisplay> colorDisplays;
            std::string inputColorSpace;
            std::string colorDisplay;
            std::string colorView;
            ImageRotate rotate = ImageRotate::First;
            ImageAspectRatio aspectRatio = ImageAspectRatio::First;

            std::shared_ptr<UI::ComboBox> inputColorSpaceComboBox;
            std::shared_ptr<UI::ComboBox> colorDisplayComboBox;
            std::shared_ptr<UI::ComboBox> colorViewComboBox;
            std::shared_ptr<UI::ComboBox> rotateComboBox;
            std::shared_ptr<UI::ComboBox> aspectRatioComboBox;
            std::shared_ptr<UI::FormLayout> formLayout;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<AV::OCIODisplay> > colorDisplaysObserver;
            std::shared_ptr<ValueObserver<std::string> > inputColorSpaceObserver;
            std::shared_ptr<ValueObserver<std::string> > colorDisplayObserver;
            std::shared_ptr<ValueObserver<std::string> > colorViewObserver;
            std::shared_ptr<ValueObserver<ImageRotate> > rotateObserver;
            std::shared_ptr<ValueObserver<ImageAspectRatio> > aspectRatioObserver;
        };

        void ImageSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageSettingsWidget");

            p.inputColorSpaceComboBox = UI::ComboBox::create(context);
            p.colorDisplayComboBox = UI::ComboBox::create(context);
            p.colorViewComboBox = UI::ComboBox::create(context);
            p.rotateComboBox = UI::ComboBox::create(context);
            p.aspectRatioComboBox = UI::ComboBox::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.inputColorSpaceComboBox);
            p.formLayout->addChild(p.colorDisplayComboBox);
            p.formLayout->addChild(p.colorViewComboBox);
            p.formLayout->addChild(p.rotateComboBox);
            p.formLayout->addChild(p.aspectRatioComboBox);
            addChild(p.formLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageSettingsWidget>(std::dynamic_pointer_cast<ImageSettingsWidget>(shared_from_this()));
            p.inputColorSpaceComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                        {
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                if (value >= 0 && value < widget->_p->colorSpaces.size())
                                {
                                    imageSettings->setInputColorSpace(widget->_p->colorSpaces[value]);
                                }
                            }
                        }
                    }
                });

            p.colorDisplayComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                        {
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                if (value >= 0 && value < widget->_p->colorDisplays.size())
                                {
                                    imageSettings->setColorDisplay(widget->_p->colorDisplays[value].name);
                                }
                            }
                        }
                    }
                });

            p.colorViewComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                        {
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                for (size_t i = 0; i < widget->_p->colorDisplays.size(); ++i)
                                {
                                    if (widget->_p->colorDisplay == widget->_p->colorDisplays[i].name)
                                    {
                                        if (value >= 0 && value < widget->_p->colorDisplays[i].views.size())
                                        {
                                            imageSettings->setColorView(widget->_p->colorDisplays[i].views[value].name);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                });

            p.rotateComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                        {
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                imageSettings->setImageRotate(static_cast<ImageRotate>(value));
                            }
                        }
                    }
                });

            p.aspectRatioComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto settingsSystem = context->getSystemT<UI::Settings::System>())
                        {
                            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                            {
                                imageSettings->setImageAspectRatio(static_cast<ImageAspectRatio>(value));
                            }
                        }
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIOSystem>();
            p.colorSpacesObserver = ListObserver<std::string>::create(
                ocioSystem->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorSpaces = value;
                        widget->_widgetUpdate();
                    }
                });

            p.colorDisplaysObserver = ListObserver<AV::OCIODisplay>::create(
                ocioSystem->observeDisplays(),
                [weak](const std::vector<AV::OCIODisplay>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorDisplays = value;
                        widget->_widgetUpdate();
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.inputColorSpaceObserver = ValueObserver<std::string>::create(
                    imageSettings->observeInputColorSpace(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->inputColorSpace = value;
                            widget->_widgetUpdate();
                        }
                    });

                p.colorDisplayObserver = ValueObserver<std::string>::create(
                    imageSettings->observeColorDisplay(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorDisplay = value;
                            widget->_widgetUpdate();
                        }
                    });

                p.colorViewObserver = ValueObserver<std::string>::create(
                    imageSettings->observeColorView(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorView = value;
                            widget->_widgetUpdate();
                        }
                    });

                p.rotateObserver = ValueObserver<ImageRotate>::create(
                    imageSettings->observeImageRotate(),
                    [weak](ImageRotate value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->rotate = value;
                            widget->_p->rotateComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });

                p.aspectRatioObserver = ValueObserver<ImageAspectRatio>::create(
                    imageSettings->observeImageAspectRatio(),
                    [weak](ImageAspectRatio value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->aspectRatio = value;
                            widget->_p->aspectRatioComboBox->setCurrentItem(static_cast<int>(value));
                        }
                    });
            }
        }

        ImageSettingsWidget::ImageSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageSettingsWidget> ImageSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<ImageSettingsWidget>(new ImageSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Image");
        }

        std::string ImageSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("DJV");
        }

        std::string ImageSettingsWidget::getSettingsSortKey() const
        {
            return "B";
        }

        void ImageSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            _widgetUpdate();
        }

        void ImageSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.formLayout->setText(p.inputColorSpaceComboBox, _getText(DJV_TEXT("Input color space")) + ":");
            p.inputColorSpaceComboBox->setItems(p.colorSpaces);

            p.formLayout->setText(p.colorDisplayComboBox, _getText(DJV_TEXT("Display")) + ":");
            p.formLayout->setText(p.colorViewComboBox, _getText(DJV_TEXT("View:")) + ":");
            p.colorDisplayComboBox->clearItems();
            p.colorViewComboBox->clearItems();
            for (size_t i = 0; i < p.colorDisplays.size(); ++i)
            {
                const auto& colorDisplay = p.colorDisplays[i];
                p.colorDisplayComboBox->addItem(colorDisplay.name);
                if (colorDisplay.name == p.colorDisplay)
                {
                    p.colorDisplayComboBox->setCurrentItem(i);
                    for (size_t j = 0; j < colorDisplay.views.size(); ++j)
                    {
                        const auto& colorView = colorDisplay.views[j];
                        p.colorViewComboBox->addItem(colorView.name);
                        if (colorView.name == p.colorView)
                        {
                            p.colorViewComboBox->setCurrentItem(j);
                        }
                    }
                }
            }

            p.formLayout->setText(p.rotateComboBox, _getText(DJV_TEXT("Rotate")) + ":");
            p.rotateComboBox->clearItems();
            for (auto i : getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                p.rotateComboBox->addItem(_getText(ss.str()));
            }
            p.rotateComboBox->setCurrentItem(static_cast<int>(p.rotate));
            
            p.formLayout->setText(p.aspectRatioComboBox, _getText(DJV_TEXT("Aspect ratio")) + ":");
            p.aspectRatioComboBox->clearItems();
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                p.aspectRatioComboBox->addItem(_getText(ss.str()));
            }
            p.aspectRatioComboBox->setCurrentItem(static_cast<int>(p.aspectRatio));
        }

    } // namespace ViewApp
} // namespace djv


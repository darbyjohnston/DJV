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

#include <djvViewApp/ColorSpaceModel.h>
#include <djvViewApp/ImageSettings.h>

#include <djvUI/ButtonGroup.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FlowLayout.h>
#include <djvUI/FormLayout.h>
#include <djvUI/ListButton.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ImageAspectRatioSettingsWidget::Private
        {
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::map<ImageAspectRatio, std::shared_ptr<UI::ListButton> > buttons;
            std::shared_ptr<UI::FlowLayout> layout;
            std::shared_ptr<ValueObserver<ImageAspectRatio> > aspectRatioObserver;
        };

        void ImageAspectRatioSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageAspectRatioSettingsWidget");

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            for (auto i : getImageAspectRatioEnums())
            {
                auto button = UI::ListButton::create(context);
                p.buttonGroup->addButton(button);
                p.buttons[i] = button;
            }

            p.layout = UI::FlowLayout::create(context);
            for (auto i : p.buttonGroup->getButtons())
            {
                p.layout->addChild(i);
            }
            addChild(p.layout);

            auto weak = std::weak_ptr<ImageAspectRatioSettingsWidget>(std::dynamic_pointer_cast<ImageAspectRatioSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                        {
                            imageSettings->setImageAspectRatio(static_cast<ImageAspectRatio>(value));
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.aspectRatioObserver = ValueObserver<ImageAspectRatio>::create(
                    imageSettings->observeImageAspectRatio(),
                    [weak](ImageAspectRatio value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->buttonGroup->setChecked(static_cast<int>(value));
                        }
                    });
            }
        }

        ImageAspectRatioSettingsWidget::ImageAspectRatioSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageAspectRatioSettingsWidget> ImageAspectRatioSettingsWidget::create(Context* context)
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

        void ImageAspectRatioSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            auto buttons = p.buttonGroup->getButtons();
            for (auto i : getImageAspectRatioEnums())
            {
                std::stringstream ss;
                ss << i;
                p.buttons[i]->setText(ss.str());
            }
        }

        struct ImageColorSpaceSettingsWidget::Private
        {
            std::shared_ptr<ColorSpaceModel> colorSpaceModel;

            std::shared_ptr<UI::ComboBox> inputColorSpaceComboBox;
            std::shared_ptr<UI::ComboBox> colorDisplayComboBox;
            std::shared_ptr<UI::ComboBox> colorViewComboBox;
            std::shared_ptr<UI::FormLayout> formLayout;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<std::string> > colorDisplaysObserver;
            std::shared_ptr<ListObserver<std::string> > colorViewsObserver;
            std::shared_ptr<ValueObserver<std::string> > inputColorSpaceObserver;
            std::shared_ptr<ValueObserver<std::string> > inputColorSpaceObserver2;
            std::shared_ptr<ValueObserver<std::string> > colorDisplayObserver;
            std::shared_ptr<ValueObserver<std::string> > colorDisplayObserver2;
            std::shared_ptr<ValueObserver<std::string> > colorViewObserver;
            std::shared_ptr<ValueObserver<std::string> > colorViewObserver2;
        };

        void ImageColorSpaceSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageColorSpaceSettingsWidget");

            p.colorSpaceModel = ColorSpaceModel::create(context);

            p.inputColorSpaceComboBox = UI::ComboBox::create(context);
            p.colorDisplayComboBox = UI::ComboBox::create(context);
            p.colorViewComboBox = UI::ComboBox::create(context);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->addChild(p.inputColorSpaceComboBox);
            p.formLayout->addChild(p.colorDisplayComboBox);
            p.formLayout->addChild(p.colorViewComboBox);
            addChild(p.formLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ImageColorSpaceSettingsWidget>(std::dynamic_pointer_cast<ImageColorSpaceSettingsWidget>(shared_from_this()));
            p.inputColorSpaceComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                        {
                            imageSettings->setColorSpace(widget->_p->colorSpaceModel->indexToColorSpace(value));
                        }
                    }
                });

            p.colorDisplayComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                        {
                            imageSettings->setColorDisplay(widget->_p->colorSpaceModel->indexToDisplay(value));
                        }
                    }
                });

            p.colorViewComboBox->setCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                        {
                            imageSettings->setColorView(widget->_p->colorSpaceModel->indexToView(value));
                        }
                    }
                });

            p.colorSpacesObserver = ListObserver<std::string>::create(
                p.colorSpaceModel->observeColorSpaces(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->inputColorSpaceComboBox->setItems(value);
                    }
                });

            p.colorDisplaysObserver = ListObserver<std::string>::create(
                p.colorSpaceModel->observeDisplays(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorDisplayComboBox->setItems(value);
                    }
                });

            p.colorViewsObserver = ListObserver<std::string>::create(
                p.colorSpaceModel->observeViews(),
                [weak](const std::vector<std::string>& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorViewComboBox->setItems(value);
                    }
                });

            p.inputColorSpaceObserver = ValueObserver<std::string>::create(
                p.colorSpaceModel->observeColorSpace(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->inputColorSpaceComboBox->setCurrentItem(widget->_p->colorSpaceModel->colorSpaceToIndex(value));
                    }
                });

            p.colorDisplayObserver = ValueObserver<std::string>::create(
                p.colorSpaceModel->observeDisplay(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorDisplayComboBox->setCurrentItem(widget->_p->colorSpaceModel->displayToIndex(value));
                    }
                });

            p.colorViewObserver = ValueObserver<std::string>::create(
                p.colorSpaceModel->observeView(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->colorViewComboBox->setCurrentItem(widget->_p->colorSpaceModel->viewToIndex(value));
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.inputColorSpaceObserver2 = ValueObserver<std::string>::create(
                    imageSettings->observeColorSpace(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorSpaceModel->setColorSpace(value);
                        }
                    });

                p.colorDisplayObserver2 = ValueObserver<std::string>::create(
                    imageSettings->observeColorDisplay(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorSpaceModel->setDisplay(value);
                        }
                    });

                p.colorViewObserver2 = ValueObserver<std::string>::create(
                    imageSettings->observeColorView(),
                    [weak](const std::string& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->colorSpaceModel->setView(value);
                        }
                    });
            }
        }

        ImageColorSpaceSettingsWidget::ImageColorSpaceSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageColorSpaceSettingsWidget> ImageColorSpaceSettingsWidget::create(Context* context)
        {
            auto out = std::shared_ptr<ImageColorSpaceSettingsWidget>(new ImageColorSpaceSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ImageColorSpaceSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Color Space");
        }

        std::string ImageColorSpaceSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("Image");
        }

        std::string ImageColorSpaceSettingsWidget::getSettingsSortKey() const
        {
            return "F";
        }

        void ImageColorSpaceSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            _widgetUpdate();
        }

        void ImageColorSpaceSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.inputColorSpaceComboBox, _getText(DJV_TEXT("Input color space")) + ":");
            p.formLayout->setText(p.colorDisplayComboBox, _getText(DJV_TEXT("Color display")) + ":");
            p.formLayout->setText(p.colorViewComboBox, _getText(DJV_TEXT("Color view")) + ":");
        }

        struct ImageRotateSettingsWidget::Private
        {
            std::shared_ptr<UI::ButtonGroup> buttonGroup;
            std::map<ImageRotate, std::shared_ptr<UI::ListButton> > buttons;
            std::shared_ptr<UI::FlowLayout> layout;
            std::shared_ptr<ValueObserver<ImageRotate> > rotateObserver;
        };

        void ImageRotateSettingsWidget::_init(Context* context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ImageRotateSettingsWidget");

            p.buttonGroup = UI::ButtonGroup::create(UI::ButtonType::Radio);
            for (auto i : getImageRotateEnums())
            {
                auto button = UI::ListButton::create(context);
                p.buttonGroup->addButton(button);
                p.buttons[i] = button;
            }

            p.layout = UI::FlowLayout::create(context);
            for (auto i : p.buttonGroup->getButtons())
            {
                p.layout->addChild(i);
            }
            addChild(p.layout);

            auto weak = std::weak_ptr<ImageRotateSettingsWidget>(std::dynamic_pointer_cast<ImageRotateSettingsWidget>(shared_from_this()));
            p.buttonGroup->setRadioCallback(
                [weak, context](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
                        {
                            imageSettings->setImageRotate(static_cast<ImageRotate>(value));
                        }
                    }
                });

            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto imageSettings = settingsSystem->getSettingsT<ImageSettings>())
            {
                p.rotateObserver = ValueObserver<ImageRotate>::create(
                    imageSettings->observeImageRotate(),
                    [weak](ImageRotate value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->buttonGroup->setChecked(static_cast<int>(value));
                        }
                    });
            }
        }

        ImageRotateSettingsWidget::ImageRotateSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ImageRotateSettingsWidget> ImageRotateSettingsWidget::create(Context* context)
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

        void ImageRotateSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            for (auto i : getImageRotateEnums())
            {
                std::stringstream ss;
                ss << i;
                p.buttons[i]->setText(_getText(ss.str()));
            }
        }

    } // namespace ViewApp
} // namespace djv


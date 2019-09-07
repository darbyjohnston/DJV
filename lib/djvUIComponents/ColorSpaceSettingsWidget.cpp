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

#include <djvUIComponents/ColorSpaceSettingsWidget.h>

#include <djvUIComponents/ColorSpaceModel.h>

#include <djvUI/ColorSpaceSettings.h>
#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/SettingsSystem.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct ColorSpaceSettingsWidget::Private
        {
            std::shared_ptr<ColorSpaceModel> model;

            std::shared_ptr<ComboBox> inputColorSpaceComboBox;
            std::shared_ptr<ComboBox> displayComboBox;
            std::shared_ptr<ComboBox> viewComboBox;
            std::shared_ptr<FormLayout> formLayout;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<std::string> > displaysObserver;
            std::shared_ptr<ListObserver<std::string> > viewsObserver;
            std::shared_ptr<ValueObserver<std::string> > inputColorSpaceObserver;
            std::shared_ptr<ValueObserver<std::string> > colorSpaceObserver;
            std::shared_ptr<ValueObserver<std::string> > displayObserver;
            std::shared_ptr<ValueObserver<std::string> > displayObserver2;
            std::shared_ptr<ValueObserver<std::string> > viewObserver;
            std::shared_ptr<ValueObserver<std::string> > viewObserver2;
        };

        void ColorSpaceSettingsWidget::_init(const std::shared_ptr<Context>& context)
        {
            ISettingsWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::UI::ColorSpaceSettingsWidget");

            p.model = ColorSpaceModel::create(context);

            p.inputColorSpaceComboBox = ComboBox::create(context);
            p.displayComboBox = ComboBox::create(context);
            p.viewComboBox = ComboBox::create(context);

            p.formLayout = FormLayout::create(context);
            p.formLayout->addChild(p.inputColorSpaceComboBox);
            p.formLayout->addChild(p.displayComboBox);
            p.formLayout->addChild(p.viewComboBox);
            addChild(p.formLayout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceSettingsWidget>(std::dynamic_pointer_cast<ColorSpaceSettingsWidget>(shared_from_this()));
            auto contextWeak = std::weak_ptr<Context>(context);
            p.inputColorSpaceComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            auto colorSpaceSettings = settingsSystem->getSettingsT<Settings::ColorSpace>();
                            colorSpaceSettings->setInputColorSpace(widget->_p->model->indexToColorSpace(value));
                        }
                    }
                });

            p.displayComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            auto colorSpaceSettings = settingsSystem->getSettingsT<Settings::ColorSpace>();
                            colorSpaceSettings->setDisplay(widget->_p->model->indexToDisplay(value));
                        }
                    }
                });

            p.viewComboBox->setCallback(
                [weak, contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        if (auto widget = weak.lock())
                        {
                            auto settingsSystem = context->getSystemT<Settings::System>();
                            auto colorSpaceSettings = settingsSystem->getSettingsT<Settings::ColorSpace>();
                            colorSpaceSettings->setView(widget->_p->model->indexToView(value));
                        }
                    }
                });

            p.colorSpacesObserver = ListObserver<std::string>::create(
                p.model->observeColorSpaces(),
                [weak](const std::vector<std::string>&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });

            p.displaysObserver = ListObserver<std::string>::create(
                p.model->observeDisplays(),
                [weak](const std::vector<std::string>&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });

            p.viewsObserver = ListObserver<std::string>::create(
                p.model->observeViews(),
                [weak](const std::vector<std::string>&)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_widgetUpdate();
                    }
                });

            p.colorSpaceObserver = ValueObserver<std::string>::create(
                p.model->observeColorSpace(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->inputColorSpaceComboBox->setCurrentItem(widget->_p->model->colorSpaceToIndex(value));
                    }
                });

            p.displayObserver = ValueObserver<std::string>::create(
                p.model->observeDisplay(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displayComboBox->setCurrentItem(widget->_p->model->displayToIndex(value));
                    }
                });

            p.viewObserver = ValueObserver<std::string>::create(
                p.model->observeView(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewComboBox->setCurrentItem(widget->_p->model->viewToIndex(value));
                    }
                });

            auto settingsSystem = context->getSystemT<Settings::System>();
            auto colorSpaceSettings = settingsSystem->getSettingsT<Settings::ColorSpace>();
            p.inputColorSpaceObserver = ValueObserver<std::string>::create(
                colorSpaceSettings->observeInputColorSpace(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setColorSpace(value);
                    }
                });

            p.displayObserver2 = ValueObserver<std::string>::create(
                colorSpaceSettings->observeDisplay(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setDisplay(value);
                    }
                });

            p.viewObserver2 = ValueObserver<std::string>::create(
                colorSpaceSettings->observeView(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setView(value);
                    }
                });
        }

        ColorSpaceSettingsWidget::ColorSpaceSettingsWidget() :
            _p(new Private)
        {}

        std::shared_ptr<ColorSpaceSettingsWidget> ColorSpaceSettingsWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceSettingsWidget>(new ColorSpaceSettingsWidget);
            out->_init(context);
            return out;
        }

        std::string ColorSpaceSettingsWidget::getSettingsName() const
        {
            return DJV_TEXT("Color Space");
        }

        std::string ColorSpaceSettingsWidget::getSettingsGroup() const
        {
            return DJV_TEXT("General");
        }

        std::string ColorSpaceSettingsWidget::getSettingsSortKey() const
        {
            return "0";
        }

        void ColorSpaceSettingsWidget::_localeEvent(Event::Locale& event)
        {
            ISettingsWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.formLayout->setText(p.inputColorSpaceComboBox, _getText(DJV_TEXT("Input")) + ":");
            p.formLayout->setText(p.displayComboBox, _getText(DJV_TEXT("Display")) + ":");
            p.formLayout->setText(p.viewComboBox, _getText(DJV_TEXT("View")) + ":");
            _widgetUpdate();
        }

        void ColorSpaceSettingsWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                p.inputColorSpaceComboBox->clearItems();
                for (const auto& i : p.model->observeColorSpaces()->get())
                {
                    p.inputColorSpaceComboBox->addItem(i);
                }
                const std::string& colorSpace = p.model->observeColorSpace()->get();
                p.inputColorSpaceComboBox->setCurrentItem(p.model->colorSpaceToIndex(colorSpace));

                p.displayComboBox->clearItems();
                for (const auto& i : p.model->observeDisplays()->get())
                {
                    p.displayComboBox->addItem(i);
                }
                p.displayComboBox->setCurrentItem(p.model->displayToIndex(p.model->observeDisplay()->get()));

                p.viewComboBox->clearItems();
                for (const auto& i : p.model->observeViews()->get())
                {
                    p.viewComboBox->addItem(i);
                }
                p.viewComboBox->setCurrentItem(p.model->viewToIndex(p.model->observeView()->get()));
            }
        }

    } // namespace UI
} // namespace djv


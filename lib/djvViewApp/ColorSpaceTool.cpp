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

#include <djvViewApp/ColorSpaceTool.h>

#include <djvViewApp/ImageView.h>
#include <djvViewApp/MDIWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>

#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceTool::Private
        {
            std::shared_ptr<MDIWidget> activeWidget;
            std::vector<std::string> colorSpaces;
            std::vector<AV::OCIODisplay> colorDisplays;
            std::string inputColorSpace;
            std::string colorDisplay;
            std::string colorView;

            std::shared_ptr<UI::ComboBox> inputColorSpaceComboBox;
            std::shared_ptr<UI::ComboBox> colorDisplayComboBox;
            std::shared_ptr<UI::ComboBox> colorViewComboBox;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<ValueObserver<std::shared_ptr<MDIWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<AV::Render::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<AV::OCIODisplay> > colorDisplaysObserver;
            std::shared_ptr<ValueObserver<std::string> > colorDisplayObserver;
            std::shared_ptr<ValueObserver<std::string> > colorViewObserver;
        };

        void ColorSpaceTool::_init(Context * context)
        {
            ITool::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceTool");

            p.inputColorSpaceComboBox = UI::ComboBox::create(context);
            p.colorDisplayComboBox = UI::ComboBox::create(context);
            p.colorViewComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::Margin);
            p.layout->addChild(p.inputColorSpaceComboBox);
            p.layout->addChild(p.colorDisplayComboBox);
            p.layout->addChild(p.colorViewComboBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceTool>(std::dynamic_pointer_cast<ColorSpaceTool>(shared_from_this()));
            p.inputColorSpaceComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value >= 0 && value < widget->_p->colorSpaces.size())
                        {
                            if (auto activeWidget = widget->_p->activeWidget)
                            {
                                auto imageView = activeWidget->getImageView();
                                AV::Render::ImageOptions imageOptions = imageView->observeImageOptions()->get();
                                imageOptions.colorSpaceXForm.first = widget->_p->colorSpaces[value];
                                imageView->setImageOptions(imageOptions);
                            }
                        }
                    }
                });

            p.colorDisplayComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (value >= 0 && value < widget->_p->colorDisplays.size())
                        {
                            if (auto activeWidget = widget->_p->activeWidget)
                            {
                                auto imageView = activeWidget->getImageView();
                                imageView->setColorDisplay(widget->_p->colorDisplays[value].name);
                            }
                        }
                    }
                });

            p.colorViewComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto activeWidget = widget->_p->activeWidget)
                        {
                            for (size_t i = 0; i < widget->_p->colorDisplays.size(); ++i)
                            {
                                if (widget->_p->colorDisplay == widget->_p->colorDisplays[i].name)
                                {
                                    if (value >= 0 && value < widget->_p->colorDisplays[i].views.size())
                                    {
                                        auto imageView = activeWidget->getImageView();
                                        imageView->setColorView(widget->_p->colorDisplays[i].views[value].name);
                                    }
                                }
                            }
                        }
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MDIWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MDIWidget>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->activeWidget = value;
                            if (widget->_p->activeWidget)
                            {
                                auto imageView = widget->_p->activeWidget->getImageView();
                                widget->_p->imageOptionsObserver = ValueObserver<AV::Render::ImageOptions>::create(
                                    imageView->observeImageOptions(),
                                    [weak](const AV::Render::ImageOptions& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->inputColorSpace = value.colorSpaceXForm.first;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->colorDisplayObserver = ValueObserver<std::string>::create(
                                    imageView->observeColorDisplay(),
                                    [weak](const std::string& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->colorDisplay = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                                widget->_p->colorViewObserver = ValueObserver<std::string>::create(
                                    imageView->observeColorView(),
                                    [weak](const std::string& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->colorView = value;
                                            widget->_widgetUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageOptionsObserver.reset();
                                widget->_p->colorDisplayObserver.reset();
                                widget->_p->colorViewObserver.reset();
                            }
                            widget->_widgetUpdate();
                        }
                    });
            }

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
        }

        ColorSpaceTool::ColorSpaceTool() :
            _p(new Private)
        {}

        ColorSpaceTool::~ColorSpaceTool()
        {}

        std::shared_ptr<ColorSpaceTool> ColorSpaceTool::create(Context * context)
        {
            auto out = std::shared_ptr<ColorSpaceTool>(new ColorSpaceTool);
            out->_init(context);
            return out;
        }

        void ColorSpaceTool::_localeEvent(Event::Locale & event)
        {
            ITool::_localeEvent(event);
            setTitle(_getText(DJV_TEXT("Color Space")));
            _widgetUpdate();
        }

        void ColorSpaceTool::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();

            p.layout->setText(p.inputColorSpaceComboBox, _getText(DJV_TEXT("Input color space")) + ":");
            p.inputColorSpaceComboBox->setItems(p.colorSpaces);
            for (size_t i = 0; i < p.colorSpaces.size(); ++i)
            {
                if (p.inputColorSpace == p.colorSpaces[i])
                {
                    p.inputColorSpaceComboBox->setCurrentItem(i);
                    break;
                }
            }

            p.layout->setText(p.colorDisplayComboBox, _getText(DJV_TEXT("Display")) + ":");
            p.layout->setText(p.colorViewComboBox, _getText(DJV_TEXT("View")) + ":");
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

            const bool activeWidget = p.activeWidget.get();
            p.inputColorSpaceComboBox->setEnabled(activeWidget);
            p.colorDisplayComboBox->setEnabled(activeWidget);
            p.colorViewComboBox->setEnabled(activeWidget);
        }

    } // namespace ViewApp
} // namespace djv


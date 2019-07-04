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

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceModel.h>
#include <djvViewApp/ImageView.h>
#include <djvViewApp/MediaWidget.h>
#include <djvViewApp/WindowSystem.h>

#include <djvUI/ComboBox.h>
#include <djvUI/FormLayout.h>
#include <djvUI/RowLayout.h>

#include <djvAV/OCIOSystem.h>
#include <djvAV/Render2D.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            std::shared_ptr<ColorSpaceModel> model;
            std::shared_ptr<MediaWidget> activeWidget;
            std::string emptyLabel;

            std::shared_ptr<UI::ComboBox> colorSpaceComboBox;
            std::shared_ptr<UI::ComboBox> displayComboBox;
            std::shared_ptr<UI::ComboBox> viewComboBox;
            std::shared_ptr<UI::FormLayout> layout;

            std::shared_ptr<ListObserver<std::string> > colorSpacesObserver;
            std::shared_ptr<ListObserver<std::string> > displaysObserver;
            std::shared_ptr<ListObserver<std::string> > viewsObserver;
            std::shared_ptr<ValueObserver<std::string> > colorSpaceObserver;
            std::shared_ptr<ValueObserver<std::string> > displayObserver;
            std::shared_ptr<ValueObserver<std::string> > displayObserver2;
            std::shared_ptr<ValueObserver<std::string> > viewObserver;
            std::shared_ptr<ValueObserver<std::string> > viewObserver2;
            std::shared_ptr<ValueObserver<std::string> > outputColorSpaceObserver;
            std::shared_ptr<ValueObserver<std::shared_ptr<MediaWidget> > > activeWidgetObserver;
            std::shared_ptr<ValueObserver<AV::Render::ImageOptions> > imageOptionsObserver;
        };

        void ColorSpaceWidget::_init(Context * context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");
            p.model = ColorSpaceModel::create(context);

            p.colorSpaceComboBox = UI::ComboBox::create(context);
            p.displayComboBox = UI::ComboBox::create(context);
            p.viewComboBox = UI::ComboBox::create(context);

            p.layout = UI::FormLayout::create(context);
            p.layout->setMargin(UI::MetricsRole::Margin);
            p.layout->setShadowOverlay({ UI::Side::Top });
            p.layout->addChild(p.colorSpaceComboBox);
            p.layout->addChild(p.displayComboBox);
            p.layout->addChild(p.viewComboBox);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.colorSpaceComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setColorSpace(widget->_p->model->indexToColorSpace(value));
                    }
                });

            p.displayComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setDisplay(widget->_p->model->indexToDisplay(value));
                    }
                });

            p.viewComboBox->setCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setView(widget->_p->model->indexToView(value));
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
                        if (auto activeWidget = widget->_p->activeWidget)
                        {
                            auto imageView = activeWidget->getImageView();
                            AV::Render::ImageOptions imageOptions = imageView->observeImageOptions()->get();
                            imageOptions.colorSpace = value;
                            imageView->setImageOptions(imageOptions);
                        }
                        widget->_widgetUpdate();
                    }
                });

            p.displayObserver = ValueObserver<std::string>::create(
                p.model->observeDisplay(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto activeWidget = widget->_p->activeWidget)
                        {
                            auto imageView = activeWidget->getImageView();
                            imageView->setColorDisplay(value);
                        }
                        widget->_widgetUpdate();
                    }
                });

            p.viewObserver = ValueObserver<std::string>::create(
                p.model->observeView(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto activeWidget = widget->_p->activeWidget)
                        {
                            auto imageView = activeWidget->getImageView();
                            imageView->setColorView(value);
                        }
                        widget->_widgetUpdate();
                    }
                });

            p.outputColorSpaceObserver = ValueObserver<std::string>::create(
                p.model->observeOutputColorSpace(),
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto activeWidget = widget->_p->activeWidget)
                        {
                            auto imageView = activeWidget->getImageView();
                            AV::Render::ImageOptions imageOptions = imageView->observeImageOptions()->get();
                            imageOptions.displayColorSpace = value;
                            imageView->setImageOptions(imageOptions);
                        }
                        widget->_widgetUpdate();
                    }
                });

            if (auto windowSystem = context->getSystemT<WindowSystem>())
            {
                p.activeWidgetObserver = ValueObserver<std::shared_ptr<MediaWidget> >::create(
                    windowSystem->observeActiveWidget(),
                    [weak](const std::shared_ptr<MediaWidget>& value)
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
                                            widget->_p->model->setColorSpace(value.colorSpace);
                                        }
                                    });
                                widget->_p->displayObserver2 = ValueObserver<std::string>::create(
                                    imageView->observeColorDisplay(),
                                    [weak](const std::string& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->model->setDisplay(value);
                                        }
                                    });
                                widget->_p->viewObserver2 = ValueObserver<std::string>::create(
                                    imageView->observeColorView(),
                                    [weak](const std::string& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->model->setView(value);
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->imageOptionsObserver.reset();
                                widget->_p->displayObserver2.reset();
                                widget->_p->viewObserver2.reset();
                            }
                            widget->_widgetUpdate();
                        }
                    });
            }
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {}

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(Context * context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceWidget::_localeEvent(Event::Locale & event)
        {
            MDIWidget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            setTitle(_getText(DJV_TEXT("Color Space")));
            p.emptyLabel = _getText(DJV_TEXT("(empty)"));
            p.layout->setText(p.colorSpaceComboBox, _getText(DJV_TEXT("Input:")));
            p.layout->setText(p.displayComboBox, _getText(DJV_TEXT("Display:")));
            p.layout->setText(p.viewComboBox, _getText(DJV_TEXT("View:")));
            _widgetUpdate();
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            p.colorSpaceComboBox->clearItems();
            for (const auto& i : p.model->observeColorSpaces()->get())
            {
                p.colorSpaceComboBox->addItem(!i.empty() ? i : "-");
            }
            const std::string& colorSpace = p.model->observeColorSpace()->get();
            p.colorSpaceComboBox->setCurrentItem(p.model->colorSpaceToIndex(colorSpace));

            p.displayComboBox->clearItems();
            for (const auto& i : p.model->observeDisplays()->get())
            {
                p.displayComboBox->addItem(!i.empty() ? i : "-");
            }
            p.displayComboBox->setCurrentItem(p.model->displayToIndex(p.model->observeDisplay()->get()));

            p.viewComboBox->clearItems();
            for (const auto& i : p.model->observeViews()->get())
            {
                p.viewComboBox->addItem(!i.empty() ? i : "-");
            }
            p.viewComboBox->setCurrentItem(p.model->viewToIndex(p.model->observeView()->get()));
        }

    } // namespace ViewApp
} // namespace djv


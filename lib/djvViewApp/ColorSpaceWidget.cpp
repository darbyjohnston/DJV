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

#include <djvUI/ButtonGroup.h>
#include <djvUI/FormLayout.h>
#include <djvUI/Label.h>
#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/TabWidget.h>

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

            std::shared_ptr<UI::ButtonGroup> colorSpaceButtonGroup;
            std::shared_ptr<UI::ButtonGroup> displayButtonGroup;
            std::shared_ptr<UI::ButtonGroup> viewButtonGroup;
            std::shared_ptr<UI::TabWidget> tabWidget;
            std::map<std::string, size_t> tabIDs;
            std::shared_ptr<UI::ScrollWidget> colorSpaceScrollWidget;
            std::shared_ptr<UI::ScrollWidget> displayScrollWidget;
            std::shared_ptr<UI::ScrollWidget> viewScrollWidget;
            std::shared_ptr<UI::Label> inputColorSpaceLabel;
            std::shared_ptr<UI::Label> outputColorSpaceLabel;
            std::shared_ptr<UI::FormLayout> formLayout;
            std::shared_ptr<UI::VerticalLayout> colorSpaceLayout;
            std::shared_ptr<UI::VerticalLayout> displayLayout;
            std::shared_ptr<UI::VerticalLayout> viewLayout;
            std::shared_ptr<UI::VerticalLayout> layout;

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

            p.colorSpaceButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.displayButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);
            p.viewButtonGroup = UI::ButtonGroup::create(UI::ButtonType::Exclusive);

            p.colorSpaceLayout = UI::VerticalLayout::create(context);
            p.colorSpaceLayout->setSpacing(UI::MetricsRole::None);
            p.colorSpaceScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.colorSpaceScrollWidget->setBorder(false);
            p.colorSpaceScrollWidget->setShadowOverlay({ UI::Side::Top });
            p.colorSpaceScrollWidget->addChild(p.colorSpaceLayout);

            p.displayLayout = UI::VerticalLayout::create(context);
            p.displayLayout->setSpacing(UI::MetricsRole::None);
            p.displayScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.displayScrollWidget->setBorder(false);
            p.displayScrollWidget->setShadowOverlay({ UI::Side::Top });
            p.displayScrollWidget->addChild(p.displayLayout);

            p.viewLayout = UI::VerticalLayout::create(context);
            p.viewLayout->setSpacing(UI::MetricsRole::None);
            p.viewScrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            p.viewScrollWidget->setBorder(false);
            p.viewScrollWidget->setShadowOverlay({ UI::Side::Top });
            p.viewScrollWidget->addChild(p.viewLayout);

            p.tabWidget = UI::TabWidget::create(context);
            p.tabWidget->setShadowOverlay({ UI::Side::Top });
            p.tabIDs["ColorSpace"] = p.tabWidget->addTab(std::string(), p.colorSpaceScrollWidget);
            p.tabIDs["Display"] = p.tabWidget->addTab(std::string(), p.displayScrollWidget);
            p.tabIDs["View"] = p.tabWidget->addTab(std::string(), p.viewScrollWidget);

            p.inputColorSpaceLabel = UI::Label::create(context);
            p.inputColorSpaceLabel->setTextHAlign(UI::TextHAlign::Left);
            p.outputColorSpaceLabel = UI::Label::create(context);
            p.outputColorSpaceLabel->setTextHAlign(UI::TextHAlign::Left);

            p.formLayout = UI::FormLayout::create(context);
            p.formLayout->setMargin(UI::MetricsRole::Margin);
            p.formLayout->setShadowOverlay({ UI::Side::Top });
            p.formLayout->addChild(p.inputColorSpaceLabel);
            p.formLayout->addChild(p.outputColorSpaceLabel);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->addChild(p.tabWidget);
            p.layout->addChild(p.formLayout);
            addChild(p.layout);

            _widgetUpdate();

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.colorSpaceButtonGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setColorSpace(widget->_p->model->indexToColorSpace(value));
                    }
                });

            p.displayButtonGroup->setExclusiveCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->model->setDisplay(widget->_p->model->indexToDisplay(value));
                    }
                });

            p.viewButtonGroup->setExclusiveCallback(
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
                            imageOptions.colorXForm.first = value;
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
                            imageOptions.colorXForm.second = value;
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
                                            widget->_p->model->setColorSpace(value.colorXForm.first);
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
            p.formLayout->setText(p.inputColorSpaceLabel, _getText(DJV_TEXT("Input:")));
            p.formLayout->setText(p.outputColorSpaceLabel, _getText(DJV_TEXT("Output:")));
            p.tabWidget->setText(p.tabIDs["ColorSpace"], _getText(DJV_TEXT("Color Space")));
            p.tabWidget->setText(p.tabIDs["Display"], _getText(DJV_TEXT("Display")));
            p.tabWidget->setText(p.tabIDs["View"], _getText(DJV_TEXT("View")));
            _widgetUpdate();
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            auto context = getContext();
            const bool activeWidget = p.activeWidget.get();
            p.colorSpaceButtonGroup->clearButtons();
            p.colorSpaceLayout->clearChildren();
            for (const auto& i : p.model->observeColorSpaces()->get())
            {
                auto button = UI::ListButton::create(context);
                button->setText(i);
                button->setEnabled(activeWidget);
                p.colorSpaceLayout->addChild(button);
                p.colorSpaceButtonGroup->addButton(button);
            }
            const std::string& colorSpace = p.model->observeColorSpace()->get();
            p.colorSpaceButtonGroup->setChecked(p.model->colorSpaceToIndex(colorSpace));

            p.displayButtonGroup->clearButtons();
            p.displayLayout->clearChildren();
            for (const auto& i : p.model->observeDisplays()->get())
            {
                auto button = UI::ListButton::create(context);
                button->setText(i);
                button->setEnabled(activeWidget);
                p.displayLayout->addChild(button);
                p.displayButtonGroup->addButton(button);
            }
            p.displayButtonGroup->setChecked(p.model->displayToIndex(p.model->observeDisplay()->get()));

            p.viewButtonGroup->clearButtons();
            p.viewLayout->clearChildren();
            for (const auto& i : p.model->observeViews()->get())
            {
                auto button = UI::ListButton::create(context);
                button->setText(i);
                button->setEnabled(activeWidget);
                p.viewLayout->addChild(button);
                p.viewButtonGroup->addButton(button);
            }
            p.viewButtonGroup->setChecked(p.model->viewToIndex(p.model->observeView()->get()));

            p.inputColorSpaceLabel->setText(!colorSpace.empty() ? colorSpace : p.emptyLabel);
            const std::string& outputColorSpace = p.model->observeOutputColorSpace()->get();
            p.outputColorSpaceLabel->setText(!outputColorSpace.empty() ? outputColorSpace : p.emptyLabel);
        }

    } // namespace ViewApp
} // namespace djv


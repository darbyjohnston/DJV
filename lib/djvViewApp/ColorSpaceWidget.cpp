// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidget.h>

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUI/ListButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/SoloLayout.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceWidget::Private
        {
            AV::OCIO::ConfigData configData;
            AV::OCIO::DisplayData displayData;
            AV::OCIO::ViewData viewData;

            std::map<std::string, std::shared_ptr<UI::ListButton> > buttons;
            std::shared_ptr<ColorSpaceConfigsWidget> configsWidget;
            std::shared_ptr<ColorSpaceDisplaysWidget> displaysWidget;
            std::shared_ptr<ColorSpaceViewsWidget> viewsWidget;
            std::shared_ptr<FileColorSpacesWidget> filesWidget;
            std::shared_ptr<FileColorSpaceWidget> fileWidget;
            std::shared_ptr<UI::VerticalLayout> mainLayout;
            std::shared_ptr<UI::SoloLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::ConfigData> > configDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::DisplayData> > displayDataObserver;
            std::shared_ptr<ValueObserver<AV::OCIO::ViewData> > viewDataObserver;
            std::shared_ptr<MapObserver<std::string, std::string> > fileColorSpacesObserver;
        };

        void ColorSpaceWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceWidget");

            p.buttons["Config"] = UI::ListButton::create(context);
            p.buttons["Display"] = UI::ListButton::create(context);
            p.buttons["View"] = UI::ListButton::create(context);
            for (const auto& i : p.buttons)
            {
                i.second->setRightIcon("djvIconArrowSmallRight");
            }

            p.configsWidget = ColorSpaceConfigsWidget::create(context);
            p.displaysWidget = ColorSpaceDisplaysWidget::create(context);
            p.viewsWidget = ColorSpaceViewsWidget::create(context);
            p.filesWidget = FileColorSpacesWidget::create(context);
            p.fileWidget = FileColorSpaceWidget::create(context);

            p.mainLayout = UI::VerticalLayout::create(context);
            p.mainLayout->setSpacing(UI::MetricsRole::None);
            p.mainLayout->addChild(p.buttons["Config"]);
            p.mainLayout->addChild(p.buttons["Display"]);
            p.mainLayout->addChild(p.buttons["View"]);
            p.mainLayout->addSeparator();
            p.mainLayout->addChild(p.filesWidget);
            p.mainLayout->setStretch(p.filesWidget, UI::RowStretch::Expand);
            p.layout = UI::SoloLayout::create(context);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.mainLayout);
            p.layout->addChild(p.configsWidget);
            p.layout->addChild(p.displaysWidget);
            p.layout->addChild(p.viewsWidget);
            p.layout->addChild(p.fileWidget);
            addChild(p.layout);

            auto weak = std::weak_ptr<ColorSpaceWidget>(std::dynamic_pointer_cast<ColorSpaceWidget>(shared_from_this()));
            p.buttons["Config"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->configsWidget, UI::Side::Left);
                    }
                });
            p.buttons["Display"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->displaysWidget, UI::Side::Left);
                    }
                });
            p.buttons["View"]->setClickedCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->viewsWidget, UI::Side::Left);
                    }
                });
            p.filesWidget->setCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->fileWidget->setFile(value);
                        widget->_p->layout->setCurrentWidget(widget->_p->fileWidget, UI::Side::Left);
                    }
                });

            p.configsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.displaysWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.viewsWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });
            p.fileWidget->setBackCallback(
                [weak]
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->layout->setCurrentWidget(widget->_p->mainLayout, UI::Side::Right);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.configDataObserver = ValueObserver<AV::OCIO::ConfigData>::create(
                ocioSystem->observeConfigData(),
                [weak](const AV::OCIO::ConfigData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->configData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.displayDataObserver = ValueObserver<AV::OCIO::DisplayData>::create(
                ocioSystem->observeDisplayData(),
                [weak](const AV::OCIO::DisplayData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displayData = value;
                        widget->_widgetUpdate();
                    }
                });
            p.viewDataObserver = ValueObserver<AV::OCIO::ViewData>::create(
                ocioSystem->observeViewData(),
                [weak](const AV::OCIO::ViewData& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->viewData = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceWidget::ColorSpaceWidget() :
            _p(new Private)
        {}

        ColorSpaceWidget::~ColorSpaceWidget()
        {}

        std::shared_ptr<ColorSpaceWidget> ColorSpaceWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceWidget>(new ColorSpaceWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            DJV_PRIVATE_PTR();
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("widget_color_space")));
                p.buttons["Config"]->setText(_getText(DJV_TEXT("widget_color_space_config")) + ":");
                p.buttons["Display"]->setText(_getText(DJV_TEXT("widget_color_space_display")) + ":");
                p.buttons["View"]->setText(_getText(DJV_TEXT("widget_color_space_view")) + ":");
                _widgetUpdate();
            }
        }

        void ColorSpaceWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            p.buttons["Config"]->setRightText(
                p.configData.current >= 0 && p.configData.current < static_cast<int>(p.configData.names.size()) ?
                p.configData.names[p.configData.current] :
                _getText(DJV_TEXT("av_ocio_config_none")));
            p.buttons["Display"]->setRightText(
                p.displayData.current >= 0 && p.displayData.current < static_cast<int>(p.displayData.names.size()) ?
                p.displayData.names[p.displayData.current] :
                _getText(DJV_TEXT("av_ocio_display_none")));
            p.buttons["View"]->setRightText(
                p.viewData.current >= 0 && p.viewData.current < static_cast<int>(p.viewData.names.size()) ?
                p.viewData.names[p.viewData.current] :
                _getText(DJV_TEXT("av_ocio_view_none")));
        }

    } // namespace ViewApp
} // namespace djv


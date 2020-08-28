// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/ColorSpaceWidgetPrivate.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/FormLayout.h>
#include <djvUI/ListWidget.h>
#include <djvUI/PopupButton.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>
#include <djvUI/Spacer.h>
#include <djvUI/ToolBar.h>

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceDisplayWidget::Private
        {
            AV::OCIO::Displays displays;

            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::Displays> > displaysObserver;
        };

        void ColorSpaceDisplayWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceDisplayWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UI::SearchBox::create(context);
            
            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            auto toolBar = UI::ToolBar::create(context);
            toolBar->addChild(p.searchBox);
            toolBar->setStretch(p.searchBox, UI::RowStretch::Expand);
            p.layout->addChild(toolBar);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
                        ocioSystem->setCurrentDisplay(value);
                    }
                });

            auto weak = std::weak_ptr<ColorSpaceDisplayWidget>(std::dynamic_pointer_cast<ColorSpaceDisplayWidget>(shared_from_this()));
            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.displaysObserver = ValueObserver<AV::OCIO::Displays>::create(
                ocioSystem->observeDisplays(),
                [weak](const AV::OCIO::Displays& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->displays = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceDisplayWidget::ColorSpaceDisplayWidget() :
            _p(new Private)
        {}

        ColorSpaceDisplayWidget::~ColorSpaceDisplayWidget()
        {}

        std::shared_ptr<ColorSpaceDisplayWidget> ColorSpaceDisplayWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceDisplayWidget>(new ColorSpaceDisplayWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceDisplayWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceDisplayWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceDisplayWidget::_initEvent(Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void ColorSpaceDisplayWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.displays.first.size(); ++i)
                {
                    const auto& display = p.displays.first[i];
                    UI::ListItem item;
                    item.text = !display.empty() ? display : _getText(DJV_TEXT("av_ocio_display_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items);
                p.listWidget->setChecked(p.displays.second);
            }
        }

    } // namespace ViewApp
} // namespace djv


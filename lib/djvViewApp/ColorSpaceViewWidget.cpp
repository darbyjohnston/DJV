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

#include <djvOCIO/OCIOSystem.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceViewWidget::Private
        {
            OCIO::Views views;

            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UIComponents::SearchBox> searchBox;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<Observer::Value<OCIO::Views> > viewsObserver;
        };

        void ColorSpaceViewWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceViewWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UIComponents::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            auto hLayout = UI::HorizontalLayout::create(context);
            hLayout->setSpacing(UI::MetricsRole::None);
            hLayout->addChild(p.searchBox);
            hLayout->setStretch(p.searchBox, UI::RowStretch::Expand);
            p.layout->addChild(hLayout);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<System::Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
                        ocioSystem->setCurrentView(value);
                    }
                });

            auto weak = std::weak_ptr<ColorSpaceViewWidget>(std::dynamic_pointer_cast<ColorSpaceViewWidget>(shared_from_this()));
            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->listWidget->setFilter(value);
                    }
                });

            auto ocioSystem = context->getSystemT<OCIO::OCIOSystem>();
            p.viewsObserver = Observer::Value<OCIO::Views>::create(
                ocioSystem->observeViews(),
                [weak](const OCIO::Views& value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_p->views = value;
                        widget->_widgetUpdate();
                    }
                });
        }

        ColorSpaceViewWidget::ColorSpaceViewWidget() :
            _p(new Private)
        {}

        ColorSpaceViewWidget::~ColorSpaceViewWidget()
        {}

        std::shared_ptr<ColorSpaceViewWidget> ColorSpaceViewWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceViewWidget>(new ColorSpaceViewWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceViewWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceViewWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceViewWidget::_initEvent(System::Event::Init& event)
        {
            if (event.getData().text)
            {
                _widgetUpdate();
            }
        }

        void ColorSpaceViewWidget::_widgetUpdate()
        {
            DJV_PRIVATE_PTR();
            if (auto context = getContext().lock())
            {
                std::vector<UI::ListItem> items;
                for (size_t i = 0; i < p.views.first.size(); ++i)
                {
                    const auto& view = p.views.first[i];
                    UI::ListItem item;
                    item.text = !view.empty() ? view : _getText(DJV_TEXT("av_ocio_view_none"));
                    items.emplace_back(item);
                }
                p.listWidget->setItems(items);
                p.listWidget->setChecked(p.views.second);
            }
        }

    } // namespace ViewApp
} // namespace djv


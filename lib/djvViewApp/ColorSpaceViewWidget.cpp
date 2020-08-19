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

#include <djvAV/OCIOSystem.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorSpaceViewWidget::Private
        {
            AV::OCIO::Views views;

            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::VerticalLayout> layout;

            std::shared_ptr<ValueObserver<AV::OCIO::Views> > viewsObserver;
        };

        void ColorSpaceViewWidget::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();

            setClassName("djv::ViewApp::ColorSpaceViewWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setAlternateRowsRoles(UI::ColorRole::None, UI::ColorRole::Trough);

            p.searchBox = UI::SearchBox::create(context);
            p.searchBox->setMargin(UI::MetricsRole::MarginSmall);

            p.layout = UI::VerticalLayout::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Both, context);
            scrollWidget->setBorder(false);
            scrollWidget->addChild(p.listWidget);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.listWidget->setRadioCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto ocioSystem = context->getSystemT<AV::OCIO::System>();
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

            auto ocioSystem = context->getSystemT<AV::OCIO::System>();
            p.viewsObserver = ValueObserver<AV::OCIO::Views>::create(
                ocioSystem->observeViews(),
                [weak](const AV::OCIO::Views& value)
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

        std::shared_ptr<ColorSpaceViewWidget> ColorSpaceViewWidget::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<ColorSpaceViewWidget>(new ColorSpaceViewWidget);
            out->_init(context);
            return out;
        }

        void ColorSpaceViewWidget::_preLayoutEvent(Event::PreLayout&)
        {
            _setMinimumSize(_p->layout->getMinimumSize());
        }

        void ColorSpaceViewWidget::_layoutEvent(Event::Layout&)
        {
            _p->layout->setGeometry(getGeometry());
        }

        void ColorSpaceViewWidget::_initEvent(Event::Init& event)
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


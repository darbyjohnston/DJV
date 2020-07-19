// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LayersWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/ListWidget.h>
#include <djvUI/RowLayout.h>
#include <djvUI/ScrollWidget.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct LayersWidget::Private
        {
            std::shared_ptr<Media> currentMedia;
            std::vector<AV::IO::VideoInfo> layers;
            int currentLayer = -1;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<std::pair<std::vector<AV::IO::VideoInfo>, int> > > layersObserver;
        };

        void LayersWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::LayersWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            auto scrollWidget = UI::ScrollWidget::create(UI::ScrollType::Vertical, context);
            scrollWidget->setBorder(false);
            scrollWidget->setShadowOverlay({ UI::Side::Top });
            scrollWidget->addChild(p.listWidget);

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(scrollWidget);
            p.layout->setStretch(scrollWidget, UI::Layout::RowStretch::Expand);
            p.layout->addSeparator();
            p.layout->addChild(p.searchBox);
            addChild(p.layout);

            auto weak = std::weak_ptr<LayersWidget>(std::dynamic_pointer_cast<LayersWidget>(shared_from_this()));
            p.listWidget->setRadioCallback(
                [weak](int value)
                {
                    if (auto widget = weak.lock())
                    {
                        if (auto media = widget->_p->currentMedia)
                        {
                            if (value >= 0)
                            {
                                media->setLayer(static_cast<size_t>(value));
                            }
                        }
                    }
                });

            p.searchBox->setFilterCallback(
                [weak](const std::string& value)
            {
                if (auto widget = weak.lock())
                {
                    widget->_p->listWidget->setFilter(value);
                }
            });

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = ValueObserver<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentMedia = value;
                            if (value)
                            {
                                widget->_p->layersObserver = ValueObserver<std::pair<std::vector<AV::IO::VideoInfo>, int> >::create(
                                    value->observeLayers(),
                                    [weak](const std::pair<std::vector<AV::IO::VideoInfo>, int>& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->layers = value.first;
                                            widget->_p->currentLayer = value.second;
                                            widget->_layersUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->layers.clear();;
                                widget->_p->currentLayer = -1;
                                widget->_p->layersObserver.reset();
                                widget->_layersUpdate();
                            }
                        }
                    });
            }
        }

        LayersWidget::LayersWidget() :
            _p(new Private)
        {}

        LayersWidget::~LayersWidget()
        {}

        std::shared_ptr<LayersWidget> LayersWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<LayersWidget>(new LayersWidget);
            out->_init(context);
            return out;
        }

        void LayersWidget::_initEvent(Event::Init & event)
        {
            MDIWidget::_initEvent(event);
            if (event.getData().text)
            {
                setTitle(_getText(DJV_TEXT("layers_title")));
                _layersUpdate();
            }
        }

        void LayersWidget::_layersUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (const auto& i : p.layers)
            {
                items.push_back(_getText(i.info.name));
            }
            p.listWidget->setItems(items);
            p.listWidget->setChecked(p.currentLayer);
        }

    } // namespace ViewApp
} // namespace djv


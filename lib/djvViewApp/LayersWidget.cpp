// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LayersWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUIComponents/SearchBox.h>

#include <djvUI/ListWidget.h>
#include <djvUI/RowLayout.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct LayersWidget::Private
        {
            std::shared_ptr<Media> currentMedia;
            AV::IO::Info info;
            size_t layer = 0;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<UI::SearchBox> searchBox;
            std::shared_ptr<UI::VerticalLayout> layout;
            std::shared_ptr<ValueObserver<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<ValueObserver<AV::IO::Info> > infoObserver;
            std::shared_ptr<ValueObserver<size_t> > layerObserver;
        };

        void LayersWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            MDIWidget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::LayersWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setBorder(false);
            p.listWidget->setShadowOverlay({ UI::Side::Top });

            p.searchBox = UI::SearchBox::create(context);

            p.layout = UI::VerticalLayout::create(context);
            p.layout->setSpacing(UI::MetricsRole::None);
            p.layout->setBackgroundRole(UI::ColorRole::Background);
            p.layout->addChild(p.listWidget);
            p.layout->setStretch(p.listWidget, UI::Layout::RowStretch::Expand);
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
                                widget->_p->infoObserver = ValueObserver<AV::IO::Info>::create(
                                    value->observeInfo(),
                                    [weak](const AV::IO::Info& value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->info = value;
                                            widget->_layersUpdate();
                                        }
                                    });
                                widget->_p->layerObserver = ValueObserver<size_t>::create(
                                    value->observeLayer(),
                                    [weak](size_t value)
                                    {
                                        if (auto widget = weak.lock())
                                        {
                                            widget->_p->layer = value;
                                            widget->_currentLayerUpdate();
                                        }
                                    });
                            }
                            else
                            {
                                widget->_p->info = AV::IO::Info();
                                widget->_p->layer = 0;
                                widget->_p->infoObserver.reset();
                                widget->_p->layerObserver.reset();
                                widget->_layersUpdate();
                                widget->_currentLayerUpdate();
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
            for (size_t i = 0; i < p.info.video.size(); ++i)
            {
                const auto& video = p.info.video[i];
                items.push_back(_getText(video.info.name));
            }
            p.listWidget->setItems(items);
        }

        void LayersWidget::_currentLayerUpdate()
        {
            DJV_PRIVATE_PTR();
            p.listWidget->setChecked(p.layer);
        }

    } // namespace ViewApp
} // namespace djv


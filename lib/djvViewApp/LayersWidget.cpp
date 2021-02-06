// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvViewApp/LayersWidget.h>

#include <djvViewApp/FileSystem.h>
#include <djvViewApp/Media.h>

#include <djvUI/ListWidget.h>
#include <djvUI/RowLayout.h>

#include <djvSystem/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct LayersWidget::Private
        {
            std::shared_ptr<Media> currentMedia;
            std::vector<Image::Info> layers;
            int currentLayer = -1;
            std::shared_ptr<UI::ListWidget> listWidget;
            std::shared_ptr<Observer::Value<std::shared_ptr<Media> > > currentMediaObserver;
            std::shared_ptr<Observer::Value<std::pair<std::vector<Image::Info>, int> > > layersObserver;
        };

        void LayersWidget::_init(const std::shared_ptr<System::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::LayersWidget");

            p.listWidget = UI::ListWidget::create(UI::ButtonType::Radio, context);
            p.listWidget->setRowColorRoles(UI::ColorRole::None, UI::ColorRole::Trough);
            addChild(p.listWidget);

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

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.currentMediaObserver = Observer::Value<std::shared_ptr<Media> >::create(
                    fileSystem->observeCurrentMedia(),
                    [weak](const std::shared_ptr<Media>& value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->currentMedia = value;
                            if (value)
                            {
                                widget->_p->layersObserver = Observer::Value<std::pair<std::vector<Image::Info>, int> >::create(
                                    value->observeLayers(),
                                    [weak](const std::pair<std::vector<Image::Info>, int>& value)
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

        std::shared_ptr<LayersWidget> LayersWidget::create(const std::shared_ptr<System::Context>& context)
        {
            auto out = std::shared_ptr<LayersWidget>(new LayersWidget);
            out->_init(context);
            return out;
        }

        void LayersWidget::setFilter(const std::string& value)
        {
            _p->listWidget->setFilter(value);
        }

        void LayersWidget::_preLayoutEvent(System::Event::PreLayout&)
        {
            _setMinimumSize(_p->listWidget->getMinimumSize());
        }

        void LayersWidget::_layoutEvent(System::Event::Layout&)
        {
            _p->listWidget->setGeometry(getGeometry());
        }

        void LayersWidget::_initEvent(System::Event::Init & event)
        {
            Widget::_initEvent(event);
            if (event.getData().text)
            {
                _layersUpdate();
            }
        }

        void LayersWidget::_layersUpdate()
        {
            DJV_PRIVATE_PTR();
            std::vector<std::string> items;
            for (const auto& i : p.layers)
            {
                items.push_back(_getText(i.name));
            }
            p.listWidget->setItems(items);
            p.listWidget->setChecked(p.currentLayer);
        }

    } // namespace ViewApp
} // namespace djv

